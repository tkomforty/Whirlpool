#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters/ParameterDefinitions.h"
#include "../JuceLibraryCode/JucePluginDefines.h"

//==============================================================================
RhythmicFilterSculptorAudioProcessor::RhythmicFilterSculptorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
      valueTreeState(*this, nullptr, "PARAMETERS", ParameterDefinitions::createParameterLayout())
#else
      : valueTreeState(*this, nullptr, "PARAMETERS", ParameterDefinitions::createParameterLayout())
#endif
{    
    // Initialize parameter pointers
    mixParameter = valueTreeState.getRawParameterValue(ParameterIDs::mix);
    filterTypeParameter = valueTreeState.getRawParameterValue(ParameterIDs::filterType);
    patternSelectParameter = valueTreeState.getRawParameterValue(ParameterIDs::patternSelect);
    tempoSyncParameter = valueTreeState.getRawParameterValue(ParameterIDs::tempoSync);
    intensityParameter = valueTreeState.getRawParameterValue(ParameterIDs::intensity);
    resonanceParameter = valueTreeState.getRawParameterValue(ParameterIDs::resonance);
    stepCountParameter = valueTreeState.getRawParameterValue(ParameterIDs::stepCount);
    outputGainParameter = valueTreeState.getRawParameterValue(ParameterIDs::masterOutputGain);
}

RhythmicFilterSculptorAudioProcessor::~RhythmicFilterSculptorAudioProcessor()
{
}

//==============================================================================
const juce::String RhythmicFilterSculptorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RhythmicFilterSculptorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RhythmicFilterSculptorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RhythmicFilterSculptorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RhythmicFilterSculptorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0; // Zero latency - critical for beat-synchronized effects
}

int RhythmicFilterSculptorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RhythmicFilterSculptorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RhythmicFilterSculptorAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String RhythmicFilterSculptorAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void RhythmicFilterSculptorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void RhythmicFilterSculptorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    // Validate sample rate for safety
    jassert(sampleRate >= 44100.0 && sampleRate <= 192000.0);
    
    // Initialize DSP components
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    filterBank.prepare(spec);
    patternSequencer.prepare(sampleRate);
    tempoSync.prepare(sampleRate);
    
    // Initialize parameter smoothers for audio safety (prevents zipper noise)
    // Much faster response times for immediate feel
    mixSmoother.reset(sampleRate, 0.01f);        // 10ms ramp time
    intensitySmoother.reset(sampleRate, 0.005f); // 5ms ramp time (very fast for modulation)
    resonanceSmoother.reset(sampleRate, 0.02f);  // 20ms ramp time for filter resonance
    outputGainSmoother.reset(sampleRate, 0.005f); // 5ms ramp for output gain (very fast)
    
    // Set initial values safely
    float initialMix = mixParameter ? mixParameter->load() : 100.0f;
    float initialIntensity = intensityParameter ? intensityParameter->load() : 50.0f;
    float initialResonance = resonanceParameter ? resonanceParameter->load() : 0.7f;
    float initialGain = outputGainParameter ? outputGainParameter->load() : 0.0f;
    
    // Convert mix from 0-100% to 0-1 for internal use
    mixSmoother.setCurrentAndTargetValue(initialMix / 100.0f);
    intensitySmoother.setCurrentAndTargetValue(initialIntensity);
    resonanceSmoother.setCurrentAndTargetValue(initialResonance);
    outputGainSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(initialGain));
    
    // Cache frequently used values
    lastFilterType = filterTypeParameter ? static_cast<int>(filterTypeParameter->load()) : 0;
    lastPatternSelect = patternSelectParameter ? static_cast<int>(patternSelectParameter->load()) : 1;
    lastStepCount = stepCountParameter ? static_cast<int>(stepCountParameter->load()) : 16;
    lastTempoSync = tempoSyncParameter ? static_cast<int>(tempoSyncParameter->load()) : 4;
}

void RhythmicFilterSculptorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RhythmicFilterSculptorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void RhythmicFilterSculptorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;
    
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const auto numSamples = buffer.getNumSamples();

    // Early validation - critical for safety
    if (numSamples == 0 || buffer.getNumChannels() == 0)
        return;

    // Clear any output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    // Update tempo sync with current playhead info
    auto playhead = getPlayHead();
    static double lastKnownBpm = 120.0;
    if (playhead != nullptr)
    {
        juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo = playhead->getPosition();
        if (positionInfo.hasValue())
        {
            // Convert new PositionInfo to old CurrentPositionInfo for compatibility
            juce::AudioPlayHead::CurrentPositionInfo oldFormat;
            oldFormat.isPlaying = positionInfo->getIsPlaying();
            double hostBpm = positionInfo->getBpm().orFallback(lastKnownBpm);
            if (hostBpm > 0.0 && hostBpm < 999.0)
                lastKnownBpm = hostBpm;
            oldFormat.bpm = lastKnownBpm;
            oldFormat.ppqPosition = positionInfo->getPpqPosition().orFallback(0.0);
            tempoSync.updatePlayheadInfo(oldFormat);
        }
    }

    // Check if parameters have changed (avoid unnecessary updates)
    int currentFilterType = filterTypeParameter ? static_cast<int>(filterTypeParameter->load()) : 0;
    int currentPatternSelect = patternSelectParameter ? static_cast<int>(patternSelectParameter->load()) : 1;
    int currentStepCount = stepCountParameter ? static_cast<int>(stepCountParameter->load()) : 16;
    int currentTempoSync = tempoSyncParameter ? static_cast<int>(tempoSyncParameter->load()) : 4;
    
    // Update components only when parameters change
    if (currentTempoSync != lastTempoSync)
    {
        tempoSync.updateTempoDivision(currentTempoSync);
        lastTempoSync = currentTempoSync;
    }
    
    if (currentPatternSelect != lastPatternSelect)
    {
        patternSequencer.setPattern(currentPatternSelect - 1); // Convert 1-8 to 0-7
        lastPatternSelect = currentPatternSelect;
    }
    
    if (currentStepCount != lastStepCount)
    {
        patternSequencer.setStepCount(currentStepCount);
        lastStepCount = currentStepCount;
    }
    
    if (currentFilterType != lastFilterType)
    {
        filterBank.setFilterType(static_cast<FilterBank::FilterType>(currentFilterType));
        lastFilterType = currentFilterType;
    }
    
    // Process tempo sync for this block
    // Always advance pattern, even if host is stopped, using fallback timing
    if (tempoSync.isPlaying() && tempoSync.getCurrentBPM() > 0.0)
    {
        tempoSync.processBlock(numSamples);
    }
    else
    {
        // Fallback: advance phase using last known BPM if host is stopped
        double fallbackBpm = lastKnownBpm;
        double fallbackSampleRate = currentSampleRate > 0.0 ? currentSampleRate : 44100.0;
        double beatsPerSecond = fallbackBpm / 60.0;
        int patternLen = lastStepCount > 0 ? lastStepCount : 16;
        int divisionIdx = lastTempoSync;
        // Get note value for current division
        double noteValue = 0.5; // default to 1/8
        {
            // Copy from TempoSync::tempoDivisions
            static constexpr double tempoDivs[] = {0.125, 0.25/3.0, 0.25, 0.5/3.0, 0.5, 1.0/3.0, 1.0, 2.0, 4.0, 8.0};
            if (divisionIdx >= 0 && divisionIdx < 10)
                noteValue = tempoDivs[divisionIdx];
        }
        double divisionsPerSecond = beatsPerSecond / noteValue;
        double stepsPerSecond = divisionsPerSecond;
        double phasesPerSecond = stepsPerSecond / patternLen;
        double phaseIncrement = phasesPerSecond / fallbackSampleRate;
        // Manually advance phase and step
        static double fallbackPhase = 0.0;
        fallbackPhase += phaseIncrement * numSamples;
        while (fallbackPhase >= 1.0) fallbackPhase -= 1.0;
        // Set fallback phase/step in pattern sequencer
        int fallbackStep = static_cast<int>(fallbackPhase * patternLen) % patternLen;
        patternSequencer.setTempoSync(nullptr); // disables host sync
        patternSequencer.setStepCount(patternLen);
        // Directly set current step for fallback
        // Set current step using new public setter
        patternSequencer.setCurrentStep(fallbackStep);
    }
    // Update pattern sequencer with tempo sync
    patternSequencer.setTempoSync(&tempoSync);

    // Update smoothers with current parameter values (safe parameter access)
    float mixPercent = mixParameter ? juce::jlimit(0.0f, 100.0f, mixParameter->load()) : 100.0f;
    mixSmoother.setTargetValue(mixPercent / 100.0f); // Convert to 0-1 range
    
    float intensityPercent = intensityParameter ? juce::jlimit(0.0f, 100.0f, intensityParameter->load()) : 50.0f;
    intensitySmoother.setTargetValue(intensityPercent);
    
    float resonanceParam = resonanceParameter ? juce::jlimit(0.1f, 10.0f, resonanceParameter->load()) : 0.7f;
    resonanceSmoother.setTargetValue(resonanceParam);
    
    float outputGainDb = outputGainParameter ? juce::jlimit(-24.0f, 12.0f, outputGainParameter->load()) : 0.0f;
    outputGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(outputGainDb));

    // Prepare dry buffer for wet/dry mix
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.setSize(buffer.getNumChannels(), numSamples, false, false, true);
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        dryBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);

    // Get pattern modulation once per block (performance optimization)
    float patternModulation = patternSequencer.getNextModulationValue();
    
    // Pre-calculate expensive operations once per block
    float intensityValue = intensitySmoother.getCurrentValue();
    float intensityNorm = intensityValue / 100.0f;
    float curvedIntensity = std::pow(intensityNorm, 0.7f); // Expensive calculation once
    
    const float minCutoff = 100.0f;
    const float maxCutoff = 8000.0f;
    
    // Process with reduced frequency updates for optimal balance
    int updateEveryNSamples = std::max(1, numSamples / 8); // Update 8 times per block
    
    for (int sample = 0; sample < numSamples; sample += updateEveryNSamples)
    {
        // Get smoothed values at regular intervals (not every sample)
        float currentResonance = resonanceSmoother.getNextValue();
        filterBank.setResonance(currentResonance);
        
        // Calculate modulated frequency
        float modulatedIntensity = curvedIntensity * (0.1f + 0.9f * patternModulation);
        float modulatedCutoff = minCutoff * std::pow(maxCutoff / minCutoff, modulatedIntensity);
        modulatedCutoff = juce::jlimit(minCutoff, maxCutoff, modulatedCutoff);
        
        // Update filter frequency
        filterBank.setFrequency(0, modulatedCutoff);
        
        // Skip ahead for next update
        for (int skip = 1; skip < updateEveryNSamples && (sample + skip) < numSamples; ++skip)
        {
            resonanceSmoother.getNextValue(); // Keep smoother in sync
        }
    }
    
    // Process the entire block through filter bank
    filterBank.processBlock(buffer);

    // Apply wet/dry mix using equal-power crossfade (audio best practice)
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* wetData = buffer.getWritePointer(channel);
        auto* dryData = dryBuffer.getReadPointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float mixValue = mixSmoother.getNextValue(); // 0 = dry, 1 = wet
            // Equal-power crossfade
            float wetGain = std::sin(juce::MathConstants<float>::halfPi * mixValue);
            float dryGain = std::cos(juce::MathConstants<float>::halfPi * mixValue);
            wetData[sample] = wetData[sample] * wetGain + dryData[sample] * dryGain;
        }
    }

    // Apply output gain with proper smoothing (fixed)
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        // Apply per-sample gain with proper smoother usage
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float currentGain = outputGainSmoother.getNextValue();
            channelData[sample] *= currentGain;
        }
    }
}

//==============================================================================
bool RhythmicFilterSculptorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RhythmicFilterSculptorAudioProcessor::createEditor()
{
    return new RhythmicFilterSculptorAudioProcessorEditor (*this);
}

//==============================================================================
void RhythmicFilterSculptorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = valueTreeState.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void RhythmicFilterSculptorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (valueTreeState.state.getType()))
            valueTreeState.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RhythmicFilterSculptorAudioProcessor();
}
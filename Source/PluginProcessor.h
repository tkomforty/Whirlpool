#pragma once

#include <algorithm>
#include <memory>
#include <atomic>
#include <JuceHeader.h>
#include "DSP/FilterBank.h"
#include "DSP/PatternSequencer.h"
#include "DSP/TempoSync.h"
#include "Parameters/ParameterDefinitions.h"

//==============================================================================
/**
*/
class RhythmicFilterSculptorAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    RhythmicFilterSculptorAudioProcessor();
    ~RhythmicFilterSculptorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Custom methods
    juce::AudioProcessorValueTreeState& getValueTreeState() { return valueTreeState; }

private:
    // Audio processing components
    FilterBank filterBank;
    PatternSequencer patternSequencer;
    TempoSync tempoSync;
    
    // Parameter management
    juce::AudioProcessorValueTreeState valueTreeState;
    // Remove createParameterLayout() - we'll use ParameterDefinitions::createParameterLayout()
    
    // Parameter pointers must come after valueTreeState
    std::atomic<float>* mixParameter = nullptr;
    std::atomic<float>* filterTypeParameter = nullptr;
    std::atomic<float>* patternSelectParameter = nullptr;
    std::atomic<float>* tempoSyncParameter = nullptr;
    std::atomic<float>* intensityParameter = nullptr;
    std::atomic<float>* resonanceParameter = nullptr;
    std::atomic<float>* stepCountParameter = nullptr;
    std::atomic<float>* outputGainParameter = nullptr;
    
    // Parameter smoothing for audio safety (prevents zipper noise)
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mixSmoother;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> intensitySmoother;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> resonanceSmoother;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> outputGainSmoother;
    
    // Parameter change tracking for performance optimization
    int lastFilterType = 0;
    int lastPatternSelect = 0;
    int lastStepCount = 16;
    int lastTempoSync = 0;
    
    // Internal state
    double currentSampleRate = 44100.0;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RhythmicFilterSculptorAudioProcessor)
};
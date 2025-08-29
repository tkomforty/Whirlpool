#include <JuceHeader.h>
#include "FilterBank.h"

FilterBank::FilterBank()
{
    // Initialize all band processors with safe defaults
    for (auto& processor : bandProcessors)
    {
        processor.currentType = FilterType::LowPass;
        processor.currentFreqValue = 1000.0f;
        processor.currentResValue = 0.7f;
    }
    
    currentFrequency = 1000.0f;
    currentResonance = 0.7f;
}

void FilterBank::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    
    // Validate sample rate for safety
    jassert(sampleRate >= 44100.0 && sampleRate <= 192000.0);
    
    // Prepare all band processors
    for (auto& processor : bandProcessors)
    {
        processor.filter.prepare(spec);
        processor.filter.reset();
        
        // Initialize parameter smoothers with safe ramp times
        processor.frequency.reset(sampleRate, 0.05);  // 50ms for frequency changes
        processor.resonance.reset(sampleRate, 0.1);   // 100ms for resonance
        processor.level.reset(sampleRate, 0.05);      // 50ms for level changes
        
        // Set initial values
        processor.frequency.setCurrentAndTargetValue(processor.currentFreqValue);
        processor.resonance.setCurrentAndTargetValue(processor.currentResValue);
        processor.level.setCurrentAndTargetValue(1.0f);
        
        // Configure initial filter state
        configureBandFilter(processor, processor.currentType);
    }
    
    // Initialize global parameter smoothers
    frequencySmoother.reset(sampleRate, 0.05);
    resonanceSmoother.reset(sampleRate, 0.1);
    
    frequencySmoother.setCurrentAndTargetValue(currentFrequency);
    resonanceSmoother.setCurrentAndTargetValue(currentResonance);
    
    // Prepare working buffer
    workingBuffer.setSize(static_cast<int>(spec.numChannels), 
                         static_cast<int>(spec.maximumBlockSize));
}

void FilterBank::reset()
{
    for (auto& processor : bandProcessors)
    {
        processor.filter.reset();
        processor.frequency.reset(sampleRate, 0.05);
        processor.resonance.reset(sampleRate, 0.1);
        processor.level.reset(sampleRate, 0.05);
    }
    
    frequencySmoother.reset(sampleRate, 0.05);
    resonanceSmoother.reset(sampleRate, 0.1);
}

void FilterBank::setFilterType(FilterType type)
{
    // Set filter type for the first band (simplified for now)
    if (isValidBandIndex(0))
    {
        bandProcessors[0].currentType = type;
        configureBandFilter(bandProcessors[0], type);
    }
}

void FilterBank::setFilterType(int bandIndex, FilterType type)
{
    if (isValidBandIndex(bandIndex))
    {
        bandProcessors[bandIndex].currentType = type;
        configureBandFilter(bandProcessors[bandIndex], type);
    }
}

void FilterBank::setResonance(float resonance)
{
    currentResonance = validateResonance(resonance);
    resonanceSmoother.setTargetValue(currentResonance);
    
    // Apply to all bands
    for (auto& processor : bandProcessors)
    {
        processor.currentResValue = currentResonance;
        processor.resonance.setTargetValue(currentResonance);
    }
}

void FilterBank::setResonance(int bandIndex, float resonance)
{
    if (isValidBandIndex(bandIndex))
    {
        float validRes = validateResonance(resonance);
        bandProcessors[bandIndex].currentResValue = validRes;
        bandProcessors[bandIndex].resonance.setTargetValue(validRes);
    }
}

void FilterBank::setFrequency(int bandIndex, float newFrequency)
{
    if (isValidBandIndex(bandIndex))
    {
        float validFreq = validateFrequency(newFrequency, sampleRate);
        bandProcessors[bandIndex].currentFreqValue = validFreq;
        bandProcessors[bandIndex].frequency.setTargetValue(validFreq);
    }
}

void FilterBank::setLevel(int bandIndex, float newLevel)
{
    if (isValidBandIndex(bandIndex))
    {
        float safeLevel = juce::jlimit(0.0f, 2.0f, newLevel);  // 0 to +6dB range
        bandProcessors[bandIndex].level.setTargetValue(safeLevel);
    }
}

void FilterBank::setModulationValue(float modulation)
{
    // Clamp modulation to safe range
    currentModulation = juce::jlimit(-1.0f, 1.0f, modulation);
    
    // Calculate modulated frequency (simple exponential mapping)
    float baseFrequency = 1000.0f;  // Center frequency
    float modulationRange = 3000.0f;  // ±3000 Hz range
    
    float modulatedFreq = baseFrequency + (currentModulation * modulationRange);
    float safeFreq = validateFrequency(modulatedFreq, sampleRate);
    
    frequencySmoother.setTargetValue(safeFreq);
}

void FilterBank::setCrossoverFrequencies(float lowMid, float midHigh)
{
    lowMidCrossover = validateFrequency(lowMid, sampleRate);
    midHighCrossover = validateFrequency(midHigh, sampleRate);
    
    // Ensure proper order
    if (midHighCrossover <= lowMidCrossover)
    {
        midHighCrossover = lowMidCrossover * 2.0f;  // Ensure at least 1 octave separation
    }
}

void FilterBank::applyPatternModulation(int bandIndex, float modAmount)
{
    if (isValidBandIndex(bandIndex))
    {
        auto& processor = bandProcessors[bandIndex];
        
        // Modulate frequency based on pattern value
        float baseFreq = processor.currentFreqValue;
        float modulatedFreq = baseFreq * (1.0f + modAmount * 0.5f);  // ±50% frequency modulation
        
        float safeFreq = validateFrequency(modulatedFreq, sampleRate);
        processor.frequency.setTargetValue(safeFreq);
    }
}

void FilterBank::processBlock(juce::AudioBuffer<float>& buffer)
{
    // Early validation for safety
    if (buffer.getNumSamples() == 0 || buffer.getNumChannels() == 0)
        return;
    
    juce::ScopedNoDenormals noDenormals;  // Prevent denormal issues
    
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    
    // Simplified single-band processing for initial implementation
    auto& processor = bandProcessors[0];  // Use first band only for now
    
    // Process block-by-block for efficiency while maintaining smooth parameter updates
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Get smoothed parameter values
        float smoothedFreq = processor.frequency.getNextValue();
        float smoothedRes = processor.resonance.getNextValue();
        float smoothedLevel = processor.level.getNextValue();
        
        // Update filter parameters safely
        processor.filter.setCutoffFrequency(smoothedFreq);
        processor.filter.setResonance(smoothedRes);
        
        // Process all channels for this sample
        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            
            // Process single sample through filter
            float inputSample = channelData[sample];
            float outputSample = processor.filter.processSample(static_cast<int>(channel), inputSample);
            
            // Apply level control and soft clipping for safety
            outputSample = std::tanh(outputSample * smoothedLevel * 0.7f);
            
            channelData[sample] = outputSample;
        }
    }
}

void FilterBank::configureBandFilter(BandProcessor& processor, FilterType type)
{
    switch (type)
    {
        case FilterType::LowPass:
            processor.filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
            break;
        case FilterType::HighPass:
            processor.filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
            break;
        case FilterType::BandPass:
            processor.filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
            break;        case FilterType::Notch:
            // Use bandpass for notch approximation
            processor.filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
            break;
        case FilterType::Peak:
            // Use bandpass for peak approximation
            processor.filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
            break;
        default:
            processor.filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
            break;
    }
}

void FilterBank::updateFilters()
{
    // Update all filter states (called when parameters change)
    for (auto& processor : bandProcessors)
    {
        configureBandFilter(processor, processor.currentType);
    }
}

FilterBank::FilterType FilterBank::getCurrentFilterType(int bandIndex) const
{
    if (isValidBandIndex(bandIndex))
        return bandProcessors[bandIndex].currentType;
    
    return FilterType::LowPass;  // Safe default
}

float FilterBank::getCurrentResonance(int bandIndex) const
{
    if (isValidBandIndex(bandIndex))
        return bandProcessors[bandIndex].currentResValue;
    
    return 0.7f;  // Safe default
}

float FilterBank::getCurrentFilterFrequency() const
{
    return currentFrequency;
}

bool FilterBank::isValidBandIndex(int bandIndex) const
{
    return bandIndex >= 0 && bandIndex < NUM_BANDS;
}

float FilterBank::validateFrequency(float frequency, double sampleRate)
{
    // Ensure frequency is within safe bounds (20Hz to Nyquist/2 for safety margin)
    float nyquist = static_cast<float>(sampleRate * 0.5);
    float maxSafeFreq = nyquist * 0.45f;  // 45% of Nyquist for safety
    
    return juce::jlimit(20.0f, maxSafeFreq, frequency);
}

float FilterBank::validateResonance(float resonance)
{
    // Limit resonance to prevent instability
    // Most filters become unstable above Q=20 (resonance ~0.95)
    return juce::jlimit(0.1f, 0.9f, resonance);
}

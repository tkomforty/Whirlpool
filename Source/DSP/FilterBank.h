#pragma once

#include <JuceHeader.h>
#include <array>

/**
 * Multi-band filter bank for rhythmic filtering effects
 * Splits audio into Low/Mid/High bands using Linkwitz-Riley crossovers
 * and applies independent filtering to each band
 */
class FilterBank
{
public:
    // Filter types available for each band
    enum class FilterType
    {
        LowPass = 0,
        HighPass,
        BandPass,
        Notch,
        Peak,
        NumFilterTypes
    };
    
    // Number of frequency bands
    static constexpr int NUM_BANDS = 3;
    
    //==============================================================================
    FilterBank();
    ~FilterBank() = default;
    
    // DSP lifecycle
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void processBlock(juce::AudioBuffer<float>& buffer);
    
    // Parameter control
    void setFilterType(FilterType newType);
    void setFilterType(int bandIndex, FilterType newType);
    void setResonance(float newResonance);
    void setResonance(int bandIndex, float newResonance);
    void setFrequency(int bandIndex, float newFrequency);
    void setLevel(int bandIndex, float newLevel);
    void setModulationValue(float modulation);
    
    // Crossover control
    void setCrossoverFrequencies(float lowMid, float midHigh);
    
    // Pattern sequencer integration
    void applyPatternModulation(int bandIndex, float modAmount);
    
    // Status queries
    FilterType getCurrentFilterType(int bandIndex = 0) const;
    float getCurrentResonance(int bandIndex = 0) const;
    float getCurrentFilterFrequency() const;
    bool isValidBandIndex(int bandIndex) const;

private:
    // Simplified implementation using JUCE's proven filters for initial version
    using Filter = juce::dsp::StateVariableTPTFilter<float>;
    
    struct BandProcessor
    {
        Filter filter;
        FilterType currentType = FilterType::LowPass;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> frequency;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> resonance;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> level;
        
        float currentFreqValue = 1000.0f;
        float currentResValue = 0.7f;
    };
    
    std::array<BandProcessor, NUM_BANDS> bandProcessors;
    
    // Working buffers for band separation
    juce::AudioBuffer<float> workingBuffer;
    
    // Parameter smoothing for safety
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> frequencySmoother;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> resonanceSmoother;
    
    // Current values
    float currentFrequency = 1000.0f;
    float currentResonance = 0.7f;
    float currentModulation = 0.0f;
    double sampleRate = 44100.0;
    
    // Configuration
    float lowMidCrossover = 400.0f;
    float midHighCrossover = 4000.0f;
    
    // Internal methods
    void configureBandFilter(BandProcessor& processor, FilterType type);
    void updateFilters();
    
    // Safety and validation
    static float validateFrequency(float frequency, double sampleRate);
    static float validateResonance(float resonance);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterBank)
};

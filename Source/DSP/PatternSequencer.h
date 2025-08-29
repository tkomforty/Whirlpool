#pragma once
#include <JuceHeader.h>
#include <array>

class TempoSync; // Forward declaration

class PatternSequencer
{
public:
    static constexpr int MAX_STEPS = 16;
    
    PatternSequencer();
    ~PatternSequencer() = default;
    
    // Setup
    void prepare(double sampleRate);
    void reset();
    
    // Pattern management
    void setStepValue(int step, float value);
    float getStepValue(int step) const;
    
    void setStepCount(int steps);
    int getStepCount() const { return stepCount; }
    
    void setPattern(int patternIndex);
    int getCurrentPattern() const { return currentPatternIndex; }
    
    // Tempo sync integration
    void setTempoSync(TempoSync* tempoSyncPtr);
    
    // Real-time modulation value access
    float getNextModulationValue();
    float getCurrentModulationValue() const;
    
    // Pattern editing
    void clearPattern();
    void randomizePattern(float intensity = 1.0f);

    // Allow setting current step (for fallback tempo mode)
    void setCurrentStep(int step);
    
private:
    std::array<float, MAX_STEPS> stepValues;
    int stepCount = MAX_STEPS;
    int currentPatternIndex = 0;
    
    // Current playback state
    int currentStep = 0;
    float currentModulation = 0.0f;
    double sampleRate = 44100.0;
    
    // Tempo sync reference
    TempoSync* tempoSync = nullptr;
    
    // Internal methods
    void initializeDefaultPattern();
    void updateCurrentStep();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternSequencer)
};

#include <JuceHeader.h>
#include "PatternSequencer.h"
#include "TempoSync.h"

void PatternSequencer::setCurrentStep(int step)
{
    currentStep = juce::jlimit(0, stepCount - 1, step);
}

PatternSequencer::PatternSequencer()
{
    initializeDefaultPattern();
}

void PatternSequencer::prepare(double sampleRateToUse)
{
    sampleRate = sampleRateToUse;
    reset();
}

void PatternSequencer::reset()
{
    currentStep = 0;
    currentModulation = 0.0f;
}

void PatternSequencer::setStepValue(int step, float value)
{
    if (step >= 0 && step < MAX_STEPS)
    {
        stepValues[step] = juce::jlimit(0.0f, 1.0f, value);
    }
}

float PatternSequencer::getStepValue(int step) const
{
    if (step >= 0 && step < MAX_STEPS)
        return stepValues[step];
    
    return 0.0f;
}

void PatternSequencer::setStepCount(int steps)
{
    stepCount = juce::jlimit(1, MAX_STEPS, steps);
}

void PatternSequencer::setPattern(int patternIndex)
{
    currentPatternIndex = juce::jlimit(0, 7, patternIndex); // Assume 8 patterns
    
    // Load pattern based on index (simplified)
    switch (currentPatternIndex)
    {
        case 0: // Basic 4/4 kick pattern
            clearPattern();
            setStepValue(0, 1.0f);
            setStepValue(4, 0.7f);
            setStepValue(8, 1.0f);
            setStepValue(12, 0.7f);
            break;
            
        case 1: // Offbeat pattern
            clearPattern();
            setStepValue(2, 1.0f);
            setStepValue(6, 0.8f);
            setStepValue(10, 1.0f);
            setStepValue(14, 0.8f);
            break;
            
        case 2: // Triplet feel
            clearPattern();
            setStepValue(0, 1.0f);
            setStepValue(3, 0.6f);
            setStepValue(6, 0.8f);
            setStepValue(9, 0.6f);
            setStepValue(12, 1.0f);
            setStepValue(15, 0.6f);
            break;
            
        default:
            initializeDefaultPattern();
            break;
    }
}

void PatternSequencer::setTempoSync(TempoSync* tempoSyncPtr)
{
    tempoSync = tempoSyncPtr;
}

float PatternSequencer::getNextModulationValue()
{
    updateCurrentStep();
    
    // Return the current step value
    if (currentStep >= 0 && currentStep < stepCount)
    {
        currentModulation = stepValues[currentStep];
    }
    else
    {
        currentModulation = 0.0f;
    }
    
    return currentModulation;
}

float PatternSequencer::getCurrentModulationValue() const
{
    // Update current step first (const_cast is safe here for internal state update)
    const_cast<PatternSequencer*>(this)->updateCurrentStep();
    
    // Return current step value
    if (currentStep >= 0 && currentStep < stepCount)
    {
        return stepValues[currentStep];
    }
    
    return 0.0f;
}

void PatternSequencer::clearPattern()
{
    for (int i = 0; i < MAX_STEPS; ++i)
    {
        stepValues[i] = 0.0f;
    }
}

void PatternSequencer::randomizePattern(float intensity)
{
    juce::Random random;
    
    for (int i = 0; i < stepCount; ++i)
    {
        stepValues[i] = random.nextFloat() * intensity;
    }
}

void PatternSequencer::initializeDefaultPattern()
{
    // Initialize with a simple 4/4 pattern
    clearPattern();
    setStepValue(0, 1.0f);   // Beat 1
    setStepValue(4, 0.7f);   // Beat 2
    setStepValue(8, 1.0f);   // Beat 3
    setStepValue(12, 0.7f);  // Beat 4
}

void PatternSequencer::updateCurrentStep()
{
    if (tempoSync && tempoSync->isPlaying())
    {
        // Get the current step from tempo sync
        currentStep = tempoSync->getCurrentStep() % stepCount;
    }
}

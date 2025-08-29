#include <JuceHeader.h>
#include "TempoSync.h"

TempoSync::TempoSync()
    : sampleRate(44100.0)
    , maxBlockSize(512)
    , currentPhase(0.0)
    , currentStep(0)
    , lastStep(-1)
    , stepChanged(false)
    , patternLength(16)
    , currentDivisionIndex(4) // Default to 1/8 note (index 4 in new array)
    , swingAmount(0.0f)
    , hostIsPlaying(false)
    , currentBPM(120.0)
    , currentPPQPosition(0.0)
{
}

void TempoSync::prepare(double newSampleRate, int newMaxBlockSize)
{
    sampleRate = newSampleRate;
    maxBlockSize = newMaxBlockSize;
    reset();
}

void TempoSync::reset()
{
    currentPhase = 0.0;
    currentStep = 0;
    lastStep = -1;
    stepChanged = false;
}

void TempoSync::updatePlayheadInfo(const juce::AudioPlayHead::CurrentPositionInfo& posInfo)
{
    hostIsPlaying = posInfo.isPlaying;
    
    if (posInfo.bpm > 0.0)
        currentBPM = posInfo.bpm;
    
    if (posInfo.ppqPosition >= 0.0)
        currentPPQPosition = posInfo.ppqPosition;
}

void TempoSync::updateTempoDivision(int divisionIndex)
{
    if (divisionIndex >= 0 && divisionIndex < numTempoDivisions)
        currentDivisionIndex = divisionIndex;
}

void TempoSync::updateSwingAmount(float swingPercent)
{
    swingAmount = juce::jlimit(0.0f, 100.0f, swingPercent);
}

void TempoSync::updatePatternLength(int steps)
{
    patternLength = juce::jlimit(1, 32, steps);
    
    // Reset pattern if current step is beyond new length
    if (currentStep >= patternLength)
    {
        currentStep = 0;
        currentPhase = 0.0;
    }
}

void TempoSync::processBlock(int numSamples)
{
    if (!hostIsPlaying || currentBPM <= 0.0)
    {
        stepChanged = false;
        return;
    }
    
    // Store the step at the beginning of the block
    lastStep = currentStep;
    
    // Calculate phase increment for the entire block
    const double phaseIncrement = calculatePhaseIncrement();
    const double totalPhaseIncrement = phaseIncrement * numSamples;
    
    // Update phase
    currentPhase += totalPhaseIncrement;
    
    // Keep phase in [0, 1) range
    while (currentPhase >= 1.0)
        currentPhase -= 1.0;
    
    // Update step position
    updateStepPosition();
    
    // Check if step changed during this block
    stepChanged = (currentStep != lastStep);
}

double TempoSync::getCurrentPhase() const
{
    return getSwingAdjustedPhase();
}

double TempoSync::getPhaseForBand(float phaseOffsetDegrees) const
{
    const double offsetPhase = phaseOffsetDegrees / 360.0;
    double phase = getCurrentPhase() + offsetPhase;
    
    // Wrap phase to [0, 1) range
    while (phase >= 1.0) phase -= 1.0;
    while (phase < 0.0) phase += 1.0;
    
    return phase;
}

double TempoSync::getSwingAdjustedPhase() const
{
    if (swingAmount <= 0.0f)
        return currentPhase;
    
    return applySwingToPhase(currentPhase, currentStep);
}

bool TempoSync::hasStepChanged() const
{
    return stepChanged;
}

int TempoSync::getCurrentStep() const
{
    return currentStep;
}

double TempoSync::getBeatLength() const
{
    if (currentBPM <= 0.0 || sampleRate <= 0.0)
        return 0.0;
        
    return (60.0 / currentBPM) * sampleRate;
}

double TempoSync::calculatePhaseIncrement() const
{
    if (currentBPM <= 0.0 || sampleRate <= 0.0)
        return 0.0;
    
    // Get current tempo division
    const auto& division = tempoDivisions[currentDivisionIndex];
    
    // Calculate beats per second for this division
    const double beatsPerSecond = currentBPM / 60.0;
    const double divisionsPerSecond = beatsPerSecond / division.noteValue;
    
    // Calculate phase increment per sample for one step
    const double stepsPerSecond = divisionsPerSecond;
    const double phasesPerSecond = stepsPerSecond / patternLength;
    
    return phasesPerSecond / sampleRate;
}

double TempoSync::applySwingToPhase(double phase, int step) const
{
    if (swingAmount <= 0.0f || patternLength <= 1)
        return phase;
    
    // Apply swing to off-beat steps (every other step)
    const bool isOffBeat = (step % 2) == 1;
    
    if (isOffBeat)
    {
        // Delay off-beat steps by swing amount (max 25% delay)
        const double swingFactor = (swingAmount / 100.0) * 0.25;
        return phase + swingFactor;
    }
    
    return phase;
}

void TempoSync::updateStepPosition()
{
    // Calculate which step we're on based on current phase
    const int newStep = static_cast<int>(currentPhase * patternLength) % patternLength;
    
    // Ensure step is in valid range
    currentStep = juce::jlimit(0, patternLength - 1, newStep);
}

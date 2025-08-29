#pragma once
#include <JuceHeader.h>

/**
 * Tempo synchronization and pattern timing manager
 * Handles host tempo sync, pattern position tracking, and rhythmic timing
 */
class TempoSync
{
public:
    TempoSync();
    ~TempoSync() = default;
    
    // Setup and configuration
    void prepare(double sampleRate, int maxBlockSize = 512);
    void reset();
    
    // Host timing information
    void updatePlayheadInfo(const juce::AudioPlayHead::CurrentPositionInfo& posInfo);
    void updateTempoDivision(int divisionIndex);
    void updateSwingAmount(float swingPercent);
    void updatePatternLength(int steps);
    
    // Pattern position and timing
    void processBlock(int numSamples);
      // Query current state
    bool isPlaying() const { return hostIsPlaying; }
    double getCurrentBPM() const { return currentBPM; }
    double getCurrentPhase() const;
    int getCurrentStep() const;
    bool hasStepChanged() const;
    
    // Get phase for specific band with offset
    double getPhaseForBand(float phaseOffsetDegrees) const;
    
    // Get swing-adjusted step timing
    double getSwingAdjustedPhase() const;

private:
    // Tempo division definitions
    struct TempoDivision
    {
        float noteValue;    // 1.0 = quarter note, 0.5 = eighth note, etc.
        bool isTriplet;
        const char* name;
    };
      static constexpr TempoDivision tempoDivisions[] = {
        {0.125f, false, "1/32"},        // Index 0
        {0.25f/3.0f, true, "1/16T"},    // Index 1
        {0.25f, false, "1/16"},         // Index 2
        {0.5f/3.0f, true, "1/8T"},      // Index 3
        {0.5f, false, "1/8"},           // Index 4
        {1.0f/3.0f, true, "1/4T"},      // Index 5
        {1.0f, false, "1/4"},           // Index 6
        {2.0f, false, "1/2"},           // Index 7
        {4.0f, false, "1/1"},           // Index 8
        {8.0f, false, "2/1"}            // Index 9
    };
    
    static constexpr int numTempoDivisions = sizeof(tempoDivisions) / sizeof(tempoDivisions[0]);
    
    // Host sync state
    bool hostIsPlaying = false;
    double currentBPM = 120.0;
    double currentPPQPosition = 0.0;
    double sampleRate = 44100.0;
    int maxBlockSize = 512;
    
    // Pattern timing
    int currentStep = 0;
    int patternLength = 16;
    double currentPhase = 0.0;
    bool stepChanged = false;
    int lastStep = -1;
    
    // Tempo division settings
    int currentDivisionIndex = 3; // Default to 1/16 note
    float swingAmount = 0.0f;
    
    // Internal timing calculation
    void calculateCurrentStep();
    double getBeatLength() const;
    double calculatePhaseIncrement() const;
    double applySwingToPhase(double phase, int step) const;
    void updateStepPosition();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoSync)
};

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace ParameterIDs
{
    // Global parameters
    static const juce::String masterTempoDivision = "master_tempo_division";
    static const juce::String masterPatternLength = "master_pattern_length";
    static const juce::String masterSwingAmount = "master_swing_amount";
    static const juce::String masterDryWet = "master_dry_wet";
    static const juce::String masterOutputGain = "master_output_gain";
    
    // Simplified GUI parameters
    static const juce::String mix = "mix";
    static const juce::String filterType = "filterType";
    static const juce::String patternSelect = "patternSelect";
    static const juce::String tempoSync = "tempoSync";
    static const juce::String intensity = "intensity";
    static const juce::String resonance = "resonance";
    static const juce::String stepCount = "stepCount";
    
    // Per-band parameters (Low, Mid, High)
    static const juce::String lowBandFilterType = "low_band_filter_type";
    static const juce::String lowBandCutoffMin = "low_band_cutoff_min";
    static const juce::String lowBandCutoffMax = "low_band_cutoff_max";
    static const juce::String lowBandResonance = "low_band_resonance";
    static const juce::String lowBandPhaseOffset = "low_band_phase_offset";
    static const juce::String lowBandLevel = "low_band_level";
    
    static const juce::String midBandFilterType = "mid_band_filter_type";
    static const juce::String midBandCutoffMin = "mid_band_cutoff_min";
    static const juce::String midBandCutoffMax = "mid_band_cutoff_max";
    static const juce::String midBandResonance = "mid_band_resonance";
    static const juce::String midBandPhaseOffset = "mid_band_phase_offset";
    static const juce::String midBandLevel = "mid_band_level";
    
    static const juce::String highBandFilterType = "high_band_filter_type";
    static const juce::String highBandCutoffMin = "high_band_cutoff_min";
    static const juce::String highBandCutoffMax = "high_band_cutoff_max";
    static const juce::String highBandResonance = "high_band_resonance";
    static const juce::String highBandPhaseOffset = "high_band_phase_offset";
    static const juce::String highBandLevel = "high_band_level";
    
    // Crossover frequencies
    static const juce::String lowMidCrossover = "low_mid_crossover";
    static const juce::String midHighCrossover = "mid_high_crossover";
    
    // Pattern parameters (16 steps × 3 bands = 48 parameters)
    static juce::String getPatternStepID(int band, int step)
    {
        juce::String bandName = (band == 0) ? "low" : (band == 1) ? "mid" : "high";
        return bandName + "_pattern_step_" + juce::String(step);
    }
}

namespace ParameterRanges
{
    // Tempo divisions (as choices)
    static const juce::StringArray tempoDivisions = { 
        "1/32", "1/16T", "1/16", "1/8T", "1/8", "1/4T", "1/4", "1/2", "1/1", "2/1" 
    };
    
    // Filter types
    static const juce::StringArray filterTypes = {
        "Low Pass", "High Pass", "Band Pass", "Notch", "Peak"
    };
    
    // Parameter ranges
    static const juce::NormalisableRange<float> frequencyRange{20.0f, 20000.0f, 1.0f, 0.3f};
    static const juce::NormalisableRange<float> resonanceRange{0.1f, 10.0f, 0.01f, 0.4f};
    static const juce::NormalisableRange<float> phaseRange{0.0f, 360.0f, 1.0f};
    static const juce::NormalisableRange<float> gainRange{0.0f, 2.0f, 0.01f};
    static const juce::NormalisableRange<float> percentRange{0.0f, 100.0f, 0.1f};
    static const juce::NormalisableRange<float> patternRange{0.0f, 1.0f, 0.01f};
}

class ParameterDefinitions
{
public:    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        
        // Simplified GUI parameters
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParameterIDs::mix, 1),
            "Mix",
            ParameterRanges::percentRange,
            100.0f));
            
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(ParameterIDs::filterType, 1),
            "Filter Type",
            ParameterRanges::filterTypes,
            0));
            
        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID(ParameterIDs::patternSelect, 1),
            "Pattern",
            1, 8, 1));
            
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(ParameterIDs::tempoSync, 1),
            "Tempo Sync",
            ParameterRanges::tempoDivisions,
            4));
            
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParameterIDs::intensity, 1),
            "Intensity",
            ParameterRanges::percentRange,
            50.0f));
            
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParameterIDs::resonance, 1),
            "Resonance",
            ParameterRanges::resonanceRange,
            0.7f));
            
        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID(ParameterIDs::stepCount, 1),
            "Steps",
            1, 16, 16));
        
        // Master parameters
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(ParameterIDs::masterTempoDivision, 1),
            "Tempo Division",
            ParameterRanges::tempoDivisions,
            4)); // Default to 1/8
            
        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID(ParameterIDs::masterPatternLength, 1),
            "Pattern Length",
            1, 16, 16));
            
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParameterIDs::masterSwingAmount, 1),
            "Swing Amount",
            ParameterRanges::percentRange,
            0.0f));
            
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParameterIDs::masterDryWet, 1),
            "Dry/Wet",
            ParameterRanges::percentRange,
            100.0f));
            
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParameterIDs::masterOutputGain, 1),
            "Output Gain",
            juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f),
            0.0f));
        
        // Crossover frequencies
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParameterIDs::lowMidCrossover, 1),
            "Low/Mid Crossover",
            ParameterRanges::frequencyRange,
            400.0f));
            
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParameterIDs::midHighCrossover, 1),
            "Mid/High Crossover",
            ParameterRanges::frequencyRange,
            4000.0f));
        
        // Per-band parameters
        addBandParameters(layout, "Low", ParameterIDs::lowBandFilterType, 
                         ParameterIDs::lowBandCutoffMin, ParameterIDs::lowBandCutoffMax,
                         ParameterIDs::lowBandResonance, ParameterIDs::lowBandPhaseOffset,
                         ParameterIDs::lowBandLevel, 200.0f, 2000.0f);
                         
        addBandParameters(layout, "Mid", ParameterIDs::midBandFilterType,
                         ParameterIDs::midBandCutoffMin, ParameterIDs::midBandCutoffMax,
                         ParameterIDs::midBandResonance, ParameterIDs::midBandPhaseOffset,
                         ParameterIDs::midBandLevel, 500.0f, 5000.0f);
                         
        addBandParameters(layout, "High", ParameterIDs::highBandFilterType,
                         ParameterIDs::highBandCutoffMin, ParameterIDs::highBandCutoffMax,
                         ParameterIDs::highBandResonance, ParameterIDs::highBandPhaseOffset,
                         ParameterIDs::highBandLevel, 2000.0f, 10000.0f);
        
        // Pattern step parameters
        for (int band = 0; band < 3; ++band)
        {
            for (int step = 0; step < 16; ++step)
            {
                juce::String paramID = ParameterIDs::getPatternStepID(band, step);
                juce::String paramName = "Band " + juce::String(band + 1) + " Step " + juce::String(step + 1);
                
                layout.add(std::make_unique<juce::AudioParameterFloat>(
                    juce::ParameterID(paramID, 1),
                    paramName,
                    ParameterRanges::patternRange,
                    0.5f)); // Default to 50% intensity
            }
        }
        
        return layout;
    }
    
private:
    static void addBandParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                                 const juce::String& bandName,
                                 const juce::String& filterTypeID,
                                 const juce::String& cutoffMinID,
                                 const juce::String& cutoffMaxID,
                                 const juce::String& resonanceID,
                                 const juce::String& phaseOffsetID,
                                 const juce::String& levelID,
                                 float defaultCutoffMin,
                                 float defaultCutoffMax)
    {
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(filterTypeID, 1),
            bandName + " Filter Type",
            ParameterRanges::filterTypes,
            0)); // Default to Low Pass
            
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(cutoffMinID, 1),
            bandName + " Cutoff Min",
            ParameterRanges::frequencyRange,
            defaultCutoffMin));
            
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(cutoffMaxID, 1),
            bandName + " Cutoff Max",
            ParameterRanges::frequencyRange,
            defaultCutoffMax));
            
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(resonanceID, 1),
            bandName + " Resonance",
            ParameterRanges::resonanceRange,
            0.7f));
            
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(phaseOffsetID, 1),
            bandName + " Phase Offset",
            ParameterRanges::phaseRange,
            0.0f));
            
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(levelID, 1),
            bandName + " Level",
            ParameterRanges::gainRange,
            1.0f));
    }
};

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/CustomLookAndFeel.h"

//==============================================================================
/**
*/
class RhythmicFilterSculptorAudioProcessorEditor : public juce::AudioProcessorEditor,
                                                   public juce::Slider::Listener,
                                                   public juce::ComboBox::Listener,
                                                   public juce::Timer
{
public:
    RhythmicFilterSculptorAudioProcessorEditor (RhythmicFilterSculptorAudioProcessor&);
    ~RhythmicFilterSculptorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    // Listener callbacks
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    
    // Timer callback for animation
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RhythmicFilterSculptorAudioProcessor& audioProcessor;
    
    // Animation variables for whirlpool effect
    float animationPhase = 0.0f;
    float intensityPhase = 0.0f;
    float centerOffsetPhase = 0.0f;
    float spiralPhase = 0.0f;
    juce::int64 lastUpdateTime = 0;
    
    // LED timing variables for downbeat indicator
    bool isDownbeatActive = false;
    float downbeatIntensity = 0.0f;
    double lastBeatPosition = -1.0;
    double currentBpm = 120.0;
    juce::Colour ledColor = juce::Colour(0xff4a9eff); // blue by default
    
    // GUI Components
    // Filter section
    juce::ComboBox filterTypeCombo;
    juce::Label filterTypeLabel;
    juce::Slider resonanceSlider;
    juce::Label resonanceLabel;
    juce::Label resonanceValueLabel;
    juce::Label intensityValueLabel;
    juce::Label mixValueLabel;
    
    // Pattern section
    juce::ComboBox patternSelectCombo;
    juce::Label patternSelectLabel;
    juce::Slider stepCountSlider;
    juce::Label stepCountLabel;
    juce::ComboBox tempoSyncCombo;
    juce::Label tempoSyncLabel;
    
    // Modulation section
    juce::Slider intensitySlider;
    juce::Label intensityLabel;
    juce::Slider mixSlider;  // Wet/Dry control
    juce::Label mixLabel;

    // Output trim section (Pattern card bottom)
    juce::Slider outputTrimSlider;
    juce::Label outputTrimValueLabel;
    
    // Company label
    juce::Label companyLabel;
    
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> patternSelectAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> tempoSyncAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> intensityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resonanceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputTrimAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stepCountAttachment;
      // Look and Feel
    CustomLookAndFeel customLookAndFeel;
      // Helper methods
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText);
    void setupComboBox(juce::ComboBox& combo, juce::Label& label, const juce::String& labelText);
    
    // Layout methods
    void layoutFilterSection(juce::Rectangle<int> area);
    void layoutPatternSection(juce::Rectangle<int> area);
    void layoutModulationSection(juce::Rectangle<int> area);
    

    // Drawing methods
    void drawSectionCard(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& title, juce::Colour accentColor);

    // Typeface members for custom fonts
    juce::Typeface::Ptr companyTypeface;
    juce::Typeface::Ptr whirlpoolTypeface;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RhythmicFilterSculptorAudioProcessorEditor)
};
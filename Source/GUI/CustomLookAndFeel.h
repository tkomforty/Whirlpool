#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Custom Look and Feel using enhanced blue/purple theme for modern knob design
 * Implements advanced knob rendering with multi-layer depth, gradients, and dynamic colors
 */
class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    
    // Color palette from Desert Monsoon Green theme
    static const juce::Colour shadow;           // #8c764a (Shadow)
    static const juce::Colour shingleFawn;      // #6d4634 (Shingle Fawn)
    static const juce::Colour englishWalnut;    // #3d2b23 (English Walnut)
    static const juce::Colour seaNymph;         // #7f9d97 (Sea Nymph)
    static const juce::Colour cuttySark;        // #4f827c (Cutty Sark)
    
    // Additional calculated colors
    static const juce::Colour lightText;
    static const juce::Colour mediumText;
    static const juce::Colour darkText;
    static const juce::Colour accent;
    static const juce::Colour highlight;
    static const juce::Colour background;
    static const juce::Colour cardBackground;
    static const juce::Colour border;
    
    // Override methods for custom styling
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override;
    
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                     int buttonX, int buttonY, int buttonW, int buttonH,
                     juce::ComboBox& box) override;
    
    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;
    
    juce::Font getComboBoxFont(juce::ComboBox& box) override;
    
    void drawLabel(juce::Graphics& g, juce::Label& label) override;
    
    juce::Font getLabelFont(juce::Label& label) override;
      juce::BorderSize<int> getLabelBorderSize(juce::Label& label) override;
    
    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override;
    
    // Whirlpool pattern methods for background theming
    void drawWhirlpoolPattern(juce::Graphics& g, juce::Rectangle<float> bounds, float intensity) const;
    void drawWhirlpoolPattern(juce::Graphics& g, juce::Rectangle<float> bounds) const;  // Uses config values only
    void drawSpiralPattern(juce::Graphics& g, juce::Point<float> center, float radius, int spirals = 3, float intensity = 0.42f) const;
    void drawConcentricRipples(juce::Graphics& g, juce::Point<float> center, float maxRadius, int rippleCount = 8, float intensity = 0.35f) const;
    void drawFlowLines(juce::Graphics& g, juce::Rectangle<float> bounds, int lineCount = 12, float intensity = 0.51f) const;
    
    // Whirlpool pattern configuration
    struct WhirlpoolConfig
    {
        bool enabled = true;
        float intensity = 0.95f;
        int spiralCount = 2;        int rippleCount = 12;
        int flowLineCount = 55;
        float centerOffsetX = 0.1f;  // Relative to bounds width
        float centerOffsetY = -0.05f; // Relative to bounds height
    };
    
    WhirlpoolConfig whirlpoolConfig;
    
    // Convenience methods for adjusting whirlpool pattern
    void setWhirlpoolEnabled(bool enabled) { whirlpoolConfig.enabled = enabled; }
    void setWhirlpoolIntensity(float intensity) { whirlpoolConfig.intensity = juce::jlimit(0.0f, 1.0f, intensity); }
    void setWhirlpoolSpiralCount(int count) { whirlpoolConfig.spiralCount = juce::jmax(1, count); }
    void setWhirlpoolRippleCount(int count) { whirlpoolConfig.rippleCount = juce::jmax(3, count); }
    void setWhirlpoolFlowLineCount(int count) { whirlpoolConfig.flowLineCount = juce::jmax(5, count); }
    void setWhirlpoolCenterOffset(float x, float y) { 
        whirlpoolConfig.centerOffsetX = juce::jlimit(-0.5f, 0.5f, x);
        whirlpoolConfig.centerOffsetY = juce::jlimit(-0.5f, 0.5f, y);
    }

private:
    // Enhanced knob drawing helper methods
    void drawKnobShadow(juce::Graphics& g, juce::Point<float> centre, float radius) const;
    void drawKnobTrack(juce::Graphics& g, juce::Point<float> centre, float arcRadius, float lineW, 
                       float startAngle, float endAngle) const;
    void drawKnobValueArc(juce::Graphics& g, juce::Point<float> centre, float arcRadius, float lineW,
                          float startAngle, float toAngle, bool isEnabled) const;
    void drawKnobThumb(juce::Graphics& g, juce::Point<float> centre, float arcRadius, float lineW,
                       float angle) const;
    void drawKnobCenterCap(juce::Graphics& g, juce::Point<float> centre, float radius) const;
    void drawKnobNotches(juce::Graphics& g, juce::Point<float> centre, float arcRadius, float lineW,
                         float startAngle, float endAngle) const;
    
    // Dynamic color methods for enhanced theming
    juce::Colour getDynamicAccentColor(float value) const;
    juce::Colour getDynamicHighlightColor(float value) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};

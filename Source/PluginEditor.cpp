#include <JuceHeader.h>
#ifdef patternCard
#undef patternCard
#endif
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================

RhythmicFilterSculptorAudioProcessorEditor::RhythmicFilterSculptorAudioProcessorEditor (RhythmicFilterSculptorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Load custom typefaces from BinaryData as member variables
    companyTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::companyTypeface_ttf, BinaryData::companyTypeface_ttfSize);
    whirlpoolTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::whirlpool_1_ttf, BinaryData::whirlpool_1_ttfSize);

    // Set the look and feel
    setLookAndFeel(&customLookAndFeel);
    // Removed companyLabel from UI; text is now drawn manually in paint()

    // Setup sliders (removed stepCountSlider as it's redundant with pattern selection)
    setupSlider(resonanceSlider, resonanceLabel, "Resonance");
    setupSlider(intensitySlider, intensityLabel, "Intensity");
    setupSlider(mixSlider, mixLabel, "Wet/Dry");
    
    // Setup combo boxes
    setupComboBox(filterTypeCombo, filterTypeLabel, "Filter Type");
    filterTypeCombo.addItem("Low Pass", 1);
    filterTypeCombo.addItem("High Pass", 2);
    filterTypeCombo.addItem("Band Pass", 3);
    filterTypeCombo.addItem("Notch", 4);
    
    setupComboBox(patternSelectCombo, patternSelectLabel, "Pattern");
    patternSelectCombo.addItem("Steady Pulse", 1);
    patternSelectCombo.addItem("Syncopated", 2);
    patternSelectCombo.addItem("Triplet Feel", 3);
    patternSelectCombo.addItem("Dotted Rhythm", 4);
    patternSelectCombo.addItem("Random Walk", 5);
    patternSelectCombo.addItem("Build Up", 6);
    patternSelectCombo.addItem("Break Down", 7);
    patternSelectCombo.addItem("Stutter", 8);
    
    setupComboBox(tempoSyncCombo, tempoSyncLabel, "Tempo Sync");
    tempoSyncCombo.addItem("1/32", 1);
    tempoSyncCombo.addItem("1/16T", 2);
    tempoSyncCombo.addItem("1/16", 3);
    tempoSyncCombo.addItem("1/8T", 4);
    tempoSyncCombo.addItem("1/8", 5);
    tempoSyncCombo.addItem("1/4T", 6);
    tempoSyncCombo.addItem("1/4", 7);
    tempoSyncCombo.addItem("1/2", 8);
    tempoSyncCombo.addItem("1/1", 9);
    tempoSyncCombo.addItem("2/1", 10);
    
    // Add and setup value labels for all sliders
    addAndMakeVisible(resonanceValueLabel);
    addAndMakeVisible(intensityValueLabel);
    addAndMakeVisible(mixValueLabel);

    // --- Output Trim Slider and Label Setup ---
    addAndMakeVisible(outputTrimSlider);
    addAndMakeVisible(outputTrimValueLabel);
    outputTrimSlider.setRange(-24.0, 12.0, 0.1); // dB range, typical for output trim
    outputTrimSlider.setValue(0.0);
    outputTrimSlider.setDoubleClickReturnValue(true, 0.0);
    outputTrimSlider.setPopupDisplayEnabled(false, false, nullptr); // Disable popup
    outputTrimSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff0f0f0));
    outputTrimSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    outputTrimSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    outputTrimSlider.setTooltip("Adjusts the overall output level (trim)");
    outputTrimSlider.addListener(this); // Ensure value label updates live
    
    // Create parameter attachments (connect GUI to global parameters used in DSP)
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "mix", mixSlider);

    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getValueTreeState(), "filterType", filterTypeCombo);

    patternSelectAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getValueTreeState(), "patternSelect", patternSelectCombo);

    tempoSyncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getValueTreeState(), "tempoSync", tempoSyncCombo);

    intensityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "intensity", intensitySlider);

    resonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "resonance", resonanceSlider);

    // Output trim parameter attachment
    outputTrimAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "master_output_gain", outputTrimSlider);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(720, 300); // Increased width for better proportions and spacing
    
    // Start animation timer - will be synced to host tempo in timerCallback
    startTimerHz(60); // 60 FPS for smooth animation
}

RhythmicFilterSculptorAudioProcessorEditor::~RhythmicFilterSculptorAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void RhythmicFilterSculptorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Professional multi-layer gradient background
    auto bounds = getLocalBounds().toFloat();
    
    // Base gradient - deep purple to dark blue
    juce::ColourGradient baseGradient(
        juce::Colour(0xff2d1b69), 0, 0,                    // Deep purple at top
        juce::Colour(0xff1a1a2e), 0, bounds.getHeight(),   // Dark blue-gray at bottom
        false
    );
    g.setGradientFill(baseGradient);
    g.fillAll();
    
    // Accent overlay gradient - subtle teal highlights
    juce::ColourGradient accentOverlay(
        juce::Colour(0xff0f3460).withAlpha(0.6f), bounds.getWidth() * 0.3f, 0,
        juce::Colour(0xff16537e).withAlpha(0.3f), bounds.getWidth() * 0.7f, bounds.getHeight() * 0.6f,
        false
    );
    g.setGradientFill(accentOverlay);
    g.fillAll();
    
    // Radial glow from center-right for depth
    juce::ColourGradient radialGlow(
        juce::Colour(0xff4a9eff).withAlpha(0.08f), bounds.getWidth() * 0.8f, bounds.getHeight() * 0.3f,
        juce::Colours::transparentBlack, bounds.getWidth() * 0.8f, bounds.getHeight() * 0.3f,
        true
    );
    radialGlow.addColour(0.3, juce::Colour(0xff6db4ff).withAlpha(0.05f));
    radialGlow.addColour(0.7, juce::Colour(0xff89cdf1).withAlpha(0.02f));
    g.setGradientFill(radialGlow);    
    g.fillEllipse(bounds.getWidth() * 0.4f, bounds.getHeight() * 0.1f, 
                  bounds.getWidth() * 0.8f, bounds.getHeight() * 0.8f);
    
    // Static, multi-ripple pattern overlay for a refined, professional look
    // Use a deterministic seed for consistent layout
    constexpr int numRipples = 16;
    uint32_t seed = 0xA5C3F17B; // Arbitrary fixed seed for repeatability
    float minRadius = bounds.getHeight() * 0.18f;
    float maxRadius = bounds.getHeight() * 0.38f;
    for (int i = 0; i < numRipples; ++i)
    {
        // Simple LCG for pseudo-random but repeatable positions
        seed = seed * 1664525u + 1013904223u;
        float x = bounds.getX() + (bounds.getWidth() * (0.12f + 0.76f * ((seed & 0xFFFF) / 65535.0f)));
        seed = seed * 1664525u + 1013904223u;
        float y = bounds.getY() + (bounds.getHeight() * (0.10f + 0.80f * ((seed & 0xFFFF) / 65535.0f)));
        seed = seed * 1664525u + 1013904223u;
        float radius = minRadius + (maxRadius - minRadius) * ((seed & 0xFFFF) / 65535.0f);
        int ripples = 8 + (seed % 7); // 8-14 ripples
        // Make ripples more visible by increasing intensity and alpha range
        float intensity = 0.70f + 0.25f * ((seed >> 8) & 0xFF) / 255.0f;
        customLookAndFeel.drawConcentricRipples(g, {x, y}, radius, ripples, intensity);
    }
    
    // Subtle texture overlay with multiple accent lines and enhanced depth
    g.setColour(juce::Colour(0xff4a9eff).withAlpha(0.06f));
    for (int i = 0; i < getWidth(); i += 80)
    {
        g.drawLine(static_cast<float>(i), 0.0f, static_cast<float>(i), static_cast<float>(getHeight()), 1.2f);
    }
    
    // Additional diagonal accent lines for visual interest
    g.setColour(juce::Colour(0xff89cdf1).withAlpha(0.04f));
    for (int i = -getHeight(); i < getWidth(); i += 100)
    {
        g.drawLine(static_cast<float>(i), 0.0f, static_cast<float>(i) + static_cast<float>(getHeight()) * 0.4f, static_cast<float>(getHeight()), 0.8f);
    }
    
    // Add subtle horizontal lines for grid effect
    g.setColour(juce::Colour(0xff6db4ff).withAlpha(0.03f));
    for (int i = 60; i < getHeight(); i += 40)
    {
        g.drawLine(0.0f, static_cast<float>(i), static_cast<float>(getWidth()), static_cast<float>(i), 0.5f);
    }
    
    // Enhanced header area with multi-color gradient
    juce::Rectangle<int> headerArea(0, 0, getWidth(), 50);
    juce::ColourGradient headerGradient(
        juce::Colour(0xff3d2a6b), 0.0f, 0.0f,                    // Purple-blue
        juce::Colour(0xff1e1e3a), 0.0f, static_cast<float>(headerArea.getHeight()), // Dark blue
        false
    );
    headerGradient.addColour(0.5, juce::Colour(0xff2a3d5b)); // Mid-tone blue
    g.setGradientFill(headerGradient);
    g.fillRect(headerArea);
    
    // Multi-layered header border with accent colors
    g.setColour(juce::Colour(0xff4a9eff).withAlpha(0.8f));
    g.drawLine(0.0f, static_cast<float>(headerArea.getBottom()), static_cast<float>(getWidth()), static_cast<float>(headerArea.getBottom()), 2.0f);
    g.setColour(juce::Colour(0xff89cdf1).withAlpha(0.4f));
    g.drawLine(0.0f, static_cast<float>(headerArea.getBottom() + 1), static_cast<float>(getWidth()), static_cast<float>(headerArea.getBottom() + 1), 1.0f);
    
    // Vertically center both 'whirlpool' and 'ORCAS AUDIO' in the header
    int whirlpoolTextHeight = 48;
    int whirlpoolTextY = headerArea.getY() + (headerArea.getHeight() - whirlpoolTextHeight) / 2;
    if (whirlpoolTypeface)
    {
        juce::FontOptions whirlpoolFontOptions { whirlpoolTypeface };
        juce::Font whirlpoolFont1(whirlpoolFontOptions.withHeight(32.0f));
        juce::Font whirlpoolFont2(whirlpoolFontOptions.withHeight(32.0f));
        juce::Font whirlpoolFont3(whirlpoolFontOptions.withHeight(30.0f));
        whirlpoolFont1.setExtraKerningFactor(10.0f / whirlpoolFont1.getHeight());
        whirlpoolFont2.setExtraKerningFactor(10.0f / whirlpoolFont2.getHeight());
        whirlpoolFont3.setExtraKerningFactor(10.0f / whirlpoolFont3.getHeight());
        g.setColour(juce::Colour(0xff4a9eff).withAlpha(0.3f));
        g.setFont(whirlpoolFont1);
        g.drawText("whirlpool", 32, whirlpoolTextY + 3, 350, whirlpoolTextHeight + 2, juce::Justification::centredLeft);

        g.setColour(juce::Colour(0xff6db4ff).withAlpha(0.5f));
        g.setFont(whirlpoolFont2);
        g.drawText("whirlpool", 30, whirlpoolTextY + 1, 350, whirlpoolTextHeight + 2, juce::Justification::centredLeft);

        g.setColour(juce::Colour(0xfff0f0f0));
        g.setFont(whirlpoolFont3);
        g.drawText("whirlpool", 30, whirlpoolTextY, 350, whirlpoolTextHeight + 2, juce::Justification::centredLeft);
    }
    else
    {
        // Fallback to default font if typeface not loaded
        juce::Font fallbackFont(juce::FontOptions(30.0f).withStyle("Bold"));
        fallbackFont.setExtraKerningFactor(10.0f / 30.0f);
        g.setColour(juce::Colour(0xfff0f0f0));
        g.setFont(fallbackFont);
        g.drawText("whirlpool", 30, whirlpoolTextY, 350, whirlpoolTextHeight + 2, juce::Justification::centredLeft);
    }

    // Add subtle version indicator (moved to bottom right)
    g.setColour(juce::Colour(0xff89cdf1).withAlpha(0.6f));
    g.setFont(juce::Font(juce::FontOptions(8.0f).withStyle("Regular")));
    g.drawText("v1.0.0", getWidth() - 55, getHeight() - 18, 50, 12, juce::Justification::centredRight);    
    
    // Enhanced section dividers with multiple accent colors
    auto contentArea = juce::Rectangle<int>(0, 50, getWidth(), getHeight() - 50);

    // Vertically center and italicize 'ORCAS AUDIO' in the header
    int companyTextHeight = 16;
    int companyTextWidth = 170;
    int companyTextY = headerArea.getY() + (headerArea.getHeight() - companyTextHeight) / 2 + 15; // move up 5px from previous
    auto companyArea = juce::Rectangle<int>(getWidth() - 180, companyTextY, companyTextWidth, companyTextHeight + 2);
    if (companyTypeface)
    {
        juce::FontOptions companyFontOptions { companyTypeface };
        juce::Font companyFont(companyFontOptions.withHeight(16.0f));
        float italicSkew = -0.5f;
        juce::AffineTransform italicTransform = juce::AffineTransform::shear(static_cast<float>(italicSkew), 0.0f);
        // Multi-layered text effect (glow/copy) similar to 'whirlpool'
        for (int i = 3; i > 0; --i) {
            float yOffset = static_cast<float>(i);
            float alpha = 0.18f * (static_cast<float>(i) / 3.0f);
            g.saveState();
            g.addTransform(juce::AffineTransform::translation(static_cast<float>(companyArea.getX()), static_cast<float>(companyArea.getY()) + yOffset));
            g.addTransform(italicTransform);
            g.setFont(companyFont);
            g.setColour(juce::Colour(0xff4a9eff).withAlpha(alpha));
            g.drawText("ORCAS AUDIO", 0, 0, companyArea.getWidth(), companyArea.getHeight(), juce::Justification::centred);
            g.restoreState();
        }
        // Main highlight layer
        g.saveState();
        g.addTransform(juce::AffineTransform::translation(static_cast<float>(companyArea.getX()), static_cast<float>(companyArea.getY())));
        g.addTransform(italicTransform);
        g.setFont(companyFont);
        g.setColour(juce::Colour(0xfff0f0f0));
        g.drawText("ORCAS AUDIO", 0, 0, companyArea.getWidth(), companyArea.getHeight(), juce::Justification::centred);
        g.restoreState();
    }
    else
    {
        juce::Font fallbackFont(juce::FontOptions(16.0f).withStyle("Italic"));
        g.setFont(fallbackFont);
        g.setColour(juce::Colour(0xff4a9eff).withAlpha(0.2f));
        g.drawText("ORCAS AUDIO", companyArea.expanded(2, 2).toFloat(), juce::Justification::centred);
        g.setColour(juce::Colour(0xfff0f0f0).withAlpha(0.5f));
        g.drawText("ORCAS AUDIO", companyArea.translated(1, 1).toFloat(), juce::Justification::centred);
        g.setColour(juce::Colour(0xffa29bfe));
        g.drawText("ORCAS AUDIO", companyArea.toFloat(), juce::Justification::centred);
    }

    // Calculate centered layout with equal padding
    int totalPadding = 60; // 30px on each side
    int availableWidth = getWidth() - totalPadding;
    int sectionWidth = availableWidth / 3;
    int leftMargin = 30; // Equal padding on both sides
    
    // Enhanced section background cards with individual gradients - properly centered
    auto filterCard = juce::Rectangle<int>(leftMargin, 85, sectionWidth, getHeight() - 105);
    auto patternAreaRect = juce::Rectangle<int>(leftMargin + sectionWidth, 85, sectionWidth, getHeight() - 105);
    auto modulationCard = juce::Rectangle<int>(leftMargin + sectionWidth * 2, 85, sectionWidth, getHeight() - 105);

    // Inset for rounded corners (matches card radius)
    constexpr int cardCornerRadius = 8;
    float dividerTop = static_cast<float>(patternAreaRect.getY() + cardCornerRadius);
    float dividerBottom = static_cast<float>(patternAreaRect.getBottom() - cardCornerRadius);

    // First divider with blue-teal gradient - exactly within card area (inset for rounded corners)
    juce::ColourGradient divider1(
        juce::Colour(0xff4a9eff).withAlpha(0.6f), static_cast<float>(leftMargin + sectionWidth), dividerTop,
        juce::Colour(0xff89cdf1).withAlpha(0.3f), static_cast<float>(leftMargin + sectionWidth), dividerBottom,
        false
    );
    g.setGradientFill(divider1);
    g.drawLine(static_cast<float>(leftMargin + sectionWidth), dividerTop, static_cast<float>(leftMargin + sectionWidth), dividerBottom, 1.5f);

    // Second divider with teal-purple gradient - exactly within card area (inset for rounded corners)
    juce::ColourGradient divider2(
        juce::Colour(0xff6db4ff).withAlpha(0.6f), static_cast<float>(leftMargin + sectionWidth * 2), dividerTop,
        juce::Colour(0xffa29bfe).withAlpha(0.3f), static_cast<float>(leftMargin + sectionWidth * 2), dividerBottom,
        false
    );
    g.setGradientFill(divider2);
    g.drawLine(static_cast<float>(leftMargin + sectionWidth * 2), dividerTop, static_cast<float>(leftMargin + sectionWidth * 2), dividerBottom, 1.5f);
    
    // Filter card - blue accent
    juce::ColourGradient filterGradient(
        juce::Colour(0xff1a2a4a).withAlpha(0.5f), static_cast<float>(filterCard.getX()), static_cast<float>(filterCard.getY()),
        juce::Colour(0xff0f1a2a).withAlpha(0.3f), static_cast<float>(filterCard.getX()), static_cast<float>(filterCard.getBottom()),
        false
    );
    g.setGradientFill(filterGradient);
    g.fillRoundedRectangle(filterCard.toFloat(), 8.0f);
    
    // Pattern card - teal accent
    juce::ColourGradient patternGradient(
        juce::Colour(0xff1a3a4a).withAlpha(0.5f), static_cast<float>(patternAreaRect.getX()), static_cast<float>(patternAreaRect.getY()),
        juce::Colour(0xff0f2a3a).withAlpha(0.3f), static_cast<float>(patternAreaRect.getX()), static_cast<float>(patternAreaRect.getBottom()),
        false
    );
    g.setGradientFill(patternGradient);
    g.fillRoundedRectangle(patternAreaRect.toFloat(), 8.0f);
    
    // Modulation card - purple accent
    juce::ColourGradient modulationGradient(
        juce::Colour(0xff2a1a4a).withAlpha(0.5f), static_cast<float>(modulationCard.getX()), static_cast<float>(modulationCard.getY()),
        juce::Colour(0xff1a0f2a).withAlpha(0.3f), static_cast<float>(modulationCard.getX()), static_cast<float>(modulationCard.getBottom()),
        false
    );
    g.setGradientFill(modulationGradient);
    g.fillRoundedRectangle(modulationCard.toFloat(), 8.0f);
    
    // Card borders with accent colors and subtle shadows
    // Add subtle drop shadows first
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.fillRoundedRectangle(filterCard.toFloat().translated(2.0f, 2.0f), 8.0f);
    g.fillRoundedRectangle(patternAreaRect.toFloat().translated(2.0f, 2.0f), 8.0f);
    g.fillRoundedRectangle(modulationCard.toFloat().translated(2.0f, 2.0f), 8.0f);
    
    // Main card borders with enhanced glow
    g.setColour(juce::Colour(0xff4a9eff).withAlpha(0.5f));
    g.drawRoundedRectangle(filterCard.toFloat(), 8.0f, 1.5f);
    g.setColour(juce::Colour(0xff4a9eff).withAlpha(0.2f));
    g.drawRoundedRectangle(filterCard.toFloat().expanded(1.0f), 8.0f, 0.8f);
    
    g.setColour(juce::Colour(0xff89cdf1).withAlpha(0.5f));
    g.drawRoundedRectangle(patternAreaRect.toFloat(), 8.0f, 1.5f);
    g.setColour(juce::Colour(0xff89cdf1).withAlpha(0.2f));
    g.drawRoundedRectangle(patternAreaRect.toFloat().expanded(1.0f), 8.0f, 0.8f);
    
    g.setColour(juce::Colour(0xffa29bfe).withAlpha(0.5f));
    g.drawRoundedRectangle(modulationCard.toFloat(), 8.0f, 1.5f);
    g.setColour(juce::Colour(0xffa29bfe).withAlpha(0.2f));
    g.drawRoundedRectangle(modulationCard.toFloat().expanded(1.0f), 8.0f, 0.8f);
    
    // Enhanced section titles with individual accent colors and glow - moved left for better alignment
    g.setColour(juce::Colour(0xff4a9eff).withAlpha(0.4f));
    g.setFont(juce::Font(juce::FontOptions(10.0f).withStyle("Bold")));
    g.drawText("FILTER", leftMargin + 10, 67, sectionWidth - 20, 16, juce::Justification::centredLeft);
    
    g.setColour(juce::Colour(0xff4a9eff));
    g.drawText("FILTER", leftMargin + 10, 65, sectionWidth - 20, 16, juce::Justification::centredLeft);
    
    g.setColour(juce::Colour(0xff89cdf1).withAlpha(0.4f));
    g.drawText("PATTERN", leftMargin + sectionWidth + 10, 67, sectionWidth - 20, 16, juce::Justification::centredLeft);
    
    g.setColour(juce::Colour(0xff89cdf1));
    g.drawText("PATTERN", leftMargin + sectionWidth + 10, 65, sectionWidth - 20, 16, juce::Justification::centredLeft);
    
    g.setColour(juce::Colour(0xffa29bfe).withAlpha(0.4f));
    g.drawText("MODULATION", leftMargin + sectionWidth * 2 + 10, 67, sectionWidth - 20, 16, juce::Justification::centredLeft);
    
    g.setColour(juce::Colour(0xffa29bfe));
    g.drawText("MODULATION", leftMargin + sectionWidth * 2 + 10, 65, sectionWidth - 20, 16, juce::Justification::centredLeft);

    // --- Draw trim~ label in pattern card, above the trim slider, matching PATTERN style ---
    // Use variable names that do not contain 'patternCard' anywhere
    auto trimSectionBase = patternAreaRect;
    // These must match layoutPatternSection
    int trimLabelHeight = 16;
    int trimSliderHeight = 24;
    int trimValueLabelHeight = 18;
    int trimSectionHeight = trimLabelHeight + trimSliderHeight + trimValueLabelHeight + 10;
    auto trimSection = trimSectionBase.removeFromBottom(trimSectionHeight);
    int trimLabelX = trimSection.getX() + 10;
    int trimLabelY = trimSection.getY();
    int trimLabelWidth = trimSection.getWidth() - 20;
    // Glow layer
    g.setColour(juce::Colour(0xff89cdf1).withAlpha(0.4f));
    g.setFont(juce::Font(juce::FontOptions(10.0f).withStyle("Bold")));
    g.drawText("trim~", trimLabelX, trimLabelY + 2, trimLabelWidth, trimLabelHeight, juce::Justification::centredLeft);
    // Main text
    g.setColour(juce::Colour(0xff89cdf1));
    g.drawText("trim~", trimLabelX, trimLabelY, trimLabelWidth, trimLabelHeight, juce::Justification::centredLeft);
    
    // --- Draw downbeat LED indicator in top-right of pattern card ---
    int ledSize = 10;
    int ledX = patternAreaRect.getRight() - 20 - ledSize;
    int ledY = patternAreaRect.getY() + 15;
    
    if (isDownbeatActive && downbeatIntensity > 0.0f)
    {
        // Draw glowing LED for downbeat
        juce::Point<float> ledCenter(ledX + ledSize * 0.5f, ledY + ledSize * 0.5f);
        float glowRadius = ledSize * 0.8f * downbeatIntensity;
        
        // Outer glow
        juce::ColourGradient glowGradient(
            juce::Colour(0xff4a9eff).withAlpha(0.6f * downbeatIntensity), 
            ledCenter.x, ledCenter.y,
            juce::Colour(0xff4a9eff).withAlpha(0.0f), 
            ledCenter.x + glowRadius, ledCenter.y + glowRadius,
            true
        );
        glowGradient.addColour(0.7, juce::Colour(0xff4a9eff).withAlpha(0.3f * downbeatIntensity));
        g.setGradientFill(glowGradient);
        g.fillEllipse(ledCenter.x - glowRadius, ledCenter.y - glowRadius, glowRadius * 2, glowRadius * 2);
        
        // Inner LED core
        g.setColour(juce::Colour(0xff6db4ff).withAlpha(0.9f * downbeatIntensity));
        g.fillEllipse(ledX + 2, ledY + 2, ledSize - 4, ledSize - 4);
        
        // Bright center
        g.setColour(juce::Colour(0xfff0f0f0).withAlpha(0.8f * downbeatIntensity));
        g.fillEllipse(ledX + 3, ledY + 3, ledSize - 6, ledSize - 6);
    }
    else
    {
        // Draw dim LED when inactive
        g.setColour(juce::Colour(0xff2a3d5b).withAlpha(0.4f));
        g.fillEllipse(ledX + 1, ledY + 1, ledSize - 2, ledSize - 2);
        g.setColour(juce::Colour(0xff4a9eff).withAlpha(0.2f));
        g.drawEllipse(ledX + 1, ledY + 1, ledSize - 2, ledSize - 2, 1.0f);
    }
}


// Removed drawSectionCard method - using minimalist design instead



//==============================================================================
// Typeface members for custom fonts are declared in the header file (PluginEditor.h)

void RhythmicFilterSculptorAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Clean header area
    auto headerArea = bounds.removeFromTop(50);
    // Removed companyLabel bounds/font; text is now drawn manually in paint()
    
    // Content area layout with centered sections and equal padding
    auto contentArea = bounds.reduced(30, 10); // Equal 30px padding on left and right
    int sectionWidth = contentArea.getWidth() / 3;
    
    // Three equal sections with proper centering
    auto filterSection = contentArea.removeFromLeft(sectionWidth).reduced(10, 20);
    auto patternSection = contentArea.removeFromLeft(sectionWidth).reduced(10, 20);  
    auto modulationSection = contentArea.reduced(10, 20);
    
    // Filter section layout
    layoutFilterSection(filterSection);
    
    // Pattern section layout  
    layoutPatternSection(patternSection);
    
    // Modulation section layout
    layoutModulationSection(modulationSection);
}

void RhythmicFilterSculptorAudioProcessorEditor::layoutFilterSection(juce::Rectangle<int> area)
{
    // Store the original area for later use
    auto originalArea = area;
    
    // Section title spacing
    area.removeFromTop(20);
    area.removeFromTop(15);
    
    // Combo box and label
    filterTypeCombo.setBounds(area.removeFromTop(24).reduced(5, 2));
    auto filterTypeLabelArea = area.removeFromTop(14);
    filterTypeLabel.setBounds(filterTypeLabelArea.reduced(5, 0));
    area.removeFromTop(8);
    
    // Knob layout
    int knobHeight = 85; 
    int knobWidth = area.getWidth();
    int knobSize = std::min(knobHeight, knobWidth - 10);
    auto knobArea = area.removeFromTop(knobHeight);
    int knobX = knobArea.getX() + (knobArea.getWidth() - knobSize) / 2;
    int knobY = knobArea.getY();
    resonanceSlider.setBounds(knobX, knobY, knobSize, knobSize);
    
    // Hide the built-in textbox for resonance slider
    resonanceSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    // --- Resonance knob label layout (side-by-side, centered as a pair) ---
    int labelY = resonanceSlider.getBottom() + 8;
    int valueLabelWidth = 40; // Increased from 36
    int textLabelWidth = 85; // Increased from 70 to accommodate "Resonance"
    int totalLabelWidth = valueLabelWidth + textLabelWidth + 6; // Increased gap to 6px
    int sliderCenterX = resonanceSlider.getBounds().getCentreX();
    int startX = sliderCenterX - (totalLabelWidth / 2);
    
    // Position value label (left side of the pair)
    resonanceValueLabel.setBounds(startX-5, labelY, valueLabelWidth+10, 20);
    resonanceValueLabel.setJustificationType(juce::Justification::centred);
    resonanceValueLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff0f0f0));
    resonanceValueLabel.setFont(juce::Font(juce::FontOptions(14.0f).withStyle("Bold")));
    resonanceValueLabel.setText(juce::String(resonanceSlider.getValue(), 1), juce::dontSendNotification);
    
    // Position text label (right side of the pair)
    resonanceLabel.setBounds(startX + valueLabelWidth + 6, labelY, textLabelWidth, 20);
    resonanceLabel.setJustificationType(juce::Justification::centredLeft); // Changed to left align for better readability
    resonanceLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa29bfe));
    resonanceLabel.setFont(juce::Font(juce::FontOptions(13.0f).withStyle("Regular")));
}

void RhythmicFilterSculptorAudioProcessorEditor::layoutPatternSection(juce::Rectangle<int> area)
{
    area.removeFromTop(20); // Space for section title

    // Move components down by increasing top spacing
    area.removeFromTop(15); // Increased spacing to move components down

    // Reserve space at the bottom for trim section (label above, slider, value label below)
    int trimLabelHeight = 16;
    int trimSliderHeight = 24;
    int trimValueLabelHeight = 18;
    int trimSectionHeight = trimLabelHeight + trimSliderHeight + trimValueLabelHeight + 10; // 10px total spacing
    auto trimSection = area.removeFromBottom(trimSectionHeight);

    // Pattern controls (upper part)

    // --- Pattern label and combo ---
    patternSelectCombo.setBounds(area.removeFromTop(24).reduced(5, 2));
    // Use a wider label bounds for both labels to guarantee identical appearance and avoid text cutoff
    constexpr int labelWidth = 120;
    auto labelArea = area.removeFromTop(14);
    juce::Rectangle<int> fixedLabelBounds = labelArea.reduced(5, 0).withWidth(labelWidth).withHeight(14);
    // Center the label horizontally in the available area
    fixedLabelBounds = fixedLabelBounds.withX(labelArea.getX() + (labelArea.getWidth() - labelWidth) / 2);
    patternSelectLabel.setBounds(fixedLabelBounds);
    patternSelectLabel.setFont(juce::Font(juce::FontOptions(13.0f).withStyle("Bold")));
    patternSelectLabel.setColour(juce::Label::textColourId, juce::Colour(0xff4a9eff)); // match filter type label
    patternSelectLabel.setJustificationType(juce::Justification::centred);

    area.removeFromTop(15); // Spacing

    // --- Tempo Sync label and combo ---
    tempoSyncCombo.setBounds(area.removeFromTop(24).reduced(5, 2));
    // Use the exact same fixed bounds as patternSelectLabel
    auto tempoLabelArea = area.removeFromTop(14);
    juce::Rectangle<int> tempoLabelBounds = tempoLabelArea.reduced(5, 0).withWidth(labelWidth).withHeight(14);
    tempoLabelBounds = tempoLabelBounds.withX(tempoLabelArea.getX() + (tempoLabelArea.getWidth() - labelWidth) / 2);
    tempoSyncLabel.setBounds(tempoLabelBounds);
    tempoSyncLabel.setFont(juce::Font(juce::FontOptions(13.0f).withStyle("Bold")));
    tempoSyncLabel.setColour(juce::Label::textColourId, juce::Colour(0xff4a9eff)); // match filter type label
    tempoSyncLabel.setJustificationType(juce::Justification::centred);

    // --- Trim section (bottom of card) ---
    int trimSliderWidth = trimSection.getWidth() - 20;
    int trimSliderX = trimSection.getX() + 10;
    int trimLabelY = trimSection.getY();
    int trimSliderY = trimLabelY + trimLabelHeight + 2;
    int trimValueLabelY = trimSliderY + trimSliderHeight + 2;

    // outputTrimLabel is no longer used; trim~ is drawn in paint()

    // Slider - use rotary knob color theme for track/thumb/background
    outputTrimSlider.setBounds(trimSliderX, trimSliderY, trimSliderWidth, trimSliderHeight);
    outputTrimSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    outputTrimSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    // Use same color resources as rotary knobs (intensity/mix)
    outputTrimSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xff3d5a8c)); // same as accent color for rotary
    outputTrimSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff4a9eff)); // same as rotary knob thumb
    outputTrimSlider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xff1a2a4a).withAlpha(0.4f));

    // Value label below slider, centered
    outputTrimValueLabel.setBounds(trimSliderX, trimValueLabelY, trimSliderWidth, trimValueLabelHeight);
    outputTrimValueLabel.setJustificationType(juce::Justification::centred);
    outputTrimValueLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa29bfe)); // match purple accent
    outputTrimValueLabel.setFont(juce::Font(juce::FontOptions(13.0f).withStyle("Bold")));
    outputTrimValueLabel.setText(juce::String(outputTrimSlider.getValue(), 1) + " dB", juce::dontSendNotification);
}

void RhythmicFilterSculptorAudioProcessorEditor::layoutModulationSection(juce::Rectangle<int> area)
{
    area.removeFromTop(20); // Space for section title
    area.removeFromTop(15); // Increased spacing to move components down
    
    // Larger two-knob layout - no space reserved above knobs
    auto knobArea = area.removeFromTop(100); 
    auto leftKnobArea = knobArea.removeFromLeft(knobArea.getWidth() / 2);
    auto rightKnobArea = knobArea;
    
    // Position sliders with some padding
    intensitySlider.setBounds(leftKnobArea.reduced(5, 5));
    mixSlider.setBounds(rightKnobArea.reduced(5, 5));
    
    // Hide the built-in textbox for both sliders
    intensitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    // Get knob centers for alignment
    int intensityCenterX = intensitySlider.getBounds().getCentreX();
    int mixCenterX = mixSlider.getBounds().getCentreX();
    int valueLabelWidth = 70; // Width for value labels
    int textLabelWidth = 100; // Increased width to accommodate full text without truncation
    
    // Calculate positions for labels below the knobs
    int valueLabelY = intensitySlider.getBottom() + 8; // 8px below knobs
    int textLabelY = valueLabelY + 22; // 22px below value labels
    
    // --- Intensity knob: value and label both below ---
    // Value label below knob
    intensityValueLabel.setBounds(intensityCenterX - valueLabelWidth / 2, valueLabelY, valueLabelWidth, 20);
    intensityValueLabel.setJustificationType(juce::Justification::centred);
    intensityValueLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff0f0f0));
    intensityValueLabel.setFont(juce::Font(juce::FontOptions(14.0f).withStyle("Bold")));
    // FIX: Display actual parameter value (0-100) instead of incorrect calculation
    intensityValueLabel.setText(juce::String(intensitySlider.getValue(), 1), juce::dontSendNotification);
    
    // Text label below value - using wider width
    intensityLabel.setBounds(intensityCenterX - textLabelWidth / 2, textLabelY, textLabelWidth, 18);
    intensityLabel.setJustificationType(juce::Justification::centred);
    intensityLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa29bfe));
    intensityLabel.setFont(juce::Font(juce::FontOptions(13.0f).withStyle("Regular")));
    
    // --- Mix knob: value and label both below ---
    // Value label below knob
    mixValueLabel.setBounds(mixCenterX - valueLabelWidth / 2, valueLabelY, valueLabelWidth + 5, 20);
    mixValueLabel.setJustificationType(juce::Justification::centred);
    mixValueLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff0f0f0));
    mixValueLabel.setFont(juce::Font(juce::FontOptions(14.0f).withStyle("Bold")));
    mixValueLabel.setText(juce::String(mixSlider.getValue(), 1), juce::dontSendNotification);
    
    // Text label below value - using wider width
    mixLabel.setBounds(mixCenterX - textLabelWidth / 2, textLabelY, textLabelWidth, 18);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa29bfe));
    mixLabel.setFont(juce::Font(juce::FontOptions(13.0f).withStyle("Regular")));
}

void RhythmicFilterSculptorAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &intensitySlider)
    {
        // FIX: Display the actual parameter value (0-100) instead of incorrect calculation
        intensityValueLabel.setText(juce::String(intensitySlider.getValue(), 1), juce::dontSendNotification);
    }
    else if (slider == &mixSlider)
    {
        mixValueLabel.setText(juce::String(mixSlider.getValue(), 1), juce::dontSendNotification);
    }
    else if (slider == &resonanceSlider)
    {
        resonanceValueLabel.setText(juce::String(resonanceSlider.getValue(), 1), juce::dontSendNotification);
    }
    else if (slider == &outputTrimSlider)
    {
        outputTrimValueLabel.setText(juce::String(slider->getValue(), 1) + " dB", juce::dontSendNotification);
    }
}

void RhythmicFilterSculptorAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    juce::ignoreUnused(comboBoxThatHasChanged);
    // Handle any custom combo box behavior here if needed
    // Most parameter handling is done through the attachments
}

void RhythmicFilterSculptorAudioProcessorEditor::timerCallback()
{
    // Get current time and calculate delta
    auto currentTime = juce::Time::getMillisecondCounterHiRes();
    if (lastUpdateTime == 0.0)
    {
        lastUpdateTime = static_cast<juce::int64>(currentTime);
        return;
    }
    auto deltaTime = (currentTime - lastUpdateTime) / 1000.0;
    lastUpdateTime = static_cast<juce::int64>(currentTime);
    
    // Get host BPM and position for LED timing
    double bpm = 120.0; // Default BPM for standalone testing
    double beatPosition = 0.0;
    bool hasValidTiming = false;
    
    // Try to get host tempo information when running as plugin
    if (auto* playHead = audioProcessor.getPlayHead())
    {
        if (auto posInfo = playHead->getPosition())
        {
            if (auto bpmOpt = posInfo->getBpm())
            {
                double hostBpm = *bpmOpt;
                if (hostBpm > 0.0 && hostBpm < 999.0)
                {
                    bpm = hostBpm;
                    currentBpm = bpm;
                }
            }
            
            if (auto ppqOpt = posInfo->getPpqPosition())
            {
                double ppq = *ppqOpt;
                beatPosition = ppq; // PPQ is in quarter note beats
                hasValidTiming = true;
            }
        }
    }
    
    // LED beat detection logic: only light on first downbeat (beat 1) of each bar
    if (hasValidTiming)
    {
        double currentBeat = std::floor(beatPosition);
        int beatInBar = static_cast<int>(std::fmod(currentBeat, 4.0));
        if (currentBeat != lastBeatPosition)
        {
            if (beatInBar == 0)
            {
                isDownbeatActive = true;
                downbeatIntensity = 0.80f;
                ledColor = juce::Colour(0xff4a9eff); // blue
            }
            lastBeatPosition = currentBeat;
        }
    }
    else
    {
        // Fallback: simulate downbeat every 2 seconds at 120 BPM when no host timing
        static double fallbackTime = 0.0;
        fallbackTime += deltaTime;
        double measureDuration = (60.0 / bpm) * 4.0; // 4 beats per measure
        if (fallbackTime >= measureDuration)
        {
            isDownbeatActive = true;
            downbeatIntensity = 1.0f;
            fallbackTime = 0.0;
            ledColor = juce::Colour(0xff4a9eff); // blue (default for fallback)
        }
    }
    
    // Fade out LED intensity over 300ms
    if (isDownbeatActive && downbeatIntensity > 0.0f)
    {
        downbeatIntensity -= static_cast<float>(deltaTime / 0.3); // 300ms fade
        if (downbeatIntensity <= 0.0f)
        {
            downbeatIntensity = 0.0f;
            isDownbeatActive = false;
        }
    }
    
    // Note: When running standalone, playHead will be nullptr or provide no BPM info,
    // so we'll use our 120 BPM default for consistent testing behavior

    // Calculate animation speed: 1/2 tempo means one full cycle every 2 beats
    // At 120 BPM, that's one cycle every second (2 beats = 1 second at 120 BPM)
    double beatsPerSecond = bpm / 60.0;
    double animationSpeed = (beatsPerSecond / 2.0) * 0.5; // Half the previous speed for subtlety
    
    // Update animation phases with different frequencies for organic motion
    animationPhase += static_cast<float>(deltaTime * animationSpeed);
    intensityPhase += static_cast<float>(deltaTime * animationSpeed * 0.7); // Slightly slower
    centerOffsetPhase += static_cast<float>(deltaTime * animationSpeed * 0.5); // Even slower
    spiralPhase += static_cast<float>(deltaTime * animationSpeed * 1.3); // Slightly faster
    
    // Keep phases in reasonable range
    if (animationPhase > juce::MathConstants<float>::twoPi)
        animationPhase -= juce::MathConstants<float>::twoPi;
    if (intensityPhase > juce::MathConstants<float>::twoPi)
        intensityPhase -= juce::MathConstants<float>::twoPi;
    if (centerOffsetPhase > juce::MathConstants<float>::twoPi)
        centerOffsetPhase -= juce::MathConstants<float>::twoPi;
    if (spiralPhase > juce::MathConstants<float>::twoPi)
        spiralPhase -= juce::MathConstants<float>::twoPi;
    
    // Trigger repaint for animation
    repaint();
}

void RhythmicFilterSculptorAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    slider.addListener(this);
    
    // Set proper ranges to match parameter definitions
    if (&slider == &intensitySlider)
    {
        slider.setRange(0.0, 100.0, 0.1); // Match ParameterRanges::percentRange
    }
    else if (&slider == &mixSlider)
    {
        slider.setRange(0.0, 100.0, 0.1); // Match ParameterRanges::percentRange  
    }
    else if (&slider == &resonanceSlider)
    {
        slider.setRange(0.1, 10.0, 0.01); // Match ParameterRanges::resonanceRange
    }
    
    // Minimalist colors
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff0f0f0));
    
    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xffa29bfe)); // Updated to use accent color
    label.setFont(juce::Font(juce::FontOptions(10.0f).withStyle("Regular")));
}

void RhythmicFilterSculptorAudioProcessorEditor::setupComboBox(juce::ComboBox& combo, juce::Label& label, const juce::String& labelText)
{
    addAndMakeVisible(combo);
    combo.addListener(this);
    
    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xff89cdf1)); // Updated to use accent color
    label.setFont(juce::Font(juce::FontOptions(10.0f).withStyle("Regular")));
}
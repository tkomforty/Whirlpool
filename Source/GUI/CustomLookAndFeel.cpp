#include <JuceHeader.h>
#include "CustomLookAndFeel.h"

//==============================================================================
// Color palette definition - Updated to blue/purple theme to match target GUI
const juce::Colour CustomLookAndFeel::shadow           = juce::Colour(0xff4a5c8c);  // Blue shadow
const juce::Colour CustomLookAndFeel::shingleFawn      = juce::Colour(0xff2d3a5c);  // Dark blue
const juce::Colour CustomLookAndFeel::englishWalnut    = juce::Colour(0xff1a1f3d);  // Very dark blue
const juce::Colour CustomLookAndFeel::seaNymph         = juce::Colour(0xff5a7aa0);  // Light blue accent
const juce::Colour CustomLookAndFeel::cuttySark        = juce::Colour(0xff3d5a8c);  // Medium blue

// Calculated colors
const juce::Colour CustomLookAndFeel::lightText        = juce::Colours::white.withAlpha(0.9f);
const juce::Colour CustomLookAndFeel::mediumText       = juce::Colours::white.withAlpha(0.7f);
const juce::Colour CustomLookAndFeel::darkText         = juce::Colours::white.withAlpha(0.5f);
const juce::Colour CustomLookAndFeel::accent           = CustomLookAndFeel::seaNymph;
const juce::Colour CustomLookAndFeel::highlight        = CustomLookAndFeel::cuttySark;
const juce::Colour CustomLookAndFeel::background       = CustomLookAndFeel::englishWalnut;
const juce::Colour CustomLookAndFeel::cardBackground   = CustomLookAndFeel::shingleFawn.withAlpha(0.3f);
const juce::Colour CustomLookAndFeel::border           = CustomLookAndFeel::shadow.withAlpha(0.6f);

//==============================================================================
CustomLookAndFeel::CustomLookAndFeel()
{
    // Set default colors
    setColour(juce::ResizableWindow::backgroundColourId, background);
    setColour(juce::TextButton::buttonColourId, cardBackground);
    setColour(juce::TextButton::textColourOffId, lightText);
    setColour(juce::TextButton::textColourOnId, lightText);
    
    // Slider colors
    setColour(juce::Slider::thumbColourId, accent);
    setColour(juce::Slider::trackColourId, border);
    setColour(juce::Slider::rotarySliderFillColourId, accent);
    setColour(juce::Slider::rotarySliderOutlineColourId, border);
    setColour(juce::Slider::textBoxTextColourId, lightText);
    setColour(juce::Slider::textBoxBackgroundColourId, cardBackground);
    setColour(juce::Slider::textBoxOutlineColourId, border);
    
    // ComboBox colors
    setColour(juce::ComboBox::backgroundColourId, cardBackground);
    setColour(juce::ComboBox::textColourId, lightText);
    setColour(juce::ComboBox::outlineColourId, border);
    setColour(juce::ComboBox::buttonColourId, accent);
    setColour(juce::ComboBox::arrowColourId, lightText);
    
    // Label colors
    setColour(juce::Label::textColourId, mediumText);
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                        juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(8);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin(6.0f, radius * 0.4f);
    auto arcRadius = radius - lineW * 0.5f;
    auto centre = bounds.getCentre();
    
    // Draw all knob components using helper methods
    drawKnobShadow(g, centre, radius);
    drawKnobTrack(g, centre, arcRadius, lineW, rotaryStartAngle, rotaryEndAngle);
    drawKnobNotches(g, centre, arcRadius, lineW, rotaryStartAngle, rotaryEndAngle);
    drawKnobValueArc(g, centre, arcRadius, lineW, rotaryStartAngle, toAngle, slider.isEnabled());
    drawKnobThumb(g, centre, arcRadius, lineW, toAngle);
    drawKnobCenterCap(g, centre, radius);
}

void CustomLookAndFeel::drawKnobShadow(juce::Graphics& g, juce::Point<float> centre, float radius) const
{
    // Enhanced shadow with multiple layers for depth
    auto shadowRadius = radius + 3.0f;
    
    // Outer shadow
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.fillEllipse(juce::Rectangle<float>(shadowRadius * 2.2f, shadowRadius * 2.2f)
                  .withCentre(centre.translated(2.0f, 3.0f)));
    
    // Inner shadow
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillEllipse(juce::Rectangle<float>(shadowRadius * 2, shadowRadius * 2)
                  .withCentre(centre.translated(1.0f, 1.5f)));
}

void CustomLookAndFeel::drawKnobTrack(juce::Graphics& g, juce::Point<float> centre, float arcRadius, 
                                     float lineW, float startAngle, float endAngle) const
{
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius,
                               0.0f, startAngle, endAngle, true);
    
    // Enhanced track with bezel effect
    // Outer darker ring
    auto outerTrackGradient = juce::ColourGradient(border.darker(0.5f), centre.x, centre.y - arcRadius,
                                                  border.darker(0.2f), centre.x, centre.y + arcRadius, false);
    g.setGradientFill(outerTrackGradient);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW * 1.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Main track
    auto trackGradient = juce::ColourGradient(border.darker(0.3f), centre.x, centre.y - arcRadius,
                                            border.brighter(0.1f), centre.x, centre.y + arcRadius, false);
    g.setGradientFill(trackGradient);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Inner highlight
    g.setColour(border.brighter(0.3f).withAlpha(0.6f));
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW * 0.3f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void CustomLookAndFeel::drawKnobValueArc(juce::Graphics& g, juce::Point<float> centre, float arcRadius, 
                                        float lineW, float startAngle, float toAngle, bool isEnabled) const
{
    if (!isEnabled) return;
    
    juce::Path valueArc;
    valueArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius,
                          0.0f, startAngle, toAngle, true);
    
    // Calculate normalized value for dynamic coloring (approximate)
    auto normalizedValue = (toAngle - startAngle) / (juce::MathConstants<float>::pi * 1.5f); // Assuming ~270 degree range
    normalizedValue = juce::jlimit(0.0f, 1.0f, normalizedValue);
    
    // Get dynamic colors based on value
    auto dynamicAccent = getDynamicAccentColor(normalizedValue);
    auto dynamicHighlight = getDynamicHighlightColor(normalizedValue);
    
    // Multi-layer glow effect with dynamic colors
    // Outer glow
    g.setColour(dynamicAccent.withAlpha(0.2f));
    g.strokePath(valueArc, juce::PathStrokeType(lineW * 2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Mid glow
    g.setColour(dynamicAccent.withAlpha(0.4f));
    g.strokePath(valueArc, juce::PathStrokeType(lineW * 1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Main value arc with enhanced gradient using dynamic colors
    auto valueGradient = juce::ColourGradient(dynamicAccent.brighter(0.5f), centre.x - arcRadius * 0.5f, centre.y - arcRadius * 0.5f,
                                            dynamicHighlight.brighter(0.3f), centre.x + arcRadius * 0.5f, centre.y + arcRadius * 0.5f, false);
    g.setGradientFill(valueGradient);
    g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Inner bright core
    g.setColour(dynamicAccent.brighter(0.8f).withAlpha(0.9f));
    g.strokePath(valueArc, juce::PathStrokeType(lineW * 0.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Specular highlight along the arc
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.strokePath(valueArc, juce::PathStrokeType(lineW * 0.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void CustomLookAndFeel::drawKnobThumb(juce::Graphics& g, juce::Point<float> centre, float arcRadius, 
                                     float lineW, float angle) const
{
    // Enhanced thumb design - arrow pointer style
    auto thumbLength = lineW * 1.8f;
    auto thumbWidth = lineW * 0.8f;
    
    juce::Point<float> thumbTip(centre.x + arcRadius * std::cos(angle - juce::MathConstants<float>::halfPi),
                               centre.y + arcRadius * std::sin(angle - juce::MathConstants<float>::halfPi));
    
    juce::Point<float> thumbBase(centre.x + (arcRadius - thumbLength) * std::cos(angle - juce::MathConstants<float>::halfPi),
                                centre.y + (arcRadius - thumbLength) * std::sin(angle - juce::MathConstants<float>::halfPi));
    
    // Create arrow-shaped thumb
    juce::Path thumbPath;
    auto perpendicular = angle + juce::MathConstants<float>::halfPi;
    
    juce::Point<float> thumbSide1(thumbBase.x + thumbWidth * std::cos(perpendicular),
                                 thumbBase.y + thumbWidth * std::sin(perpendicular));
    juce::Point<float> thumbSide2(thumbBase.x - thumbWidth * std::cos(perpendicular),
                                 thumbBase.y - thumbWidth * std::sin(perpendicular));
    
    thumbPath.addTriangle(thumbTip, thumbSide1, thumbSide2);
    
    // Draw thumb shadow
    auto shadowOffset = 1.5f;
    juce::Path shadowPath = thumbPath;
    shadowPath.applyTransform(juce::AffineTransform::translation(shadowOffset, shadowOffset));
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillPath(shadowPath);
    
    // Draw thumb with gradient
    auto thumbGradient = juce::ColourGradient(lightText.brighter(0.4f), thumbBase.x, thumbBase.y - thumbLength,
                                            lightText.darker(0.2f), thumbBase.x, thumbBase.y + thumbLength, false);
    g.setGradientFill(thumbGradient);
    g.fillPath(thumbPath);
    
    // Add metallic highlight
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    juce::Path highlightPath;
    highlightPath.addTriangle(thumbTip, 
                             thumbSide1.translated(-thumbWidth * 0.3f, -thumbWidth * 0.3f),
                             thumbSide2.translated(thumbWidth * 0.3f, -thumbWidth * 0.3f));
    g.fillPath(highlightPath);
}

void CustomLookAndFeel::drawKnobCenterCap(juce::Graphics& g, juce::Point<float> centre, float radius) const
{
    auto centerRadius = radius * 0.20f;
    auto centerBounds = juce::Rectangle<float>(centerRadius * 2, centerRadius * 2).withCentre(centre);
    
    // Enhanced multi-layer center cap
    
    // Outer shadow
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.fillEllipse(centerBounds.translated(1.5f, 2.0f));
    
    // Outer bezel ring
    auto outerBezelRadius = centerRadius * 1.1f;
    auto outerBezelBounds = juce::Rectangle<float>(outerBezelRadius * 2, outerBezelRadius * 2).withCentre(centre);
    auto bezelGradient = juce::ColourGradient(border.brighter(0.3f), centre.x, centre.y - outerBezelRadius,
                                            border.darker(0.4f), centre.x, centre.y + outerBezelRadius, false);
    g.setGradientFill(bezelGradient);
    g.fillEllipse(outerBezelBounds);
    
    // Main center cap with enhanced gradient
    auto centerGradient = juce::ColourGradient(cardBackground.brighter(0.4f), centre.x, centre.y - centerRadius,
                                             cardBackground.darker(0.4f), centre.x, centre.y + centerRadius, false);
    g.setGradientFill(centerGradient);
    g.fillEllipse(centerBounds);
    
    // Inner recessed circle
    auto innerRadius = centerRadius * 0.65f;
    auto innerBounds = juce::Rectangle<float>(innerRadius * 2, innerRadius * 2).withCentre(centre);
    auto innerGradient = juce::ColourGradient(cardBackground.darker(0.5f), centre.x, centre.y - innerRadius,
                                            cardBackground.darker(0.2f), centre.x, centre.y + innerRadius, false);
    g.setGradientFill(innerGradient);
    g.fillEllipse(innerBounds);
      // Accent ring with subtle dynamic color hint
    auto centerAccentColor = accent.withAlpha(0.6f);
    g.setColour(centerAccentColor);
    g.drawEllipse(innerBounds.expanded(1.0f), 1.2f);
    
    // Center indicator dot with glow
    auto dotRadius = centerRadius * 0.18f;
    auto dotBounds = juce::Rectangle<float>(dotRadius * 2, dotRadius * 2).withCentre(centre);
    
    // Dot glow
    g.setColour(centerAccentColor.withAlpha(0.24f));
    g.fillEllipse(dotBounds.expanded(2.0f));
    
    // Main dot
    g.setColour(centerAccentColor.brighter(0.25f));
    g.fillEllipse(dotBounds);
    
    // Dot highlight
    g.setColour(juce::Colours::green.withAlpha(0.28f));
    g.fillEllipse(juce::Rectangle<float>(dotRadius, dotRadius).withCentre(centre.translated(-dotRadius * 0.3f, -dotRadius * 0.3f)));
}

void CustomLookAndFeel::drawKnobNotches(juce::Graphics& g, juce::Point<float> centre, float arcRadius, 
                                       float lineW, float startAngle, float endAngle) const
{
    // Draw subtle notch indicators at key positions
    auto notchRadius = arcRadius + lineW * 0.8f;
    auto notchLength = lineW * 0.6f;
    auto notchWidth = 1.5f;
    
    // Calculate notch positions (start, center, end, plus intermediate marks)
    std::vector<float> notchAngles;
    
    // Main notches (start, center, end)
    notchAngles.push_back(startAngle);
    notchAngles.push_back(startAngle + (endAngle - startAngle) * 0.5f); // Center
    notchAngles.push_back(endAngle);
    
    // Intermediate notches (quarter positions)
    notchAngles.push_back(startAngle + (endAngle - startAngle) * 0.25f);
    notchAngles.push_back(startAngle + (endAngle - startAngle) * 0.75f);
    
    for (size_t i = 0; i < notchAngles.size(); ++i)
    {
        auto angle = notchAngles[i];
        auto isMainNotch = (i < 3); // First three are main notches
        auto currentNotchLength = isMainNotch ? notchLength : notchLength * 0.6f;
        auto currentNotchWidth = isMainNotch ? notchWidth : notchWidth * 0.8f;
        
        // Calculate notch line positions
        juce::Point<float> notchStart(centre.x + notchRadius * std::cos(angle - juce::MathConstants<float>::halfPi),
                                     centre.y + notchRadius * std::sin(angle - juce::MathConstants<float>::halfPi));
        
        juce::Point<float> notchEnd(centre.x + (notchRadius + currentNotchLength) * std::cos(angle - juce::MathConstants<float>::halfPi),
                                   centre.y + (notchRadius + currentNotchLength) * std::sin(angle - juce::MathConstants<float>::halfPi));
        
        // Draw notch with subtle styling
        g.setColour(isMainNotch ? mediumText.withAlpha(0.8f) : darkText.withAlpha(0.6f));
        g.drawLine(notchStart.x, notchStart.y, notchEnd.x, notchEnd.y, currentNotchWidth);
        
        // Add subtle glow for main notches
        if (isMainNotch)
        {
            g.setColour(accent.withAlpha(0.3f));
            g.drawLine(notchStart.x, notchStart.y, notchEnd.x, notchEnd.y, currentNotchWidth * 2.0f);
        }
    }
}

void CustomLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                    int buttonX, int buttonY, int buttonW, int buttonH,
                                    juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();
    auto cornerSize = 6.0f;
      // Background
    auto gradient = juce::ColourGradient(cardBackground.brighter(0.1f), 0.0f, 0.0f,
                                       cardBackground.darker(0.1f), 0.0f, static_cast<float>(height), false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, cornerSize);
    
    // Border
    g.setColour(isButtonDown ? accent : border);
    g.drawRoundedRectangle(bounds.reduced(0.5f), cornerSize, 1.5f);
    
    // Arrow button area
    auto arrowZone = juce::Rectangle<int>(buttonX, buttonY, buttonW, buttonH).toFloat();
    
    // Draw arrow
    auto arrowX = arrowZone.getCentreX();
    auto arrowY = arrowZone.getCentreY();
    auto arrowSize = 4.0f;
    
    juce::Path arrow;
    arrow.addTriangle(arrowX - arrowSize, arrowY - arrowSize * 0.5f,
                     arrowX + arrowSize, arrowY - arrowSize * 0.5f,
                     arrowX, arrowY + arrowSize * 0.5f);
    
    g.setColour(box.findColour(juce::ComboBox::arrowColourId));
    g.fillPath(arrow);
}

void CustomLookAndFeel::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
{
    label.setBounds(8, 0, box.getWidth() - 30, box.getHeight());
    label.setFont(getComboBoxFont(box));
}

juce::Font CustomLookAndFeel::getComboBoxFont(juce::ComboBox& box)
{
    return juce::Font(juce::FontOptions(juce::jmin(15.0f, (float)box.getHeight() * 0.6f)));
}

void CustomLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.fillAll(label.findColour(juce::Label::backgroundColourId));
    
    if (!label.isBeingEdited())
    {
        auto alpha = label.isEnabled() ? 1.0f : 0.5f;
        auto font = getLabelFont(label);
        
        g.setColour(label.findColour(juce::Label::textColourId).withMultipliedAlpha(alpha));
        g.setFont(font);
        
        auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                        juce::jmax(1, (int)(textArea.getHeight() / font.getHeight())),
                        label.getMinimumHorizontalScale());
    }
}

juce::Font CustomLookAndFeel::getLabelFont(juce::Label& label)
{
    return juce::Font(juce::FontOptions(juce::jmin(13.0f, (float)label.getHeight() * 0.8f)));
}

juce::BorderSize<int> CustomLookAndFeel::getLabelBorderSize(juce::Label& label)
{
    juce::ignoreUnused(label);
    // Balanced padding around labels for good spacing without taking too much room
    return juce::BorderSize<int>(3, 5, 3, 5); // top, left, bottom, right padding - balanced
}

juce::Slider::SliderLayout CustomLookAndFeel::getSliderLayout(juce::Slider& slider)
{
    // Get the default layout
    auto layout = LookAndFeel_V4::getSliderLayout(slider);
    
    // Shift the text box 20 pixels to the left
    layout.textBoxBounds = layout.textBoxBounds.translated(-20, 0);
    
    return layout;
}

juce::Colour CustomLookAndFeel::getDynamicAccentColor(float value) const
{
    // Create dynamic color shifts based on parameter value (0.0 to 1.0)
    // Low values: cooler blue tones
    // Mid values: balanced blue-purple
    // High values: warmer purple-pink tones
    
    if (value < 0.33f)
    {
        // Cool blue range
        auto factor = value / 0.33f;
        return seaNymph.interpolatedWith(accent, factor);
    }
    else if (value < 0.66f)
    {
        // Mid range - standard accent
        return accent;
    }
    else
    {
        // Warm range - more purple/pink
        auto factor = (value - 0.66f) / 0.34f;
        return accent.interpolatedWith(juce::Colour(0xffa29bfe), factor); // Purple accent
    }
}

juce::Colour CustomLookAndFeel::getDynamicHighlightColor(float value) const
{
    // Complementary highlight colors that work with the dynamic accent
    if (value < 0.33f)
    {
        return cuttySark.brighter(0.2f);
    }
    else if (value < 0.66f)
    {
        return highlight;
    }
    else
    {
        return highlight.interpolatedWith(juce::Colour(0xff89cdf1), (value - 0.66f) / 0.34f); // Teal highlight
    }
}

//==============================================================================
// Whirlpool Pattern Methods
//==============================================================================

void CustomLookAndFeel::drawWhirlpoolPattern(juce::Graphics& g, juce::Rectangle<float> bounds, float intensity) const
{
    if (!whirlpoolConfig.enabled) return;
    
    // Professional subtle intensity - balanced for readability
    float actualIntensity = intensity * whirlpoolConfig.intensity * 1.2f;
    
    // Calculate center with subtle offset for natural asymmetry
    auto center = bounds.getCentre().translated(
        bounds.getWidth() * whirlpoolConfig.centerOffsetX,
        bounds.getHeight() * whirlpoolConfig.centerOffsetY
    );
    auto maxRadius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.97f;
    
    // Save the current state
    juce::Graphics::ScopedSaveState state(g);
    
    // Set clipping to bounds to keep pattern contained
    g.reduceClipRegion(bounds.toNearestInt());
    
    // Layer 1: Subtle base ripples for texture
    drawConcentricRipples(g, center, maxRadius * 1.1f, whirlpoolConfig.rippleCount + 4, actualIntensity * 0.3f);
    
    // Layer 2: Primary spiral with professional refinement
    drawSpiralPattern(g, center, maxRadius * 0.9f, whirlpoolConfig.spiralCount, actualIntensity * 0.6f);
    
    // Layer 3: Secondary counter-spiral for depth (removed to eliminate visual artifact)
    // drawSpiralPattern(g, center.translated(maxRadius * 0.08f, -maxRadius * 0.03f), 
    //                  maxRadius * 0.65f, whirlpoolConfig.spiralCount + 1, actualIntensity * 0.4f);
    
    // Layer 4: Refined flow lines for subtle movement
    drawFlowLines(g, bounds, whirlpoolConfig.flowLineCount + 5, actualIntensity * 0.35f);
    
    // Layer 5: Professional radial gradient for depth and polish
    juce::ColourGradient innerGlow(
        seaNymph.withAlpha(actualIntensity * 0.12f), center.x, center.y,
        juce::Colours::transparentBlack, center.x, center.y,
        true
    );
    innerGlow.addColour(0.2, accent.withAlpha(actualIntensity * 0.08f));
    innerGlow.addColour(0.5, seaNymph.darker(0.2f).withAlpha(actualIntensity * 0.04f));
    innerGlow.addColour(0.8, juce::Colours::transparentBlack);
    g.setGradientFill(innerGlow);
    g.fillEllipse(juce::Rectangle<float>(maxRadius * 2.2f, maxRadius * 2.2f).withCentre(center));
    
    // Layer 6: Outer ambient glow for professional finish
    juce::ColourGradient outerGlow(
        juce::Colours::transparentBlack, center.x, center.y,
        cuttySark.withAlpha(actualIntensity * 0.06f), center.x, center.y,
        true
    );
    g.setGradientFill(outerGlow);
    g.fillEllipse(juce::Rectangle<float>(maxRadius * 3.0f, maxRadius * 3.0f).withCentre(center));
}

void CustomLookAndFeel::drawWhirlpoolPattern(juce::Graphics& g, juce::Rectangle<float> bounds) const
{
    // Use only config values - no external intensity parameter
    drawWhirlpoolPattern(g, bounds, whirlpoolConfig.intensity);
}

void CustomLookAndFeel::drawSpiralPattern(juce::Graphics& g, juce::Point<float> center, float radius, int spirals, float intensity) const
{
    // Create elegant logarithmic spiral pattern with professional refinement
    juce::Path spiralPath;
    
    const float angleIncrement = 0.08f; // Smoother curve
    const float maxAngle = juce::MathConstants<float>::twoPi * spirals;
    const float spiralTightness = 0.18f; // More refined spiral decay
    bool firstPoint = true;
    
    for (float angle = 0; angle < maxAngle; angle += angleIncrement)
    {
        // Professional logarithmic spiral: r = a * e^(b * theta)
        float normalizedAngle = angle / maxAngle;
        float spiralRadius = radius * std::exp(-angle * spiralTightness) * (1.0f - normalizedAngle * 0.1f);
        
        // Add subtle mathematical variation for organic feel
        float variation = 1.0f + 0.05f * std::sin(angle * 3.0f) * (1.0f - normalizedAngle);
        spiralRadius *= variation;
        
        float x = center.x + spiralRadius * std::cos(angle);
        float y = center.y + spiralRadius * std::sin(angle);
        
        if (firstPoint)
        {
            spiralPath.startNewSubPath(x, y);
            firstPoint = false;
        }
        else
        {
            spiralPath.lineTo(x, y);
        }
    }
    
    // Professional gradient with refined color transitions
    juce::ColourGradient spiralGradient(
        seaNymph.withAlpha(intensity * 0.7f), center.x - radius, center.y - radius,
        cuttySark.withAlpha(intensity * 0.25f), center.x + radius, center.y + radius,
        false
    );
    spiralGradient.addColour(0.3, accent.withAlpha(intensity * 0.5f));
    spiralGradient.addColour(0.7, seaNymph.darker(0.4f).withAlpha(intensity * 0.15f));
    
    // Draw main spiral with refined stroke
    g.setGradientFill(spiralGradient);
    g.strokePath(spiralPath, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Add subtle professional glow
    g.setColour(accent.withAlpha(intensity * 0.2f));
    g.strokePath(spiralPath, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void CustomLookAndFeel::drawConcentricRipples(juce::Graphics& g, juce::Point<float> center, float maxRadius, int rippleCount, float intensity) const
{
    // Professional concentric ripples with elegant spacing and alpha distribution
    for (int i = 1; i <= rippleCount; ++i)
    {
        float normalizedIndex = (float)i / (float)rippleCount;
        
        // Professional spacing with subtle logarithmic distribution
        float rippleRadius = maxRadius * (0.3f + 0.7f * normalizedIndex);
        
        // Refined alpha calculation for professional fade
        float baseFade = 1.0f - normalizedIndex;
        float alpha = intensity * baseFade * baseFade * 0.6f; // Squared falloff for smoother transition
        
        // Alternate between complementary colors for subtle visual rhythm
        auto rippleColor = (i % 3 == 0) ? seaNymph.withAlpha(alpha) :
                          (i % 3 == 1) ? cuttySark.withAlpha(alpha * 0.8f) :
                                        accent.darker(0.3f).withAlpha(alpha * 0.6f);
        
        // Draw main ripple with professional weight
        g.setColour(rippleColor);
        g.drawEllipse(juce::Rectangle<float>(rippleRadius * 2, rippleRadius * 2).withCentre(center), 1.2f);
        
        // Add selective emphasis to key ripples
        if (i % 4 == 0 || i == rippleCount / 2)
        {
            // Subtle inner glow for depth
            g.setColour(rippleColor.withAlpha(alpha * 0.3f));
            g.drawEllipse(juce::Rectangle<float>(rippleRadius * 2, rippleRadius * 2).withCentre(center), 2.5f);
        }
        
        // Add micro-variations for organic feel on every 5th ripple
        if (i % 5 == 0 && i > 2)
        {
            float variation = 0.95f + 0.1f * std::sin(i * 0.7f);
            g.setColour(seaNymph.darker(0.1f).withAlpha(alpha * 0.4f));
            g.drawEllipse(juce::Rectangle<float>(rippleRadius * 2 * variation, rippleRadius * 2 * variation).withCentre(center), 0.8f);
        }
    }
}

void CustomLookAndFeel::drawFlowLines(juce::Graphics& g, juce::Rectangle<float> bounds, int lineCount, float intensity) const
{
    // Professional flowing curves that suggest subtle water movement
    auto center = bounds.getCentre();
    auto maxDimension = juce::jmax(bounds.getWidth(), bounds.getHeight());
    
    for (int i = 0; i < lineCount; ++i)
    {
        // Create elegant bezier curves with mathematical precision
        juce::Path flowPath;
        
        float linePhase = (juce::MathConstants<float>::twoPi / lineCount) * i;
        float radiusVariation = 0.25f + 0.15f * std::sin(i * 0.8f); // Organic radius variation
        float baseRadius = maxDimension * radiusVariation;
        
        // Professional curve generation with golden ratio influences
        float goldenAngle = linePhase + (i * 2.39996f); // Approximation of golden angle
        
        auto startPoint = center.translated(
            baseRadius * 0.7f * std::cos(goldenAngle),
            baseRadius * 0.7f * std::sin(goldenAngle)
        );
        
        auto controlPoint1 = center.translated(
            baseRadius * 1.2f * std::cos(goldenAngle + 0.6f),
            baseRadius * 1.2f * std::sin(goldenAngle + 0.6f)
        );
        
        auto controlPoint2 = center.translated(
            baseRadius * 0.9f * std::cos(goldenAngle + 1.4f),
            baseRadius * 0.9f * std::sin(goldenAngle + 1.4f)
        );
        
        auto endPoint = center.translated(
            baseRadius * 0.6f * std::cos(goldenAngle + 2.1f),
            baseRadius * 0.6f * std::sin(goldenAngle + 2.1f)
        );
        
        // Create smooth cubic bezier curve
        flowPath.startNewSubPath(startPoint);
        flowPath.cubicTo(controlPoint1, controlPoint2, endPoint);
        
        // Professional color distribution with subtle variations
        float normalizedIndex = (float)i / (float)lineCount;
        float alphaBase = intensity * (0.3f + 0.4f * std::sin(normalizedIndex * juce::MathConstants<float>::pi));
        
        // Sophisticated color selection
        juce::Colour flowColor;
        switch (i % 4)
        {
            case 0: flowColor = seaNymph.withAlpha(alphaBase); break;
            case 1: flowColor = cuttySark.withAlpha(alphaBase * 0.8f); break;
            case 2: flowColor = accent.darker(0.2f).withAlpha(alphaBase * 0.9f); break;
            case 3: flowColor = seaNymph.darker(0.3f).withAlpha(alphaBase * 0.7f); break;
        }
        
        // Draw with professional stroke weight variation
        float strokeWeight = 1.2f + 0.6f * normalizedIndex;
        g.setColour(flowColor);
        g.strokePath(flowPath, juce::PathStrokeType(strokeWeight, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        // Add subtle highlight to selected curves for depth
        if (i % 6 == 0)
        {
            g.setColour(flowColor.brighter(0.3f).withAlpha(alphaBase * 0.3f));
            g.strokePath(flowPath, juce::PathStrokeType(strokeWeight * 0.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }
    }
}

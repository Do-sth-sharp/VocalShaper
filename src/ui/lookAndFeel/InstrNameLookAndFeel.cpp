#include "InstrNameLookAndFeel.h"
#include "../misc/ColorMap.h"

InstrNameLookAndFeel::InstrNameLookAndFeel()
    : SeqTrackLookAndFeel() {}

juce::Font InstrNameLookAndFeel::getTextButtonFont(
    juce::TextButton&, int buttonHeight) {
    return juce::Font(juce::jmin(15.0f, (float)buttonHeight * 0.6f));
}

void InstrNameLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& b,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
    juce::Font font(getTextButtonFont(b, b.getHeight()));
    g.setFont(font);
    g.setColour(b.findColour(b.getToggleState() ? juce::TextButton::textColourOnId
        : juce::TextButton::textColourOffId)
        .withMultipliedAlpha(b.isEnabled() ? 1.0f : 0.5f));

    const int yIndent = juce::jmin(4, b.proportionOfHeight(0.3f));
    const int cornerSize = juce::jmin(b.getHeight(), b.getWidth()) / 2;

    const int fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
    const int leftIndent = juce::jmin(fontHeight, 2 + cornerSize / (b.isConnectedOnLeft() ? 4 : 2));
    const int rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (b.isConnectedOnRight() ? 4 : 2));
    const int textWidth = b.getWidth() - leftIndent - rightIndent;

    if (textWidth > 0)
        g.drawFittedText(b.getButtonText(),
            leftIndent, yIndent, textWidth, b.getHeight() - yIndent * 2,
            juce::Justification::centred, 1, 0.75f);
}

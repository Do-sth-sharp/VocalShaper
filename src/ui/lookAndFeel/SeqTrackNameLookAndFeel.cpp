#include "SeqTrackNameLookAndFeel.h"
#include "../misc/ColorMap.h"

SeqTrackNameLookAndFeel::SeqTrackNameLookAndFeel()
	: MainLookAndFeel() {
	/** Buttons */
	this->setColour(juce::TextButton::ColourIds::buttonColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextButton::ColourIds::buttonOnColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextButton::ColourIds::textColourOffId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::TextButton::ColourIds::textColourOnId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::ComboBox::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
}

void SeqTrackNameLookAndFeel::drawButtonBackground(
	juce::Graphics& g, juce::Button& b,
	const juce::Colour& backgroundColour,
	bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) {
    auto cornerSize = 6.0f;
    auto bounds = b.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

    auto baseColour = backgroundColour
        .withMultipliedAlpha(b.isEnabled() ? 1.0f : 0.5f);

    g.setColour(baseColour);

    auto flatOnLeft = b.isConnectedOnLeft();
    auto flatOnRight = b.isConnectedOnRight();
    auto flatOnTop = b.isConnectedOnTop();
    auto flatOnBottom = b.isConnectedOnBottom();

    if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom) {
        juce::Path path;
        path.addRoundedRectangle(bounds.getX(), bounds.getY(),
            bounds.getWidth(), bounds.getHeight(),
            cornerSize, cornerSize,
            !(flatOnLeft || flatOnTop),
            !(flatOnRight || flatOnTop),
            !(flatOnLeft || flatOnBottom),
            !(flatOnRight || flatOnBottom));

        g.fillPath(path);

        g.setColour(b.findColour(juce::ComboBox::outlineColourId));
        g.strokePath(path, juce::PathStrokeType(1.0f));
    }
    else {
        g.fillRoundedRectangle(bounds, cornerSize);

        g.setColour(b.findColour(juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
    }
}

void SeqTrackNameLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& b,
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
            juce::Justification::centredLeft, 2);
}

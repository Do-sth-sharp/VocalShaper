#include "SourceViewLookAndFeel.h"
#include "../misc/ColorMap.h"
#include "../Utils.h"

SourceViewLookAndFeel::SourceViewLookAndFeel()
	: MainLookAndFeel() {
	/** Background */
	this->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));

	/** List Box */
	this->setColour(juce::ListBox::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::ListBox::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::ListBox::ColourIds::outlineColourId + 1,
		ColorMap::getInstance()->get("ThemeColorA1"));/** Outline Focused */
	this->setColour(juce::ListBox::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));

	/** Name Editor */
	this->setColour(juce::TextEditor::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextEditor::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::TextEditor::ColourIds::highlightColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	this->setColour(juce::TextEditor::ColourIds::highlightedTextColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextEditor::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextEditor::ColourIds::focusedOutlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextEditor::ColourIds::shadowColourId,
		juce::Colour::fromRGBA(0, 0, 0, 0));
	this->setColour(juce::TextEditor::ColourIds::shadowColourId + 1,
		ColorMap::getInstance()->get("ThemeColorB9"));/**< Empty Text */
	this->setColour(juce::CaretComponent::ColourIds::caretColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));

	/** Text */
	this->setColour(juce::Label::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));/** Item Background */
	this->setColour(juce::Label::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::Label::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB4"));/** Selected Bar */
	this->setColour(juce::Label::ColourIds::backgroundWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::Label::ColourIds::textWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::Label::ColourIds::outlineWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));/** Selected Bar Highlighted */

	/** Synthesizer Name */
	this->setColour(juce::TextButton::ColourIds::buttonColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextButton::ColourIds::buttonOnColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextButton::ColourIds::textColourOffId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::TextButton::ColourIds::textColourOnId,
		ColorMap::getInstance()->get("ThemeColorB10"));
}

juce::Font SourceViewLookAndFeel::getTextButtonFont(juce::TextButton& b, int) {
	auto screenSize = utils::getScreenSize(&b);
	return juce::Font{ screenSize.getHeight() * 0.015f };
}

void SourceViewLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
	const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted,
	bool shouldDrawButtonAsDown) {
	auto cornerSize = 3.0f;
	auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

	auto baseColour = backgroundColour;

	g.setColour(baseColour);

	auto flatOnLeft = button.isConnectedOnLeft();
	auto flatOnRight = button.isConnectedOnRight();
	auto flatOnTop = button.isConnectedOnTop();
	auto flatOnBottom = button.isConnectedOnBottom();

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

		g.setColour(button.findColour(juce::ComboBox::outlineColourId));
		g.strokePath(path, juce::PathStrokeType(1.0f));
	}
	else {
		g.fillRoundedRectangle(bounds, cornerSize);

		g.setColour(button.findColour(juce::ComboBox::outlineColourId));
		g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
	}
}

void SourceViewLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
	bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
	juce::Font font(getTextButtonFont(button, button.getHeight()));
	g.setFont(font);
	g.setColour(button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
		: juce::TextButton::textColourOffId)
		.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

	const int yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
	const int cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;

	const int fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
	const int leftIndent = 0;//juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
	const int rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
	const int textWidth = button.getWidth() - leftIndent - rightIndent;

	if (textWidth > 0) {
		g.drawFittedText(button.getButtonText(),
			leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
			juce::Justification::centredLeft, 2);
	}
}

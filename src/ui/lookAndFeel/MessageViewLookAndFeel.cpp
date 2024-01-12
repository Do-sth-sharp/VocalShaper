#include "MessageViewLookAndFeel.h"
#include "../misc/ColorMap.h"
#include "../Utils.h"

MessageViewLookAndFeel::MessageViewLookAndFeel()
	: LookAndFeel_V4() {
	/** Background */
	this->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));

	/** Button */
	this->setColour(juce::TextButton::ColourIds::buttonColourId,
		ColorMap::getInstance()->get("ThemeColorA1"));
	this->setColour(juce::TextButton::ColourIds::buttonOnColourId,
		ColorMap::getInstance()->get("ThemeColorA1"));
	this->setColour(juce::TextButton::ColourIds::textColourOffId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::TextButton::ColourIds::textColourOnId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::ComboBox::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));

	/** List */
	this->setColour(juce::TextEditor::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));/**< Background */
	this->setColour(juce::TextEditor::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));/**< Text */
	this->setColour(juce::TextEditor::ColourIds::highlightColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	this->setColour(juce::TextEditor::ColourIds::highlightedTextColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::TextEditor::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB6"));/**< Split Line */
	this->setColour(juce::TextEditor::ColourIds::focusedOutlineColourId,
		ColorMap::getInstance()->get("ThemeColorA1"));
	this->setColour(juce::TextEditor::ColourIds::shadowColourId,
		juce::Colour::fromRGBA(0, 0, 0, 0));
	this->setColour(juce::TextEditor::ColourIds::shadowColourId + 1,
		ColorMap::getInstance()->get("ThemeColorB9"));/**< Time */

	/** Scroll Bar */
	this->setColour(juce::ScrollBar::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::ScrollBar::ColourIds::thumbColourId,
		ColorMap::getInstance()->get("ThemeColorA1"));
	this->setColour(juce::ScrollBar::ColourIds::trackColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
}

void MessageViewLookAndFeel::drawCallOutBoxBackground(
	juce::CallOutBox& box, juce::Graphics& g, const juce::Path& path, juce::Image& cachedImage) {
	juce::Colour colorBackground = this->findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	if (cachedImage.isNull()) {
		cachedImage = { juce::Image::ARGB, box.getWidth(), box.getHeight(), true };
		juce::Graphics g2(cachedImage);

		juce::DropShadow(juce::Colours::black.withAlpha(0.7f), 8, { 0, 2 }).drawForPath(g2, path);
	}

	g.setColour(juce::Colours::black);
	g.drawImageAt(cachedImage, 0, 0);

	g.setColour(colorBackground);
	g.fillPath(path);
};

int MessageViewLookAndFeel::getCallOutBoxBorderSize(const juce::CallOutBox& box) {
	auto screenSize = utils::getScreenSize(&box);
	return screenSize.getHeight() * 0.005;
};

float MessageViewLookAndFeel::getCallOutBoxCornerSize(const juce::CallOutBox& box) {
	auto screenSize = utils::getScreenSize(&box);
	return screenSize.getHeight() * 0.01;
};

#include "MessageViewLookAndFeel.h"
#include "../../misc/ColorMap.h"

MessageViewLookAndFeel::MessageViewLookAndFeel()
	: MainLookAndFeel() {
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

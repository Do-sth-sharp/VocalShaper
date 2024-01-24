#include "InstrViewLookAndFeel.h"
#include "../misc/ColorMap.h"
#include "../Utils.h"

InstrViewLookAndFeel::InstrViewLookAndFeel()
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

	/** Text */
	this->setColour(juce::Label::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));/** Item Background */
	this->setColour(juce::Label::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));/** Item Name */
	this->setColour(juce::Label::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB4"));
	this->setColour(juce::Label::ColourIds::backgroundWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));/** Item Background Opened */
	this->setColour(juce::Label::ColourIds::textWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));/** Item Name Opened */
	this->setColour(juce::Label::ColourIds::outlineWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));/** Drop Line */

	/** Button */
	this->setColour(juce::TextButton::ColourIds::buttonColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextButton::ColourIds::buttonOnColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::TextButton::ColourIds::textColourOffId,
		ColorMap::getInstance()->get("ThemeColorB7"));
	this->setColour(juce::TextButton::ColourIds::textColourOnId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::ComboBox::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
}

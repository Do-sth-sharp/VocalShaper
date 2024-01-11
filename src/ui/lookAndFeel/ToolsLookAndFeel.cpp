#include "ToolsLookAndFeel.h"
#include "../misc/ColorMap.h"

ToolsLookAndFeel::ToolsLookAndFeel()
	: LookAndFeel_V4() {
	/** Buttons */
	this->setColour(juce::TextButton::ColourIds::buttonColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextButton::ColourIds::buttonOnColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::TextButton::ColourIds::textColourOffId,
		ColorMap::getInstance()->get("ThemeColorB8"));
	this->setColour(juce::TextButton::ColourIds::textColourOnId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::ComboBox::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
}
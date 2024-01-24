#include "PluginEditorLookAndFeel.h"
#include "../misc/ColorMap.h"
#include "../Utils.h"

PluginEditorLookAndFeel::PluginEditorLookAndFeel()
	: MainLookAndFeel() {
	/** Background */
	this->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));

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

	/** Scroll Bar */
	this->setColour(juce::ScrollBar::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::ScrollBar::ColourIds::thumbColourId,
		ColorMap::getInstance()->get("ThemeColorA1"));
	this->setColour(juce::ScrollBar::ColourIds::trackColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
}

#include "MixerLookAndFeel.h"
#include "../../misc/ColorMap.h"

MixerLookAndFeel::MixerLookAndFeel()
	: MainLookAndFeel() {
	/** Background */
	this->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));

	/** Track */
	this->setColour(juce::Label::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));/**< Track Background */
	this->setColour(juce::Label::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));/**< Item Name Light */
	this->setColour(juce::Label::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));/**< Track Outline */
	this->setColour(juce::Label::ColourIds::backgroundWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::Label::ColourIds::textWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB0"));/**< Item Name Dark */
	this->setColour(juce::Label::ColourIds::outlineWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));/**< Track Hovered Outline */

	/** Effect List */
	this->setColour(juce::ListBox::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::ListBox::ColourIds::backgroundColourId + 1,
		ColorMap::getInstance()->get("ThemeColorB2"));/**< Background On */
	this->setColour(juce::ListBox::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::ListBox::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	this->setColour(juce::ListBox::ColourIds::textColourId + 1,
		ColorMap::getInstance()->get("ThemeColorB10"));/**< Text On */
}

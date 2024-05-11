#include "ChannelLinkLookAndFeel.h"
#include "../../misc/ColorMap.h"

ChannelLinkLookAndFeel::ChannelLinkLookAndFeel()
	: MainLookAndFeel() {
	/** Background */
	this->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));

	/** Graph */
	this->setColour(juce::Label::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::Label::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));/**< Channel Name */
	this->setColour(juce::Label::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));/**< Table Border */
	this->setColour(juce::Label::ColourIds::backgroundWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));/**< Hovered */
	this->setColour(juce::Label::ColourIds::textWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));/**< Title */
	this->setColour(juce::Label::ColourIds::outlineWhenEditingColourId,
		juce::Colours::lightgreen);/**< Link */
}
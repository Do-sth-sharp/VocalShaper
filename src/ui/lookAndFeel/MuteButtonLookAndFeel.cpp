#include "MuteButtonLookAndFeel.h"
#include "../misc/ColorMap.h"

MuteButtonLookAndFeel::MuteButtonLookAndFeel()
	: MainLookAndFeel() {
	/** Button */
	this->setColour(juce::TextButton::ColourIds::buttonColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextButton::ColourIds::buttonOnColourId,
		juce::Colours::yellow.withAlpha(0.6f));
	this->setColour(juce::TextButton::ColourIds::textColourOffId,
		ColorMap::getInstance()->get("ThemeColorB8"));
	this->setColour(juce::TextButton::ColourIds::textColourOnId,
		ColorMap::getInstance()->get("ThemeColorB10"));
}

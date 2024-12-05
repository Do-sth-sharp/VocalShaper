#include "SoloButtonLookAndFeel.h"
#include "../../misc/ColorMap.h"

SoloButtonLookAndFeel::SoloButtonLookAndFeel()
	: MainLookAndFeel() {
	/** Button */
	this->setColour(juce::TextButton::ColourIds::buttonColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextButton::ColourIds::buttonOnColourId,
		juce::Colours::red.withAlpha(0.6f));
	this->setColour(juce::TextButton::ColourIds::textColourOffId,
		ColorMap::getInstance()->get("ThemeColorB8"));
	this->setColour(juce::TextButton::ColourIds::textColourOnId,
		ColorMap::getInstance()->get("ThemeColorB10"));
}

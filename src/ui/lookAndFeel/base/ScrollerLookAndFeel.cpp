#include "ScrollerLookAndFeel.h"
#include "../../misc/ColorMap.h"

ScrollerLookAndFeel::ScrollerLookAndFeel()
	: MainLookAndFeel() {
	/** Background */
	this->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));

	/** Scroller */
	this->setColour(juce::ScrollBar::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::ScrollBar::ColourIds::thumbColourId,
		ColorMap::getInstance()->get("ThemeColorB4").withAlpha(0.7f));
	this->setColour(juce::ScrollBar::ColourIds::trackColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));/** Outline */

	/** Play Pos */
	this->setColour(juce::Label::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));
}

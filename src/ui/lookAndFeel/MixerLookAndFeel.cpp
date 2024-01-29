#include "MixerLookAndFeel.h"
#include "../misc/ColorMap.h"

MixerLookAndFeel::MixerLookAndFeel()
	: MainLookAndFeel() {
	/** Background */
	this->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
}

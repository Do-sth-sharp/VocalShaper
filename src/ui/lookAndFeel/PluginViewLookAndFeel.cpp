#include "PluginViewLookAndFeel.h"
#include "../misc/ColorMap.h"

PluginViewLookAndFeel::PluginViewLookAndFeel()
	: MainLookAndFeel() {
	/** Background */
	this->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
}

#include "ToolBarLookAndFeel.h"
#include "../misc/ColorMap.h"

ToolBarLookAndFeel::ToolBarLookAndFeel()
	: LookAndFeel_V4() {
	/** Background */
	this->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
}

#include "ToolBarLookAndFeel.h"
#include "../../misc/ColorMap.h"

ToolBarLookAndFeel::ToolBarLookAndFeel()
	: MainLookAndFeel() {
	/** Background */
	this->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
}

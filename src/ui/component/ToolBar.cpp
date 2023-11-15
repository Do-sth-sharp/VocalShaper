#include "ToolBar.h"
#include "../lookAndFeel/LookAndFeelFactory.h"

ToolBar::ToolBar()
	: FlowComponent(TRANS("ToolBar")) {
	this->setLookAndFeel(LookAndFeelFactory::getInstance()->forToolBar());
}

void ToolBar::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** BackGround */
	g.setColour(laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId));
	g.fillAll();
}

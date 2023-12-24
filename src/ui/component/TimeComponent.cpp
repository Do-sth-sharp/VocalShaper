#include "TimeComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"

TimeComponent::TimeComponent() {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSysStatus());
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void TimeComponent::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** Color */
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** BackGround */
	g.setColour(backgroundColor);
	g.fillAll();
}

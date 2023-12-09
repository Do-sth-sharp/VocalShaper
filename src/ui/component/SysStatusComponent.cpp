#include "SysStatusComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"

SysStatusComponent::SysStatusComponent()
	: AnimatedAppComponent() {
	/** Animate */
	this->setFramesPerSecond(2);

	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSysStatus());
}

void SysStatusComponent::update() {
	/** TODO */
}

void SysStatusComponent::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** BackGround */
	g.setColour(laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId));
	g.fillAll();
}

void SysStatusComponent::resized() {
	/** TODO */
}

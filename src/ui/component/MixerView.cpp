#include "MixerView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"

MixerView::MixerView()
	: FlowComponent(TRANS("Mixer")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forMixer());
}

void MixerView::paint(juce::Graphics& g) {
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();
}

#include "EffectComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"

EffectComponent::EffectComponent() {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forEffect());
}

void EffectComponent::paint(juce::Graphics& g) {
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();
}

void EffectComponent::update(int track, int index) {
	this->track = track;
	this->index = index;
	if (this->track > -1 && this->index > -1) {
		this->repaint();
	}
}

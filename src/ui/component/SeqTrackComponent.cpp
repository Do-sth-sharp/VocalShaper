#include "SeqTrackComponent.h"
#include "../Utils.h"

SeqTrackComponent::SeqTrackComponent() {
	/** TODO */
}

void SeqTrackComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		/** TODO */
	}
}

void SeqTrackComponent::updateBlock(int blockIndex) {
	/** TODO */
}

void SeqTrackComponent::updateHPos(double pos, double itemSize) {
	/** TODO */
}

void SeqTrackComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float lineThickness = screenSize.getHeight() * 0.0025;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour outlineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);

	/** Outline */
	juce::Rectangle<float> outlineRect(
		0, this->getHeight() - lineThickness,
		this->getWidth(), lineThickness);
	g.setColour(outlineColor);
	g.fillRect(outlineRect);
}

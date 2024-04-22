#include "SeqTrackComponent.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

SeqTrackComponent::SeqTrackComponent() {
	/** TODO */
}

void SeqTrackComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->trackColor = quickAPI::getSeqTrackColor(index);

		auto& laf = this->getLookAndFeel();
		if (utils::isLightColor(this->trackColor)) {
			this->idColor = laf.findColour(
				juce::Label::ColourIds::textWhenEditingColourId);
		}
		else {
			this->idColor = laf.findColour(
				juce::Label::ColourIds::textColourId);
		}

		this->repaint();
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
	float trackColorWidth = screenSize.getWidth() * 0.005;

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour outlineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);

	/** Track Color */
	juce::Rectangle<float> trackColorRect(
		0, 0, trackColorWidth, this->getHeight());
	g.setColour(this->trackColor);
	g.fillRect(trackColorRect);

	/** Outline */
	juce::Rectangle<float> outlineRect(
		0, this->getHeight() - lineThickness,
		this->getWidth(), lineThickness);
	g.setColour(outlineColor);
	g.fillRect(outlineRect);
}

void SeqTrackComponent::mouseMove(const juce::MouseEvent& event) {
	auto screenSize = utils::getScreenSize(this);
	float trackColorWidth = screenSize.getWidth() * 0.005;
	float x = event.position.getX();

	if (x >= 0 && x < trackColorWidth) {
		this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	}
	else {
		this->setMouseCursor(juce::MouseCursor::NormalCursor);
	}
}

void SeqTrackComponent::mouseUp(const juce::MouseEvent& event) {
	auto screenSize = utils::getScreenSize(this);
	float trackColorWidth = screenSize.getWidth() * 0.005;
	float x = event.position.getX();

	if (event.mods.isLeftButtonDown() || event.mods.isRightButtonDown()) {
		if (x >= 0 && x < trackColorWidth) {
			CoreActions::setSeqColorGUI(this->index);
		}
	}
}

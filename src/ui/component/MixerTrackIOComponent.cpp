#include "MixerTrackIOComponent.h"
#include "../Utils.h"

MixerTrackIOComponent::MixerTrackIOComponent(
	bool isInput, bool isMidi)
	: isInput(isInput), isMidi(isMidi) {
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void MixerTrackIOComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float pointHeight = screenSize.getHeight() * 0.0075;

	/** Color */
	juce::Colour colorOn = this->isMidi
		? juce::Colours::skyblue
		: juce::Colours::lightgreen;
	juce::Colour colorOff = this->isMidi
		? juce::Colours::indianred
		: juce::Colours::yellow;

	/** Center */
	auto centerPoint = this->getLocalBounds().getCentre();

	/** Draw Point */
	if (this->linked) {
		juce::Path path;
		path.startNewSubPath(centerPoint.getX(), centerPoint.getY() - pointHeight / 2);
		path.lineTo(centerPoint.getX() + pointHeight / 2, centerPoint.getY());
		path.lineTo(centerPoint.getX(), centerPoint.getY() + pointHeight / 2);
		path.lineTo(centerPoint.getX() - pointHeight / 2, centerPoint.getY());
		path.closeSubPath();

		g.setColour(colorOn);
		g.fillPath(path);
	}
	else {
		juce::Rectangle<float> pointRect(
			centerPoint.getX() - pointHeight / 2, centerPoint.getY() - pointHeight / 2,
			pointHeight, pointHeight);

		g.setColour(colorOff);
		g.fillEllipse(pointRect);
	}
}

void MixerTrackIOComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->repaint();
	}
}

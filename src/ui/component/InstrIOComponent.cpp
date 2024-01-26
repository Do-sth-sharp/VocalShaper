#include "InstrIOComponent.h"
#include "../Utils.h"

InstrIOComponent::InstrIOComponent(bool isInput)
	: isInput(isInput) {
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void InstrIOComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float pointHeight = screenSize.getHeight() * 0.01;

	/** Color */
	juce::Colour colorOn = this->isInput
		? juce::Colours::blue
		:juce::Colours::lightgreen;
	juce::Colour colorOff = this->isInput
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

void InstrIOComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		/** TODO */
		this->repaint();
	}
}

#include "SourceDstComponent.h"
#include "../misc/DragSourceType.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

SourceDstComponent::SourceDstComponent() {
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void SourceDstComponent::update(int index, int dst) {
	this->index = index;
	this->dst = dst;

	int dstID = quickAPI::getSourceId(dst);
	this->setTooltip((dst >= 0) ? ("#" + juce::String{ dstID }) : "-");

	this->repaint();
}

void SourceDstComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float pointHeight = screenSize.getHeight() * 0.01;

	/** Color */
	juce::Colour colorOn = juce::Colours::lightgreen;
	juce::Colour colorOff = juce::Colours::yellow;

	/** Center */
	auto centerPoint = this->getLocalBounds().getCentre();

	/** Draw Point */
	if (this->dst >= 0) {
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

void SourceDstComponent::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		if (!event.mouseWasDraggedSinceMouseDown()) {
			auto callback = [index = this->index](int dst) {
				CoreActions::setSourceSynthDst(index, dst);
				};

			auto menu = this->createDstSourceMenu(callback);
			menu.showAt(this);
		}
	}
	else if (event.mods.isRightButtonDown()) {
		CoreActions::setSourceSynthDst(index, -1);
	}
}

void SourceDstComponent::mouseDrag(const juce::MouseEvent& event) {
	/** Synth Link */
	if (event.mods.isLeftButtonDown()) {
		if (auto dragSource = juce::DragAndDropContainer::findParentDragContainerFor(this)) {
			dragSource->startDragging(this->getDragSourceDescription(),
				this, juce::ScaledImage{}, true);
		}
	}
}

juce::var SourceDstComponent::getDragSourceDescription() const {
	auto object = std::make_unique<juce::DynamicObject>();

	object->setProperty("type", (int)DragSourceType::SourceSynth);
	object->setProperty("src", this->index);

	return juce::var{ object.release() };
}

juce::PopupMenu SourceDstComponent::createDstSourceMenu(
	const std::function<void(int)>& callback) const {
	auto sourceList = quickAPI::getSourceNamesWithID();

	juce::PopupMenu menu;

	for (int i = 0; i < sourceList.size(); i++) {
		auto& name = sourceList[i];
		menu.addItem(name, i != this->index, i == this->dst,
			std::bind(callback, i));
	}

	return menu;
}

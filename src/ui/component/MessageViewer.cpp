#include "MessageViewer.h"
#include "MessageList.h"
#include "../dataModel/MessageModel.h"
#include "../Utils.h"
#include <IconManager.h>

MessageViewer::MessageViewer()
	: Component() {
	/** Clear Icon */
	this->clearIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("System", "delete-bin-line").getFullPathName());
	this->clearIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::Label::ColourIds::textColourId));

	/** Clear Button */
	this->clearButton = std::make_unique<juce::DrawableButton>(
		TRANS("Clear Notifications"), juce::DrawableButton::ImageOnButtonBackground);
	this->clearButton->setImages(this->clearIcon.get());
	this->clearButton->setWantsKeyboardFocus(false);
	this->clearButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->clearButton->onClick = [this] { this->clear(); };
	this->addAndMakeVisible(this->clearButton.get());

	/** Viewport */
	this->messageViewPort = std::make_unique<juce::Viewport>(TRANS("Notifications"));
	this->messageViewPort->setViewedComponent(new MessageList, true);
	this->messageViewPort->setScrollBarsShown(true, false);
	this->messageViewPort->setScrollOnDragMode(
		juce::Viewport::ScrollOnDragMode::nonHover);
	this->addAndMakeVisible(this->messageViewPort.get());

	/** Add Listener */
	MessageModel::getInstance()->addChangeListener(this);

	/** Update Message List */
	this->update();
}

MessageViewer::~MessageViewer() {
	MessageModel::getInstance()->removeChangeListener(this);
}

void MessageViewer::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int headerHeight = screenSize.getHeight() * 0.05;
	int buttonHeight = headerHeight * 0.75;
	int paddingWidth = screenSize.getWidth() * 0.0075;

	/** Button */
	juce::Rectangle<int> buttonRect(
		paddingWidth, headerHeight / 2 - buttonHeight / 2,
		buttonHeight, buttonHeight);
	this->clearButton->setBounds(buttonRect);

	/** Viewport */
	juce::Rectangle<int> viewportRect =
		this->getLocalBounds().withTrimmedTop(headerHeight);
	this->messageViewPort->setBounds(viewportRect);

	/** Content Size */
	if (auto viewer = dynamic_cast<MessageList*>(this->messageViewPort->getViewedComponent())) {
		int contentWidth = this->messageViewPort->getMaximumVisibleWidth();
		int contentHeight = viewer->getHeightPrefered();
		viewer->setBounds(0, 0,
			contentWidth, contentHeight);/**< To Get Width Without Scroller */

		contentWidth = this->messageViewPort->getMaximumVisibleWidth();
		contentHeight = viewer->getHeightPrefered();
		viewer->setBounds(0, 0,
			contentWidth, std::max(contentHeight, this->messageViewPort->getHeight()));
	}

	/** Update Content */
	auto currentBounds = this->getLocalBounds();
	if (currentBounds != this->boundsTemp) {
		this->boundsTemp = currentBounds;
		this->update();
	}
}

void MessageViewer::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** Color */
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** BackGround */
	g.setColour(backgroundColor);
	g.fillAll();
}

void MessageViewer::update() {
	/** Update Message List */
	if (auto viewer = dynamic_cast<MessageList*>(this->messageViewPort->getViewedComponent())) {
		viewer->update(MessageModel::getInstance()->getList());
		this->resized();
	}
}

void MessageViewer::clear() {
	MessageModel::getInstance()->clear();
}

void MessageViewer::changeListenerCallback(juce::ChangeBroadcaster*) {
	this->update();
}

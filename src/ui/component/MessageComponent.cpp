#include "MessageComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../dataModel/MessageModel.h"
#include "../Utils.h"
#include <IconManager.h>

MessageComponent::MessageComponent() {
	/** Mouse Cursor */
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);

	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forMessage());

	/** Icon */
	this->mesIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Media", "notification-3-line").getFullPathName());
	this->mesIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::Label::ColourIds::textColourId));

	/** Message */
	this->emptyMes = TRANS("No notification");

	/** Listen Messages */
	MessageModel::getInstance()->addChangeListener(this);
}

MessageComponent::~MessageComponent() {
	MessageModel::getInstance()->removeChangeListener(this);
}

void MessageComponent::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** Size */
	int paddingWidth = this->getHeight() * 0.2;
	int paddingHeight = this->getHeight() * 0.15;
	int splitWidth = this->getHeight() * 0.2;

	int iconHeight = this->getHeight() * 0.4;
	float noticeHeight = this->getHeight() * 0.15;
	int mesHeight = this->getHeight() - paddingHeight * 2;
	int mesLines = 2;
	float textHeight = mesHeight / (float)mesLines;

	/** Color */
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour textColor = laf.findColour(
		juce::Label::ColourIds::textColourId);
	juce::Colour noticeColor = juce::Colours::red;

	/** Font */
	juce::Font textFont(textHeight);

	/** BackGround */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Icon */
	g.setColour(textColor);
	juce::Rectangle<float> iconRect(
		paddingWidth, this->getHeight() / 2 - iconHeight / 2, iconHeight, iconHeight);
	this->mesIcon->drawWithin(g, iconRect,
		juce::RectanglePlacement::centred | juce::RectanglePlacement::fillDestination, 1.f);

	/** Notice */
	if (this->showNoticeDot) {
		g.setColour(noticeColor);
		juce::Point<float> centerPoint = iconRect.getTopRight();
		juce::Rectangle<float> noticeRect(
			centerPoint.getX() - noticeHeight / 2, centerPoint.getY() - noticeHeight / 2,
			noticeHeight, noticeHeight);
		g.fillEllipse(noticeRect);
	}

	/** Message */
	g.setColour(textColor);
	g.setFont(textFont);
	juce::Rectangle<int> mesRect(
		iconRect.getRight() + splitWidth, paddingHeight,
		this->getWidth() - (iconRect.getRight() + splitWidth) - paddingWidth, mesHeight);
	g.drawFittedText(this->mes.isEmpty() ? this->emptyMes : this->mes, mesRect,
		juce::Justification::topLeft, mesLines, 1.f);

	/** Padding */
	g.setColour(backgroundColor);
	g.fillRect(this->getLocalBounds().withWidth(paddingWidth));
	g.fillRect(this->getLocalBounds().withHeight(paddingHeight));
	g.fillRect(this->getLocalBounds().withTrimmedLeft(this->getWidth() - paddingWidth));
	g.fillRect(this->getLocalBounds().withTrimmedTop(this->getHeight() - paddingHeight));
}

void MessageComponent::changeListenerCallback(juce::ChangeBroadcaster*) {
	if (MessageModel::getInstance()->isEmpty()) {
		this->mes.clear();
		this->showNoticeDot = false;
	}
	else {
		auto [time, mes, callback] = MessageModel::getInstance()->getList().getLast();
		this->mes = time.formatted("[%H:%M:%S]") + " " + mes;
		this->showNoticeDot = true;
	}
	
	this->repaint();
}

void MessageComponent::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		/** TODO Show Message Box */

		/** Clear Notice */
		this->showNoticeDot = false;
		this->repaint();
	}
}

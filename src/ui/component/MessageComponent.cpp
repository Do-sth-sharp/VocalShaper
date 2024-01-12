#include "MessageComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
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
		this->getWidth() - (iconRect.getRight() + splitWidth), mesHeight);
	g.drawFittedText(this->mes.isEmpty() ? this->emptyMes : this->mes, mesRect,
		juce::Justification::centredLeft, mesLines, 1.f);
}

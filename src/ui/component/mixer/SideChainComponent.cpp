#include "SideChainComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"
#include <IconManager.h>

SideChainComponent::SideChainComponent() {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSideChain());

	/** Icons */
	this->addIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("System", "add-line").getFullPathName());
	this->addIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));

	this->subIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("System", "subtract-line").getFullPathName());
	this->subIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));

	/** Buttons */
	this->addButton = std::make_unique<juce::DrawableButton>(
		TRANS("Add Side Chain Bus"), juce::DrawableButton::ImageOnButtonBackground);
	this->addButton->setImages(this->addIcon.get());
	this->addButton->setWantsKeyboardFocus(false);
	this->addButton->setConnectedEdges(
		juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
	this->addButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->addButton->onClick = [this] { this->add(); };
	this->addAndMakeVisible(this->addButton.get());

	this->subButton = std::make_unique<juce::DrawableButton>(
		TRANS("Remove Side Chain Bus"), juce::DrawableButton::ImageOnButtonBackground);
	this->subButton->setImages(this->subIcon.get());
	this->subButton->setWantsKeyboardFocus(false);
	this->subButton->setConnectedEdges(
		juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
	this->subButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->subButton->onClick = [this] { this->sub(); };
	this->addAndMakeVisible(this->subButton.get());
}

void SideChainComponent::resized() {
	/** Size */
	int buttonHeight = this->getHeight();
	int buttonWidth = buttonHeight;

	/** Button */
	juce::Rectangle<int> addRect(
		this->getWidth() - buttonWidth, 0,
		buttonWidth, buttonHeight);
	this->addButton->setBounds(addRect);

	juce::Rectangle<int> subRect(
		addRect.getX() - buttonWidth, 0,
		buttonWidth, buttonHeight);
	this->subButton->setBounds(subRect);
}

void SideChainComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int buttonHeight = this->getHeight();
	int buttonWidth = buttonHeight;

	int numPaddingWidth = screenSize.getWidth() * 0.005;
	float numFontHeight = screenSize.getHeight() * 0.0125;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour textColor = laf.findColour(
		juce::Label::ColourIds::textColourId);

	/** Font */
	juce::Font numFont(juce::FontOptions{ numFontHeight });

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Num */
	juce::Rectangle<int> numRect(
		numPaddingWidth, 0,
		this->getWidth() - buttonWidth * 2 - numPaddingWidth * 2,
		this->getHeight());
	g.setFont(numFont);
	g.setColour(textColor);
	g.drawFittedText(juce::String{ this->sideChainNum }, numRect,
		juce::Justification::centredLeft, 1, 0.f);
}

void SideChainComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->sideChainNum = quickAPI::getMixerTrackSideChainBusNum(index);

		this->subButton->setEnabled(this->sideChainNum > 0);

		this->repaint();
	}
}

void SideChainComponent::add() {
	CoreActions::addTrackSideChain(this->index);
}

void SideChainComponent::sub() {
	CoreActions::removeTrackSideChain(this->index);
}

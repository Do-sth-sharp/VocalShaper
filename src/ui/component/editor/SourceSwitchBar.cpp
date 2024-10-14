#include "SourceSwitchBar.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../Utils.h"
#include <IconManager.h>

SourceSwitchBar::SourceSwitchBar(
	const StateChangedCallback& stateCallback)
	: stateCallback(stateCallback) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forEditorSwitchBar());

	/** Icons */
	this->switchIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Arrows", "arrow-down-s-fill").getFullPathName());
	this->switchIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));

	/** Buttons */
	this->switchButton = std::make_unique<juce::DrawableButton>(
		TRANS("Switch Editor Source"), juce::DrawableButton::ImageOnButtonBackground);
	this->switchButton->setImages(this->switchIcon.get());
	this->switchButton->setWantsKeyboardFocus(false);
	this->switchButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->switchButton->setConnectedEdges(juce::Button::ConnectedOnRight);
	this->switchButton->onClick = [this] { this->showSwitchMenu(); };
	this->addAndMakeVisible(this->switchButton.get());

	this->nameButton = std::make_unique<juce::TextButton>(
		TRANS("Switch Editor Source"), TRANS("Switch Editor Source"));
	this->nameButton->setWantsKeyboardFocus(false);
	this->nameButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->nameButton->setConnectedEdges(juce::Button::ConnectedOnLeft);
	this->nameButton->onClick = [this] { this->showSwitchMenu(); };
	this->addAndMakeVisible(this->nameButton.get());
}

void SourceSwitchBar::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int buttonPaddingHeight = screenSize.getHeight() * 0.003;
	int buttonPaddingWidth = screenSize.getWidth() * 0.003;
	int buttonSplitWidth = screenSize.getWidth() * 0.003;

	int buttonHeight = this->getHeight() - buttonPaddingHeight * 2;
	int nameButtonWidth = screenSize.getWidth() * 0.03;

	/** Switch Button */
	juce::Rectangle<int> switchRect(
		buttonPaddingWidth, buttonPaddingHeight,
		buttonHeight, buttonHeight);
	this->switchButton->setBounds(switchRect);

	/** Name Button */
	juce::Rectangle<int> nameRect(
		switchRect.getRight(), buttonPaddingHeight,
		nameButtonWidth, buttonHeight);
	this->nameButton->setBounds(nameRect);

	/** Fit Name Button Width */
	this->nameButton->changeWidthToFitText();
	int rightLimit = this->getWidth() - buttonPaddingWidth;
	if (this->nameButton->getWidth() < nameButtonWidth) {
		nameRect.setWidth(nameButtonWidth);
		this->nameButton->setBounds(nameRect);
	}
	if (this->nameButton->getRight() > rightLimit) {
		nameRect.setRight(rightLimit);
		this->nameButton->setBounds(nameRect);
	}
}

void SourceSwitchBar::paint(juce::Graphics& g) {
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** Background */
	g.fillAll(backgroundColor);
}

void SourceSwitchBar::showSwitchMenu() {
	/** TODO */
}

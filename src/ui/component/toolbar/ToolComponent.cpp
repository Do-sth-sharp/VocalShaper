#include "ToolComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../menuAndCommand/CommandManager.h"
#include "../../menuAndCommand/CommandTypes.h"
#include "../../Utils.h"
#include <IconManager.h>

ToolComponent::ToolComponent() {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forTools());

	/** Icons */
	auto arrowIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Development", "cursor-line").getFullPathName());
	arrowIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));
	this->icons.add(std::move(arrowIcon));

	auto arrowIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("Development", "cursor-line").getFullPathName());
	arrowIconOn->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
	this->icons.add(std::move(arrowIconOn));

	auto pencilIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Design", "pencil-line").getFullPathName());
	pencilIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));
	this->icons.add(std::move(pencilIcon));

	auto pencilIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("Design", "pencil-line").getFullPathName());
	pencilIconOn->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
	this->icons.add(std::move(pencilIconOn));

	/** Buttons */
	auto arrowButton = std::make_unique<juce::DrawableButton>(
		TRANS("Arrow"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackground);
	arrowButton->setImages(
		this->icons[0], nullptr, nullptr, nullptr,
		this->icons[1], nullptr, nullptr, nullptr);
	arrowButton->setWantsKeyboardFocus(false);
	arrowButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	arrowButton->setCommandToTrigger(
		CommandManager::getInstance(), (juce::CommandID)GUICommandType::Arrow, true);
	this->addAndMakeVisible(arrowButton.get());
	this->buttons.add(std::move(arrowButton));

	auto pencilButton = std::make_unique<juce::DrawableButton>(
		TRANS("Pencil"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackground);
	pencilButton->setImages(
		this->icons[2], nullptr, nullptr, nullptr,
		this->icons[3], nullptr, nullptr, nullptr);
	pencilButton->setWantsKeyboardFocus(false);
	pencilButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	pencilButton->setCommandToTrigger(
		CommandManager::getInstance(), (juce::CommandID)GUICommandType::Pencil, true);
	this->addAndMakeVisible(pencilButton.get());
	this->buttons.add(std::move(pencilButton));
}

void ToolComponent::resized() {
	auto screenSize = utils::getScreenSize(this);

	/** Size */
	int paddingHeight = this->getHeight() * 0.1;
	int paddingWidth = this->getHeight() * 0.1;
	int splitHeight = this->getHeight() * 0.05;
	int splitWidth = this->getHeight() * 0.05;

	int leftPos = paddingWidth;
	int buttonHeight = this->getHeight() * 0.9;

	/** Buttons */
	for (auto i : this->buttons) {
		i->setBounds(leftPos, this->getHeight() / 2 - buttonHeight / 2,
			buttonHeight, buttonHeight);
		leftPos += buttonHeight;
		leftPos += splitWidth;
	}
}

void ToolComponent::paint(juce::Graphics& g) {
	/** Nothing To Do */
}

#include "ToolComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../menuAndCommand/CommandManager.h"
#include "../menuAndCommand/CommandTypes.h"
#include "../Utils.h"
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

	auto handIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Editor", "hand").getFullPathName());
	handIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));
	this->icons.add(std::move(handIcon));

	auto handIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("Editor", "hand").getFullPathName());
	handIconOn->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
	this->icons.add(std::move(handIconOn));

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

	auto magicIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Design", "magic-line").getFullPathName());
	magicIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));
	this->icons.add(std::move(magicIcon));

	auto magicIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("Design", "magic-line").getFullPathName());
	magicIconOn->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
	this->icons.add(std::move(magicIconOn));

	auto scissorsIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Design", "scissors-2-line").getFullPathName());
	scissorsIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));
	this->icons.add(std::move(scissorsIcon));

	auto scissorsIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("Design", "scissors-2-line").getFullPathName());
	scissorsIconOn->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
	this->icons.add(std::move(scissorsIconOn));

	auto eraserIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Design", "eraser-line").getFullPathName());
	eraserIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));
	this->icons.add(std::move(eraserIcon));

	auto eraserIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("Design", "eraser-line").getFullPathName());
	eraserIconOn->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
	this->icons.add(std::move(eraserIconOn));

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

	auto handButton = std::make_unique<juce::DrawableButton>(
		TRANS("Hand"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackground);
	handButton->setImages(
		this->icons[2], nullptr, nullptr, nullptr,
		this->icons[3], nullptr, nullptr, nullptr);
	handButton->setWantsKeyboardFocus(false);
	handButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	handButton->setCommandToTrigger(
		CommandManager::getInstance(), (juce::CommandID)GUICommandType::Hand, true);
	this->addAndMakeVisible(handButton.get());
	this->buttons.add(std::move(handButton));

	auto pencilButton = std::make_unique<juce::DrawableButton>(
		TRANS("Pencil"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackground);
	pencilButton->setImages(
		this->icons[4], nullptr, nullptr, nullptr,
		this->icons[5], nullptr, nullptr, nullptr);
	pencilButton->setWantsKeyboardFocus(false);
	pencilButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	pencilButton->setCommandToTrigger(
		CommandManager::getInstance(), (juce::CommandID)GUICommandType::Pencil, true);
	this->addAndMakeVisible(pencilButton.get());
	this->buttons.add(std::move(pencilButton));

	auto magicButton = std::make_unique<juce::DrawableButton>(
		TRANS("Magic"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackground);
	magicButton->setImages(
		this->icons[6], nullptr, nullptr, nullptr,
		this->icons[7], nullptr, nullptr, nullptr);
	magicButton->setWantsKeyboardFocus(false);
	magicButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	magicButton->setCommandToTrigger(
		CommandManager::getInstance(), (juce::CommandID)GUICommandType::Magic, true);
	this->addAndMakeVisible(magicButton.get());
	this->buttons.add(std::move(magicButton));

	auto scissorsButton = std::make_unique<juce::DrawableButton>(
		TRANS("Scissors"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackground);
	scissorsButton->setImages(
		this->icons[8], nullptr, nullptr, nullptr,
		this->icons[9], nullptr, nullptr, nullptr);
	scissorsButton->setWantsKeyboardFocus(false);
	scissorsButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	scissorsButton->setCommandToTrigger(
		CommandManager::getInstance(), (juce::CommandID)GUICommandType::Scissors, true);
	this->addAndMakeVisible(scissorsButton.get());
	this->buttons.add(std::move(scissorsButton));

	auto eraserButton = std::make_unique<juce::DrawableButton>(
		TRANS("Eraser"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackground);
	eraserButton->setImages(
		this->icons[10], nullptr, nullptr, nullptr,
		this->icons[11], nullptr, nullptr, nullptr);
	eraserButton->setWantsKeyboardFocus(false);
	eraserButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	eraserButton->setCommandToTrigger(
		CommandManager::getInstance(), (juce::CommandID)GUICommandType::Eraser, true);
	this->addAndMakeVisible(eraserButton.get());
	this->buttons.add(std::move(eraserButton));
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

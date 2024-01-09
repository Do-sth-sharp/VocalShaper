#include "ControllerComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../menuAndCommand/CommandManager.h"
#include "../menuAndCommand/CommandTypes.h"
#include "../Utils.h"
#include <IconManager.h>

ControllerComponent::ControllerComponent() {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forController());

	/** Icons */
	this->playIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Media", "play-fill").getFullPathName());
	this->playIcon->replaceColour(juce::Colours::black, juce::Colours::green);
	this->pauseIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Media", "pause-fill").getFullPathName());
	this->pauseIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
	this->stopIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Media", "stop-fill").getFullPathName());
	this->stopIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));
	this->recordIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Design", "circle-fill").getFullPathName());
	this->recordIcon->replaceColour(juce::Colours::black, juce::Colours::darkred);
	this->recordOnIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Design", "circle-fill").getFullPathName());
	this->recordOnIcon->replaceColour(juce::Colours::black, juce::Colours::red);

	this->rewindIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Media", "skip-back-fill").getFullPathName());
	this->rewindIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));
	this->followIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Arrows", "arrow-right-fill").getFullPathName());
	this->followIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));
	this->followOnIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Arrows", "arrow-right-fill").getFullPathName());
	this->followOnIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));

	/** Buttons */
	this->playButton = std::make_unique<juce::DrawableButton>(
		TRANS("Play/Pause"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackgroundOriginalSize);
	this->playButton->setImages(
		this->playIcon.get(), nullptr, nullptr, nullptr,
		this->pauseIcon.get(), nullptr, nullptr, nullptr);
	this->playButton->setWantsKeyboardFocus(false);
	this->playButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->playButton->setCommandToTrigger(
		CommandManager::getInstance(), (juce::CommandID)CoreCommandType::Play, true);
	this->addAndMakeVisible(this->playButton.get());

	this->stopButton = std::make_unique<juce::DrawableButton>(
		TRANS("Stop"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackgroundOriginalSize);
	this->stopButton->setImages(this->stopIcon.get());
	this->stopButton->setWantsKeyboardFocus(false);
	this->stopButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->stopButton->setCommandToTrigger(
		CommandManager::getInstance(), (juce::CommandID)CoreCommandType::Stop, true);
	this->addAndMakeVisible(this->stopButton.get());

	this->recordButton = std::make_unique<juce::DrawableButton>(
		TRANS("Record"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackgroundOriginalSize);
	this->recordButton->setImages(
		this->recordIcon.get(), nullptr, nullptr, nullptr,
		this->recordOnIcon.get(), nullptr, nullptr, nullptr);
	this->recordButton->setWantsKeyboardFocus(false);
	this->recordButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->recordButton->setCommandToTrigger(
		CommandManager::getInstance(), (juce::CommandID)CoreCommandType::Record, true);
	this->addAndMakeVisible(this->recordButton.get());

	this->rewindButton = std::make_unique<juce::DrawableButton>(
		TRANS("Rewind"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackgroundOriginalSize);
	this->rewindButton->setImages(this->rewindIcon.get());
	this->rewindButton->setWantsKeyboardFocus(false);
	this->rewindButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->rewindButton->setCommandToTrigger(
		CommandManager::getInstance(), (juce::CommandID)CoreCommandType::Rewind, true);
	this->addAndMakeVisible(this->rewindButton.get());

	this->followButton = std::make_unique<juce::DrawableButton>(
		TRANS("Follow"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackgroundOriginalSize);
	this->followButton->setImages(
		this->followIcon.get(), nullptr, nullptr, nullptr,
		this->followOnIcon.get(), nullptr, nullptr, nullptr);
	this->followButton->setWantsKeyboardFocus(false);
	this->followButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->followButton->setCommandToTrigger(
		CommandManager::getInstance(), (juce::CommandID)GUICommandType::Follow, true);
	this->addAndMakeVisible(this->followButton.get());
}

void ControllerComponent::resized() {
	auto screenSize = utils::getScreenSize(this);

	/** Size */
	int paddingHeight = this->getHeight() * 0.1;
	int paddingWidth = this->getHeight() * 0.1;
	int splitHeight = this->getHeight() * 0.05;
	int splitWidth = this->getHeight() * 0.05;

	int rightPos = this->getWidth() - paddingWidth;
	int buttonHeight = this->getHeight() * 0.6;

	/** Record Button */
	this->recordButton->setBounds(
		rightPos - buttonHeight, this->getHeight() / 2 - buttonHeight / 2,
		buttonHeight, buttonHeight);
	rightPos -= buttonHeight;
	rightPos -= splitWidth;

	/** Stop Button */
	this->stopButton->setBounds(
		rightPos - buttonHeight, this->getHeight() / 2 - buttonHeight / 2,
		buttonHeight, buttonHeight);
	rightPos -= buttonHeight;
	rightPos -= splitWidth;

	/** Play Button */
	this->playButton->setBounds(
		rightPos - buttonHeight, this->getHeight() / 2 - buttonHeight / 2,
		buttonHeight, buttonHeight);
	rightPos -= buttonHeight;
	rightPos -= splitWidth;

	/** Rewind Button */
	this->rewindButton->setBounds(
		rightPos - buttonHeight, this->getHeight() / 2 - buttonHeight / 2,
		buttonHeight, buttonHeight);
	rightPos -= buttonHeight;
	rightPos -= splitWidth;

	/** Follow Button */
	this->followButton->setBounds(
		rightPos - buttonHeight, this->getHeight() / 2 - buttonHeight / 2,
		buttonHeight, buttonHeight);
	rightPos -= buttonHeight;
	rightPos -= splitWidth;
}

void ControllerComponent::paint(juce::Graphics& g) {
	/** Nothing To Do */
}

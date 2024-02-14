#include "EffectComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"
#include <IconManager.h>

EffectComponent::EffectComponent() {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forEffect());

	/** Bypass Icon */
		this->bypassIcon = flowUI::IconManager::getSVG(
			utils::getIconFile("Device", "shut-down-line").getFullPathName());
	this->bypassIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));

	this->bypassIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("Device", "shut-down-line").getFullPathName());
	this->bypassIconOn->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));

	/** Bypass Button */
	this->bypassButton = std::make_unique<juce::DrawableButton>(
		TRANS("Bypass Effect"), juce::DrawableButton::ImageOnButtonBackground);
	this->bypassButton->setImages(
		this->bypassIcon.get(), nullptr, nullptr, nullptr,
		this->bypassIconOn.get(), nullptr, nullptr, nullptr);
	this->bypassButton->setWantsKeyboardFocus(false);
	this->bypassButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->bypassButton->setConnectedEdges(
		juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
	this->bypassButton->onClick = [this] { this->bypass(); };
	this->addAndMakeVisible(this->bypassButton.get());
}

void EffectComponent::resized() {
	/** Size */
	int buttonHeight = this->getHeight();
	int buttonWidth = buttonHeight;

	int right = this->getWidth();

	/** Bypass Button */
	juce::Rectangle<int> bypassRect(
		right - buttonWidth, 0,
		buttonWidth, buttonHeight);
	this->bypassButton->setBounds(bypassRect);
	right -= buttonWidth;
}

void EffectComponent::paint(juce::Graphics& g) {
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();
}

void EffectComponent::update(int track, int index) {
	this->track = track;
	this->index = index;
	if (this->track > -1 && this->index > -1) {
		this->bypassButton->setToggleState(
			!quickAPI::getEffectBypass(track, index),
			juce::NotificationType::dontSendNotification);

		this->repaint();
	}
}

void EffectComponent::bypass() {
	/** TODO */
}

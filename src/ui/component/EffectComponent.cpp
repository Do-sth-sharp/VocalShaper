#include "EffectComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreActions.h"
#include "../misc/PluginEditorHub.h"
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
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int textPaddingWidth = screenSize.getWidth() * 0.0025;
	float textHeight = this->getHeight() * 0.8;

	int buttonHeight = this->getHeight();
	int buttonWidth = buttonHeight;

	int right = this->getWidth();
	right -= buttonWidth;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(this->editorOpened
		? juce::Label::ColourIds::backgroundWhenEditingColourId
		: juce::Label::ColourIds::backgroundColourId);
	juce::Colour textColor = laf.findColour(this->editorOpened
		? juce::Label::ColourIds::textWhenEditingColourId
		: juce::Label::ColourIds::textColourId);

	/** Font */
	juce::Font textFont(textHeight);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Text */
	juce::Rectangle<int> textRect(
		textPaddingWidth, 0,
		right - textPaddingWidth * 2, this->getHeight());
	g.setColour(textColor);
	g.setFont(textFont);
	g.drawFittedText(this->name, textRect,
		juce::Justification::centredLeft, 1, 0.75f);
}

void EffectComponent::update(int track, int index) {
	this->track = track;
	this->index = index;
	if (this->track > -1 && this->index > -1) {
		this->name = quickAPI::getEffectName(track, index);
		this->editorOpened = PluginEditorHub::getInstance()->checkEffect(track, index);

		this->bypassButton->setToggleState(
			!quickAPI::getEffectBypass(track, index),
			juce::NotificationType::dontSendNotification);

		this->repaint();

		this->setTooltip(this->createToolTip());
	}
}

void EffectComponent::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		this->editorShow();
	}
}

void EffectComponent::bypass() {
	CoreActions::bypassEffect(this->track, this->index,
		this->bypassButton->getToggleState());
}

void EffectComponent::editorShow() {
	if (this->editorOpened) {
		PluginEditorHub::getInstance()->closeEffect(this->track, this->index);
	}
	else {
		PluginEditorHub::getInstance()->openEffect(this->track, this->index);
	}
}

juce::String EffectComponent::createToolTip() const {
	juce::String result =
		"#" + juce::String{ this->index } + "\n"
		+ TRANS("Name:") + " " + this->name + "\n"
		+ TRANS("Bypassed:") + " " + TRANS(this->bypassButton->getToggleState() ? "No" : "Yes") + "\n";

	return result;
}

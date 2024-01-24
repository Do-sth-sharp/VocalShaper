#include "InstrComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreActions.h"
#include "../misc/PluginEditorHub.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"
#include <IconManager.h>

InstrComponent::InstrComponent() {
	this->setWantsKeyboardFocus(true);

	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forInstrView());

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
		TRANS("Bypass Instrument"), juce::DrawableButton::ImageOnButtonBackground);
	this->bypassButton->setImages(
		this->bypassIcon.get(), nullptr, nullptr, nullptr,
		this->bypassIconOn.get(), nullptr, nullptr, nullptr);
	this->bypassButton->setWantsKeyboardFocus(false);
	this->bypassButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->bypassButton->onClick = [this] { this->bypass(); };
	this->addAndMakeVisible(this->bypassButton.get());
}

void InstrComponent::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingHeight = screenSize.getHeight() * 0.005;
	int paddingWidth = screenSize.getWidth() * 0.005;
	int splitWidth = screenSize.getWidth() * 0.005;

	int buttonHeight = this->getHeight() - paddingHeight * 2;

	/** Bypass Button */
	juce::Rectangle<int> bypassRect(
		this->getWidth() - paddingWidth - buttonHeight,
		paddingHeight, buttonHeight, buttonHeight);
	this->bypassButton->setBounds(bypassRect);
}

void InstrComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingHeight = screenSize.getHeight() * 0.0025;
	int paddingWidth = screenSize.getWidth() * 0.01;
	int splitWidth = screenSize.getWidth() * 0.005;

	int buttonHeight = this->getHeight() - paddingHeight * 2;
	float textHeight = buttonHeight * 0.8;

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
		paddingWidth, paddingHeight,
		this->getWidth() - paddingWidth * 2 - buttonHeight - splitWidth,
		buttonHeight);
	g.setColour(textColor);
	g.setFont(textFont);
	g.drawFittedText(this->name, textRect,
		juce::Justification::centredLeft, 1, 1.f);
}

void InstrComponent::update(int index) {
	if (index >= 0 && index < quickAPI::getInstrNum()) {
		this->index = index;
		this->name = quickAPI::getInstrName(index);
		this->editorOpened = PluginEditorHub::getInstance()->checkInstr(index);

		this->bypassButton->setToggleState(!quickAPI::getInstrBypass(index),
			juce::NotificationType::dontSendNotification);

		this->repaint();

		this->setTooltip(this->createToolTip());
	}
}

void InstrComponent::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		this->editorShow();
	}
}

void InstrComponent::bypass() {
	CoreActions::bypassInstr(this->index,
		this->bypassButton->getToggleState());
}

void InstrComponent::editorShow() {
	if (this->editorOpened) {
		PluginEditorHub::getInstance()->closeInstr(this->index);
	}
	else {
		PluginEditorHub::getInstance()->openInstr(this->index);
	}
}

juce::String InstrComponent::createToolTip() const {
	juce::String result =
		"#" + juce::String{ this->index } + "\n"
		+ TRANS("Name:") + " " + this->name + "\n"
		+ TRANS("Bypassed:") + " " + TRANS(this->bypassButton->getToggleState() ? "No" : "Yes") + "\n";

	return result;
}

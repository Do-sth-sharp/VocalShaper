#include "PluginEditor.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include <IconManager.h>

PluginEditor::PluginEditor(const juce::String& name, PluginType type,
	quickAPI::PluginHolder plugin,
	juce::Component::SafePointer<juce::AudioProcessorEditor> editor)
	: FlowComponent(name), plugin(plugin), editor(editor), type(type) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forPluginEditor());

	/** Add Listener To Comp */
	if (editor) {
		editor->addComponentListener(this);
	}

	/** Editor Viewport */
	this->editorViewport = std::make_unique<juce::Viewport>(TRANS("Plugin Editor"));
	this->editorViewport->setViewedComponent(editor, false);
	this->editorViewport->setScrollOnDragMode(
		juce::Viewport::ScrollOnDragMode::nonHover);
	this->addAndMakeVisible(this->editorViewport.get());

	/** Config Viewport */
	this->configViewport = std::make_unique<juce::Viewport>(TRANS("Plugin Config"));
	this->configViewport->setScrollBarsShown(true, false);
	this->configViewport->setScrollOnDragMode(
		juce::Viewport::ScrollOnDragMode::nonHover);
	this->addChildComponent(this->configViewport.get());

	/** Icons */
	this->bypassIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Device", "shut-down-line").getFullPathName());
	this->bypassIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));

	this->bypassIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("Device", "shut-down-line").getFullPathName());
	this->bypassIconOn->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));

	this->configIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("System", "settings-line").getFullPathName());
	this->configIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));

	this->configIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("System", "settings-line").getFullPathName());
	this->configIconOn->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));

	/** Buttons */
	this->bypassButton = std::make_unique<juce::DrawableButton>(
		TRANS("Bypass Plugin"), juce::DrawableButton::ImageOnButtonBackground);
	this->bypassButton->setImages(
		this->bypassIcon.get(), nullptr, nullptr, nullptr,
		this->bypassIconOn.get(), nullptr, nullptr, nullptr);
	this->bypassButton->setWantsKeyboardFocus(false);
	this->bypassButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->bypassButton->onClick = [this] { this->bypass(); };
	this->addAndMakeVisible(this->bypassButton.get());

	this->configButton = std::make_unique<juce::DrawableButton>(
		TRANS("Config Plugin"), juce::DrawableButton::ImageOnButtonBackground);
	this->configButton->setImages(
		this->configIcon.get(), nullptr, nullptr, nullptr,
		this->configIconOn.get(), nullptr, nullptr, nullptr);
	this->configButton->setWantsKeyboardFocus(false);
	this->configButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->configButton->onClick = [this] { this->config(); };
	this->addAndMakeVisible(this->configButton.get());

	/** Update Now */
	this->update();
}

PluginEditor::~PluginEditor() {
	if (this->editor) {
		this->editor->removeComponentListener(this);
	}
}

void PluginEditor::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int toolBarHeight = screenSize.getHeight() * 0.015;
	int toolPaddingHeight = screenSize.getHeight() * 0.0025;
	int toolPaddingWidth = screenSize.getWidth() * 0.0025;
	int toolSplitWidth = screenSize.getWidth() * 0.0025;

	int buttonHeight = toolBarHeight - toolPaddingHeight * 2;

	/** Buttons */
	juce::Rectangle<int> bypassRect(
		toolPaddingWidth, toolPaddingHeight,
		buttonHeight, buttonHeight);
	this->bypassButton->setBounds(bypassRect);

	juce::Rectangle<int> configRect(
		bypassRect.getRight() + toolSplitWidth, toolPaddingHeight,
		buttonHeight, buttonHeight);
	this->configButton->setBounds(configRect);

	/** Content */
	juce::Rectangle<int> contentRect =
		this->getLocalBounds().withTrimmedTop(toolBarHeight);
	this->editorViewport->setBounds(contentRect);
	this->configViewport->setBounds(contentRect);
}

void PluginEditor::paint(juce::Graphics& g) {
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();
}

void PluginEditor::update() {
	if (this->editor) {
		this->bypassButton->setEnabled(true);
		
		switch (this->type) {
		case PluginType::Instr: {
			this->bypassButton->setToggleState(
				!quickAPI::getInstrBypass(this->plugin),
				juce::NotificationType::dontSendNotification);
			break;
		}
		case PluginType::Effect: {
			this->bypassButton->setToggleState(
				!quickAPI::getEffectBypass(this->plugin),
				juce::NotificationType::dontSendNotification);
			break;
		}
		}
	}
	else {
		this->bypassButton->setEnabled(false);
	}
}

void PluginEditor::componentBeingDeleted(juce::Component&) {
	this->editorViewport->setViewedComponent(nullptr, false);
	this->update();
}

void PluginEditor::bypass() {
	if (this->editor) {
		switch (this->type) {
		case PluginType::Instr: {
			CoreActions::bypassInstr(this->plugin,
				this->bypassButton->getToggleState());
			break;
		}
		case PluginType::Effect: {
			CoreActions::bypassEffect(this->plugin,
				this->bypassButton->getToggleState());
			break;
		}
		}
	}
}

void PluginEditor::config() {
	bool newState = !(this->configButton->getToggleState());
	this->editorViewport->setVisible(!newState);
	this->configViewport->setVisible(newState);
	this->configButton->setToggleState(newState,
		juce::NotificationType::dontSendNotification);
}

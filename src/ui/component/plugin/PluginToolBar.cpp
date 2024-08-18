#include "PluginToolBar.h"
#include "PluginEditor.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../Utils.h"
#include <IconManager.h>

PluginToolBar::PluginToolBar(PluginEditorContent* parent,
	quickAPI::PluginHolder plugin, PluginType type)
	: parent(parent), plugin(plugin), type(type) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forPluginEditor());

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

	this->pinIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Map", "pushpin-line").getFullPathName());
	this->pinIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));

	this->pinIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("Map", "pushpin-line").getFullPathName());
	this->pinIconOn->replaceColour(juce::Colours::black,
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

	this->pinButton = std::make_unique<juce::DrawableButton>(
		TRANS("Pin Plugin Window"), juce::DrawableButton::ImageOnButtonBackground);
	this->pinButton->setImages(
		this->pinIcon.get(), nullptr, nullptr, nullptr,
		this->pinIconOn.get(), nullptr, nullptr, nullptr);
	this->pinButton->setWantsKeyboardFocus(false);
	this->pinButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->pinButton->onClick = [this] { this->pin(); };
	this->addAndMakeVisible(this->pinButton.get());

	this->scaleButton = std::make_unique<juce::TextButton>(
		TRANS("Scale Plugin Window"), TRANS("Scale Plugin Window"));
	this->scaleButton->setWantsKeyboardFocus(false);
	this->scaleButton->setToggleState(true, juce::NotificationType::dontSendNotification);
	this->scaleButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->scaleButton->onClick = [this] { this->changeScale(); };
	this->addAndMakeVisible(this->scaleButton.get());

	/** Update Scale */
	this->setEditorScale(1.0f);
}

void PluginToolBar::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int toolPaddingHeight = screenSize.getHeight() * 0.003;
	int toolPaddingWidth = screenSize.getWidth() * 0.003;
	int toolSplitWidth = screenSize.getWidth() * 0.003;

	int buttonHeight = this->getHeight() - toolPaddingHeight * 2;
	int scaleButtonWidth = screenSize.getWidth() * 0.03;

	/** Buttons */
	juce::Rectangle<int> bypassRect(
		toolPaddingWidth, toolPaddingHeight,
		buttonHeight, buttonHeight);
	this->bypassButton->setBounds(bypassRect);

	juce::Rectangle<int> configRect(
		bypassRect.getRight() + toolSplitWidth, toolPaddingHeight,
		buttonHeight, buttonHeight);
	this->configButton->setBounds(configRect);

	juce::Rectangle<int> pinRect(
		configRect.getRight() + toolSplitWidth, toolPaddingHeight,
		buttonHeight, buttonHeight);
	this->pinButton->setBounds(pinRect);

	juce::Rectangle<int> scaleRect(
		this->getWidth() - toolPaddingWidth - scaleButtonWidth,
		toolPaddingHeight, scaleButtonWidth, buttonHeight);
	this->scaleButton->setBounds(scaleRect);
}

void PluginToolBar::update() {
	if (this->plugin) {
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

void PluginToolBar::bypass() {
	if (this->plugin) {
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

void PluginToolBar::config() {
	bool newState = !(this->configButton->getToggleState());
	this->parent->config(newState);
	this->configButton->setToggleState(newState,
		juce::NotificationType::dontSendNotification);
}

void PluginToolBar::pin() {
	bool newState = !(this->pinButton->getToggleState());
	this->parent->pin(newState);
	this->pinButton->setToggleState(newState,
		juce::NotificationType::dontSendNotification);
}

enum PluginEditorScaleType {
	Scale_0_25 = 1, Scale_0_5, Scale_0_75, Scale_1_0,
	Scale_1_25, Scale_1_5, Scale_1_75, Scale_2_0,
	Scale_3_0, Scale_4_0
};

void PluginToolBar::changeScale() {
	auto menu = this->createScaleMenu();
	int result = menu.showAt(this->scaleButton.get());

	switch (result) {
	case PluginEditorScaleType::Scale_0_25: {
		this->setEditorScale(0.25f);
		break;
	}
	case PluginEditorScaleType::Scale_0_5: {
		this->setEditorScale(0.5f);
		break;
	}
	case PluginEditorScaleType::Scale_0_75: {
		this->setEditorScale(0.75f);
		break;
	}
	case PluginEditorScaleType::Scale_1_0: {
		this->setEditorScale(1.0f);
		break;
	}
	case PluginEditorScaleType::Scale_1_25: {
		this->setEditorScale(1.25f);
		break;
	}
	case PluginEditorScaleType::Scale_1_5: {
		this->setEditorScale(1.5f);
		break;
	}
	case PluginEditorScaleType::Scale_1_75: {
		this->setEditorScale(1.75f);
		break;
	}
	case PluginEditorScaleType::Scale_2_0: {
		this->setEditorScale(2.0f);
		break;
	}
	case PluginEditorScaleType::Scale_3_0: {
		this->setEditorScale(3.0f);
		break;
	}
	case PluginEditorScaleType::Scale_4_0: {
		this->setEditorScale(4.0f);
		break;
	}
	}
}

void PluginToolBar::setEditorScale(float scale) {
	this->scale = scale;
	this->scaleButton->setButtonText(juce::String{ scale, 2 } + "x");
	this->parent->setEditorScale(scale);
}

float PluginToolBar::getEditorScale() const {
	return this->scale;
}

juce::PopupMenu PluginToolBar::createScaleMenu() {
	juce::PopupMenu menu;

	menu.addItem(PluginEditorScaleType::Scale_0_25, "0.25x",
		true, juce::approximatelyEqual(this->scale, 0.25f));
	menu.addItem(PluginEditorScaleType::Scale_0_5, "0.5x",
		true, juce::approximatelyEqual(this->scale, 0.5f));
	menu.addItem(PluginEditorScaleType::Scale_0_75, "0.75x",
		true, juce::approximatelyEqual(this->scale, 0.75f));
	menu.addItem(PluginEditorScaleType::Scale_1_0, "1.0x",
		true, juce::approximatelyEqual(this->scale, 1.0f));
	menu.addItem(PluginEditorScaleType::Scale_1_25, "1.25x",
		true, juce::approximatelyEqual(this->scale, 1.25f));
	menu.addItem(PluginEditorScaleType::Scale_1_5, "1.5x",
		true, juce::approximatelyEqual(this->scale, 1.5f));
	menu.addItem(PluginEditorScaleType::Scale_1_75, "1.75x",
		true, juce::approximatelyEqual(this->scale, 1.75f));
	menu.addItem(PluginEditorScaleType::Scale_2_0, "2.0x",
		true, juce::approximatelyEqual(this->scale, 2.0f));
	menu.addItem(PluginEditorScaleType::Scale_3_0, "3.0x",
		true, juce::approximatelyEqual(this->scale, 3.0f));
	menu.addItem(PluginEditorScaleType::Scale_4_0, "4.0x",
		true, juce::approximatelyEqual(this->scale, 4.0f));

	return menu;
}

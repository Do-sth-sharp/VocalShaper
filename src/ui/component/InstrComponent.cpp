#include "InstrComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreActions.h"
#include "../misc/PluginEditorHub.h"
#include "../misc/DragSourceType.h"
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

	/** IO Component */
	this->input = std::make_unique<InstrIOComponent>(true);
	this->addAndMakeVisible(this->input.get());
	this->output = std::make_unique<InstrIOComponent>(false);
	this->addAndMakeVisible(this->output.get());
}

void InstrComponent::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingHeight = screenSize.getHeight() * 0.005;
	int paddingWidth = screenSize.getWidth() * 0.005;
	int splitWidth = screenSize.getWidth() * 0.0025;

	int buttonHeight = this->getHeight() - paddingHeight * 2;
	int ioHideWidth = screenSize.getWidth() * 0.075;

	int left = paddingWidth, right = (this->getWidth() - paddingWidth);
	bool ioShown = this->getWidth() > ioHideWidth;

	/** Input Comp */
	if (ioShown) {
		juce::Rectangle<int> inputRect(
			left, paddingHeight,
			buttonHeight, buttonHeight);
		this->input->setBounds(inputRect);
		left += (buttonHeight + splitWidth);
	}
	this->input->setVisible(ioShown);

	/** Output Comp */
	if (ioShown) {
		juce::Rectangle<int> outputRect(
			right - buttonHeight,
			paddingHeight, buttonHeight, buttonHeight);
		this->output->setBounds(outputRect);
		right -= (buttonHeight + splitWidth);
	}
	this->output->setVisible(ioShown);

	/** Bypass Button */
	juce::Rectangle<int> bypassRect(
		right - buttonHeight,
		paddingHeight, buttonHeight, buttonHeight);
	this->bypassButton->setBounds(bypassRect);
}

void InstrComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingHeight = screenSize.getHeight() * 0.0025;
	int paddingWidth = screenSize.getWidth() * 0.01;
	int splitWidth = screenSize.getWidth() * 0.0025;

	float borderThickness = screenSize.getHeight() * 0.0025;

	int buttonHeight = this->getHeight() - paddingHeight * 2;
	float textHeight = buttonHeight * 0.8;
	int ioHideWidth = screenSize.getWidth() * 0.075;

	bool ioShown = this->getWidth() > ioHideWidth;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(this->editorOpened
		? juce::Label::ColourIds::backgroundWhenEditingColourId
		: juce::Label::ColourIds::backgroundColourId);
	juce::Colour textColor = laf.findColour(this->editorOpened
		? juce::Label::ColourIds::textWhenEditingColourId
		: juce::Label::ColourIds::textColourId);
	juce::Colour borderColor = laf.findColour(
		juce::Label::ColourIds::outlineWhenEditingColourId);

	/** Font */
	juce::Font textFont(textHeight);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Outline */
	if (this->dragHovered) {
		auto totalRect = this->getLocalBounds();
		g.setColour(borderColor);
		g.drawRect(totalRect.toFloat(), borderThickness);
	}

	/** Text */
	int textLeft = paddingWidth;
	int textRight = this->getWidth() - paddingWidth - buttonHeight - splitWidth;
	if (ioShown) {
		textLeft += (buttonHeight + splitWidth);
		textRight -= (buttonHeight + splitWidth);
	}
	juce::Rectangle<int> textRect(
		textLeft, paddingHeight,
		textRight - textLeft,
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

		this->input->update(index);
		this->output->update(index);

		this->repaint();

		this->setTooltip(this->createToolTip());
	}
}

void InstrComponent::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		this->editorShow();
	}
	else if (event.mods.isRightButtonDown()) {
		this->showMenu();
	}
}

bool InstrComponent::isInterestedInDragSource(
	const SourceDetails& dragSourceDetails) {
	auto& des = dragSourceDetails.description;

	/** From Track Audio Input */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackAudioInput)) {
		return true;
	}

	return false;
}

void InstrComponent::itemDragEnter(
	const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	this->preDrop();
}

void InstrComponent::itemDragExit(
	const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	this->endDrop();
}

void InstrComponent::itemDropped(
	const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	this->endDrop();

	auto& des = dragSourceDetails.description;

	/** From Track Audio Input */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackAudioInput)) {
		int track = des["track"];

		this->output->outputTo(track);
		return;
	}
}

void InstrComponent::preDrop() {
	this->dragHovered = true;
	this->repaint();
}

void InstrComponent::endDrop() {
	this->dragHovered = false;
	this->repaint();
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

enum InstrMenuActionType {
	Bypass = 1, Remove
};

void InstrComponent::showMenu() {
	auto menu = this->createMenu();
	int result = menu.show();

	switch (result) {
	case InstrMenuActionType::Bypass: {
		this->bypass();
		break;
	}
	case InstrMenuActionType::Remove: {
		CoreActions::removeInstrGUI(this->index);
		break;
	}
	}
}

juce::String InstrComponent::createToolTip() const {
	juce::String result =
		"#" + juce::String{ this->index } + "\n"
		+ TRANS("Name:") + " " + this->name + "\n"
		+ TRANS("Bypassed:") + " " + TRANS(this->bypassButton->getToggleState() ? "No" : "Yes") + "\n";

	return result;
}

juce::PopupMenu InstrComponent::createMenu() const {
	juce::PopupMenu menu;

	menu.addItem(InstrMenuActionType::Bypass, TRANS("Bypass"), true, !(this->bypassButton->getToggleState()));
	menu.addItem(InstrMenuActionType::Remove, TRANS("Remove"));

	return menu;
}

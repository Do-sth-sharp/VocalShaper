#include "EffectComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../misc/PluginEditorHub.h"
#include "../../misc/DragSourceType.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"
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
		if (!event.mouseWasDraggedSinceMouseDown()) {
			this->editorShow();
		}
	}
	else if (event.mods.isRightButtonDown()) {
		this->showMenu();
	}
}

void EffectComponent::mouseDrag(const juce::MouseEvent& event) {
	/** Start Drag */
	if (event.mods.isLeftButtonDown()) {
		this->startDrag();
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

enum EffectMenuActionType {
	Bypass = 1, Remove
};

void EffectComponent::showMenu() {
	/** Callback */
	auto addCallback = [comp = juce::Component::SafePointer{ this }](const juce::PluginDescription& pluginDes) {
		if (comp) {
			comp->addEffect(pluginDes);
		}
		};
	auto editCallback = [comp = juce::Component::SafePointer{ this }](const juce::PluginDescription& pluginDes) {
		if (comp) {
			comp->replaceEffect(pluginDes);
		}
		};

	/** Create Menu */
	auto menu = this->createMenu(addCallback, editCallback);
	int result = menu.show();

	switch (result) {
	case EffectMenuActionType::Bypass: {
		this->bypass();
		break;
	}
	case EffectMenuActionType::Remove: {
		CoreActions::removeEffectGUI(this->track, this->index);
		break;
	}
	}
}

void EffectComponent::startDrag() {
	if (auto container = juce::DragAndDropContainer::findParentDragContainerFor(this)) {
		container->startDragging(this->getDragSourceDescription(),
			this, juce::ScaledImage{}, true);
	}
}

void EffectComponent::addEffect(
	const juce::PluginDescription& pluginDes) {
	CoreActions::insertEffect(this->track, this->index + 1,
		pluginDes.createIdentifierString());
}

void EffectComponent::replaceEffect(
	const juce::PluginDescription& pluginDes) {
	CoreActions::replaceEffect(this->track, this->index,
		pluginDes.createIdentifierString());
}

juce::var EffectComponent::getDragSourceDescription() const {
	auto object = std::make_unique<juce::DynamicObject>();

	object->setProperty("type", (int)DragSourceType::Effect);
	object->setProperty("name", this->name);
	object->setProperty("track", this->track);
	object->setProperty("index", this->index);

	return juce::var{ object.release() };
}

juce::String EffectComponent::createToolTip() const {
	juce::String result =
		"#" + juce::String{ this->index } + "\n"
		+ TRANS("Name:") + " " + this->name + "\n"
		+ TRANS("Bypassed:") + " " + TRANS(this->bypassButton->getToggleState() ? "No" : "Yes") + "\n";

	return result;
}

juce::PopupMenu EffectComponent::createMenu(
	const std::function<void(const juce::PluginDescription&)>& addCallback,
	const std::function<void(const juce::PluginDescription&)>& editCallback) const {
	juce::PopupMenu menu;

	menu.addSubMenu(TRANS("Add"), this->createAddMenu(addCallback));
	menu.addSubMenu(TRANS("Replace"), this->createAddMenu(editCallback));
	menu.addItem(EffectMenuActionType::Bypass, TRANS("Bypass"), true, !(this->bypassButton->getToggleState()));
	menu.addItem(EffectMenuActionType::Remove, TRANS("Remove"));

	return menu;
}

juce::PopupMenu EffectComponent::createAddMenu(
	const std::function<void(const juce::PluginDescription&)>& callback) const {
	/** Create Menu */
	auto [valid, list] = quickAPI::getPluginList(true, false);
	if (!valid) { return juce::PopupMenu{}; }
	auto groups = utils::groupPlugin(list, utils::PluginGroupType::Category);
	return utils::createPluginMenu(groups, callback);
}

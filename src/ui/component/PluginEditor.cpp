#include "PluginEditor.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreActions.h"
#include "../misc/PluginEditorHub.h"
#include "../Utils.h"
#include <IconManager.h>

PluginEditorContent::PluginEditorContent(PluginEditor* parent, 
	const juce::String& name, PluginType type,
	quickAPI::PluginHolder plugin, quickAPI::EditorPointer editor)
	: Component(name), parent(parent), plugin(plugin),
	editor(editor), type(type) {
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
	this->editorViewport->setScrollBarPosition(false, false);
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

	/** Update Now */
	this->update();
}

PluginEditorContent::~PluginEditorContent() {
	if (this->editor) {
		this->editor->removeComponentListener(this);
	}
}

quickAPI::EditorPointer PluginEditorContent::getEditor() const {
	return this->editor;
}

juce::Point<int> PluginEditorContent::getPerferedSize() {
	auto screenSize = utils::getScreenSize(this);
	int toolBarHeight = screenSize.getHeight() * 0.03;
	if (this->editor) {
		/** Can't Get The Actual Size */
		return { (int)(this->editor->getWidth() * 1.03),
			(int)(this->editor->getHeight() * 1.13) + toolBarHeight };
	}
	return { screenSize.getWidth() / 4, screenSize.getHeight() / 4 };
}

void PluginEditorContent::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int toolBarHeight = screenSize.getHeight() * 0.03;
	int toolPaddingHeight = screenSize.getHeight() * 0.003;
	int toolPaddingWidth = screenSize.getWidth() * 0.003;
	int toolSplitWidth = screenSize.getWidth() * 0.003;

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

	juce::Rectangle<int> pinRect(
		configRect.getRight() + toolSplitWidth, toolPaddingHeight,
		buttonHeight, buttonHeight);
	this->pinButton->setBounds(pinRect);

	/** Content */
	juce::Rectangle<int> contentRect =
		this->getLocalBounds().withTrimmedTop(toolBarHeight);
	this->editorViewport->setBounds(contentRect);
	this->configViewport->setBounds(contentRect);
}

void PluginEditorContent::paint(juce::Graphics& g) {
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();
}

void PluginEditorContent::update() {
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

void PluginEditorContent::componentBeingDeleted(juce::Component&) {
	/** Remove Editor */
	this->editorViewport->setViewedComponent(nullptr, false);
	this->update();

	/** Delete From Hub */
	this->deleteEditor();
}

void PluginEditorContent::componentMovedOrResized(
	juce::Component&, bool wasMoved, bool wasResized) {
	if (wasResized) {
		this->parent->updateSize();
	}
}

void PluginEditorContent::bypass() {
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

void PluginEditorContent::config() {
	bool newState = !(this->configButton->getToggleState());
	this->editorViewport->setVisible(!newState);
	this->configViewport->setVisible(newState);
	this->configButton->setToggleState(newState,
		juce::NotificationType::dontSendNotification);
}

void PluginEditorContent::pin() {
	bool newState = !(this->pinButton->getToggleState());
	this->parent->setPinned(newState);
	this->pinButton->setToggleState(newState,
		juce::NotificationType::dontSendNotification);
}

void PluginEditorContent::deleteEditor() {
	switch (this->type) {
	case PluginType::Instr: {
		PluginEditorHub::getInstance()->
			deleteInstrEditor(this->parent);
		break;
	}
	case PluginType::Effect: {
		PluginEditorHub::getInstance()->
			deleteEffectEditor(this->parent);
		break;
	}
	}
}

PluginEditor::PluginEditor(const juce::String& name, PluginType type,
	quickAPI::PluginHolder plugin, quickAPI::EditorPointer editor)
	: DocumentWindow(name, juce::LookAndFeel::getDefaultLookAndFeel().findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId), 
		juce::DocumentWindow::allButtons, true) {
	this->setUsingNativeTitleBar(true);
	this->setResizable(true, false);

	this->setContentOwned(
		new PluginEditorContent{
			this, name, type, plugin, editor }, false);

	juce::MessageManager::callAsync(
		[this] {
			this->updateSize();
		}
	);
}

PluginEditor::~PluginEditor() {
	this->renderer = nullptr;
}

quickAPI::EditorPointer PluginEditor::getEditor() const {
	if (auto ptr = dynamic_cast<PluginEditorContent*>(this->getContentComponent())) {
		return ptr->getEditor();
	}
	return nullptr;
}

void PluginEditor::update() {
	if (auto ptr = dynamic_cast<PluginEditorContent*>(this->getContentComponent())) {
		return ptr->update();
	}
}

void PluginEditor::updateSize() {
	if (auto ptr = dynamic_cast<PluginEditorContent*>(this->getContentComponent())) {
		auto size = ptr->getPerferedSize();

		this->setResizeLimits(
			size.getX() / 2, size.getY() / 2,
			size.getX(), size.getY());
		this->centreWithSize(size.getX(), size.getY());
	}
}

void PluginEditor::setOpenGL(bool openGLOn) {
	if (openGLOn) {
		this->renderer = std::make_unique<juce::OpenGLContext>();
		this->renderer->attachTo(*this);
	}
	else {
		this->renderer = nullptr;
	}
}

void PluginEditor::setWindowIcon(const juce::Image& icon) {
	this->setIcon(icon);
	this->getPeer()->setIcon(icon);
}

void PluginEditor::setPinned(bool pin) {
	this->setAlwaysOnTop(pin);
}

bool PluginEditor::getPinned() const {
	return this->isAlwaysOnTop();
}

void PluginEditor::closeButtonPressed() {
	if (auto ptr = dynamic_cast<PluginEditorContent*>(this->getContentComponent())) {
		return ptr->deleteEditor();
	}
}

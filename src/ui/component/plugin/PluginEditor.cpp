#include "PluginEditor.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../misc/PluginEditorHub.h"
#include "../../Utils.h"

PluginEditorContent::PluginEditorContent(PluginEditor* parent, 
	const juce::String& name, PluginType type,
	quickAPI::PluginHolder plugin, quickAPI::EditorPointer editor)
	: Component(name), parent(parent), plugin(plugin),
	editor(editor), type(type) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forPluginEditor());

	if (editor) {
		/** Add Listener To Comp */
		editor->addComponentListener(this);

		/** Add As Child */
		this->addAndMakeVisible(editor);
	}

	/** Tool Bar */
	this->toolBar = std::make_unique<PluginToolBar>(this, plugin, type);
	this->addAndMakeVisible(this->toolBar.get());

	/** Config Viewport */
	this->pluginProp = std::make_unique<PluginPropComponent>(type, plugin);
	this->configViewport = std::make_unique<juce::Viewport>(TRANS("Plugin Config"));
	this->configViewport->setViewedComponent(this->pluginProp.get(), false);
	this->configViewport->setScrollBarsShown(true, false);
	this->configViewport->setScrollOnDragMode(
		juce::Viewport::ScrollOnDragMode::nonHover);
	this->addChildComponent(this->configViewport.get());

	/** Update Now */
	this->update();

	/** Update Size */
	auto size = this->getPerferedSize();
	this->setSize(size.getX(), size.getY());
}

PluginEditorContent::~PluginEditorContent() {
	if (this->editor) {
		this->editor->removeComponentListener(this);
	}
}

quickAPI::EditorPointer PluginEditorContent::getEditor() const {
	return this->editor;
}

bool PluginEditorContent::isResizable() const {
	if (this->editor) {
		return this->editor->isResizable();
	}
	return false;
}

juce::Point<int> PluginEditorContent::getPerferedSize() {
	auto screenSize = utils::getScreenSize(this);
	int toolBarHeight = this->getToolBarHeight();
	if (this->editor) {
		auto bounds = this->editor->getBounds();
		return { bounds.getWidth(), bounds.getHeight() + toolBarHeight };
	}
	return { screenSize.getWidth() / 4, screenSize.getHeight() / 4 };
}

void PluginEditorContent::resized() {
	/** Size */
	int toolBarHeight = this->getToolBarHeight();

	/** Tool Bar */
	this->toolBar->setBounds(
		0, 0, this->getWidth(), toolBarHeight);

	/** Config View Pos */
	auto configViewPos = this->configViewport->getViewPosition();

	/** Content */
	juce::Rectangle<int> contentRect =
		this->getLocalBounds().withTrimmedTop(toolBarHeight);
	this->configViewport->setBounds(contentRect);
	if (this->editor) {
		if (this->editor->isResizable()) {
			this->editor->setBounds(contentRect);
		}
		else {
			this->editor->setTopLeftPosition(contentRect.getTopLeft());
		}
	}

	/** Config View Port */
	{
		int contentWidth = this->configViewport->getMaximumVisibleWidth();
		int contentHeight = this->pluginProp->getPreferedHeight();
		this->pluginProp->setBounds(0, 0,
			contentWidth, std::max(contentHeight, contentRect.getHeight()));
		this->configViewport->setViewPosition(configViewPos);
	}
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
	this->toolBar->update();
	this->pluginProp->update();
}

void PluginEditorContent::componentBeingDeleted(juce::Component&) {
	/** Update Plugin */
	this->update();

	/** Delete From Hub */
	this->deleteEditor();
}

void PluginEditorContent::componentMovedOrResized(
	juce::Component&, bool wasMoved, bool wasResized) {
	if (wasResized) {
		juce::MessageManager::callAsync(
			[comp = PluginEditorContent::SafePointer{ this }] {
				if (comp) {
					auto size = comp->getPerferedSize();
					if (size.getX() != comp->getWidth() ||
						size.getY() != comp->getHeight()) {
						comp->setSize(size.getX(), size.getY());
						comp->parent->sizeChanged();
					}
				}
			}
		);
	}
}

void PluginEditorContent::config(bool showConfig) {
	if (this->editor) {
		this->editor->setVisible(!showConfig);
	}
	this->configViewport->setVisible(showConfig);
}

void PluginEditorContent::pin(bool pinned) {
	this->parent->setPinned(pinned);
}

void PluginEditorContent::setEditorScale(float scale) {
	if (this->editor) {
		this->editor->setScaleFactor(scale);
	}
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

int PluginEditorContent::getToolBarHeight() const {
	auto screenSize = utils::getScreenSize(this);
	return screenSize.getHeight() * 0.03;
}

PluginEditor::PluginEditor(const juce::String& name, PluginType type,
	quickAPI::PluginHolder plugin, quickAPI::EditorPointer editor)
	: DocumentWindow(name, juce::LookAndFeel::getDefaultLookAndFeel().findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId), 
		juce::DocumentWindow::allButtons, true) {
	this->setUsingNativeTitleBar(true);

	auto content = new PluginEditorContent{
			this, name, type, plugin, editor };
	this->setContentOwned(content, true);

	bool resizable = content->isResizable();
	this->setResizable(resizable, false);

	this->sizeChanged();
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

void PluginEditor::sizeChanged() {
	juce::MessageManager::callAsync(
		[comp = juce::Component::SafePointer(this)] {
			if (comp) {
				comp->limitBounds();
			}
		}
	);
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

void PluginEditor::limitBounds() {
	auto screenBounds = this->getBounds();
	/**
	 * I can't get the native title bar height.
	 * I think 31px is enough for title area.
	 */
	int titleHeight = 31;
	if (screenBounds.getY() < titleHeight) {
		this->setBounds(screenBounds.withY(titleHeight));
	}
}

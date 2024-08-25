#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "PluginPropComponent.h"
#include "PluginToolBar.h"
#include "../../misc/PluginType.h"
#include "../../../audioCore/AC_API.h"

class PluginEditor;

class PluginEditorContent final 
	: public juce::Component,
	public juce::ComponentListener {
public:
	PluginEditorContent(PluginEditor* parent, const juce::String& name, PluginType type,
		quickAPI::PluginHolder plugin, quickAPI::EditorPointer editor);
	~PluginEditorContent();

	quickAPI::EditorPointer getEditor() const;
	bool isResizable() const;

	juce::Point<int> getPerferedSize();
	void resized() override;
	void paint(juce::Graphics& g) override;

	void update();

private:
	PluginEditor* const parent;
	const quickAPI::PluginHolder plugin;
	const quickAPI::EditorPointer editor;
	const PluginType type;

	std::unique_ptr<PluginToolBar> toolBar = nullptr;
	std::unique_ptr<juce::Viewport> configViewport = nullptr;
	std::unique_ptr<PluginPropComponent> pluginProp = nullptr;

	bool resizingFlag = false;

	void componentBeingDeleted(juce::Component&) override;
	void componentMovedOrResized(juce::Component&,
		bool wasMoved, bool wasResized) override;

	friend class PluginToolBar;
	void config(bool showConfig);
	void pin(bool pinned);
	void setEditorScale(float scale);

	friend class PluginEditor;
	void deleteEditor();

	int getToolBarHeight() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorContent)
};

class PluginEditor final : public juce::DocumentWindow {
public:
	PluginEditor() = delete;
	PluginEditor(const juce::String& name, PluginType type,
		quickAPI::PluginHolder plugin, quickAPI::EditorPointer editor);
	~PluginEditor();

	quickAPI::EditorPointer getEditor() const;

	void update();
	void sizeChanged();

	void setOpenGL(bool openGLOn);
	void setWindowIcon(const juce::Image& icon);

	void setPinned(bool pin);
	bool getPinned() const;

private:
	void closeButtonPressed() override;

private:
	std::unique_ptr<juce::OpenGLContext> renderer = nullptr;

	void limitBounds();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

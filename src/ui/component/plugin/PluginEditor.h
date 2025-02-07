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
	PluginEditorContent(PluginEditor* parent, const juce::String& name,
		PluginType type, quickAPI::EditorPointer editor);
	~PluginEditorContent();

	quickAPI::EditorPointer getEditor() const;
	bool isResizable() const;

	juce::Point<int> getPerferedSize();
	void resized() override;
	void paint(juce::Graphics& g) override;

	/** Type, Track, Index */
	using Index = std::tuple<int, int, int>;
	const Index getIndex() const;
	void update(int type, int track, int index);

private:
	PluginEditor* const parent;
	const quickAPI::EditorPointer editor;
	const PluginType type;

	int trackType = -1, track = -1, index = -1;

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
	PluginEditor(const juce::String& name,
		PluginType type, quickAPI::EditorPointer editor);
	~PluginEditor();

	quickAPI::EditorPointer getEditor() const;

	/** Type, Track, Index */
	using Index = PluginEditorContent::Index;
	const Index getIndex() const;
	void update(int type, int track, int index);
	void sizeChanged();

	void setOpenGL(bool openGLOn);
	void setBufferedPainting(bool bufferedPainting);
	void setWindowIcon(const juce::Image& icon);

	void setPinned(bool pin);
	bool getPinned() const;

private:
	void closeButtonPressed() override;

private:
	std::unique_ptr<juce::OpenGLContext> renderer = nullptr;
	bool bufferedPainting = false;

	void limitBounds();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

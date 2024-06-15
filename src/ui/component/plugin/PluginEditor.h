#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "PluginPropComponent.h"
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

	juce::Point<int> getPerferedSize();
	void resized() override;
	void paint(juce::Graphics& g) override;

	void update();

private:
	PluginEditor* const parent;
	const quickAPI::PluginHolder plugin;
	const quickAPI::EditorPointer editor;
	const PluginType type;

	std::unique_ptr<juce::Viewport> configViewport = nullptr;
	std::unique_ptr<juce::Drawable> bypassIcon = nullptr;
	std::unique_ptr<juce::Drawable> bypassIconOn = nullptr;
	std::unique_ptr<juce::Drawable> configIcon = nullptr;
	std::unique_ptr<juce::Drawable> configIconOn = nullptr;
	std::unique_ptr<juce::Drawable> pinIcon = nullptr;
	std::unique_ptr<juce::Drawable> pinIconOn = nullptr;
	std::unique_ptr<juce::DrawableButton> bypassButton = nullptr;
	std::unique_ptr<juce::DrawableButton> configButton = nullptr;
	std::unique_ptr<juce::DrawableButton> pinButton = nullptr;
	std::unique_ptr<PluginPropComponent> pluginProp = nullptr;
	std::unique_ptr<juce::TextButton> scaleButton = nullptr;

	void componentBeingDeleted(juce::Component&) override;
	void componentMovedOrResized(juce::Component&,
		bool wasMoved, bool wasResized) override;

	void bypass();
	void config();
	void pin();
	void changeScale();

	float scale = 1.0;
	void setEditorScale(float scale);
	float getEditorScale() const;

	friend class PluginEditor;
	void deleteEditor();

	juce::PopupMenu createScaleMenu();

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
	void sizeChanged(const juce::Point<int>& size);

	void setOpenGL(bool openGLOn);
	void setWindowIcon(const juce::Image& icon);

	void setPinned(bool pin);
	bool getPinned() const;

private:
	void closeButtonPressed() override;

private:
	std::unique_ptr<juce::OpenGLContext> renderer = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "../misc/PluginType.h"
#include "../../audioCore/AC_API.h"

class PluginEditor final 
	: public flowUI::FlowComponent,
	public juce::ComponentListener {
public:
	PluginEditor(const juce::String& name, PluginType type,
		quickAPI::PluginHolder plugin,
		juce::Component::SafePointer<juce::AudioProcessorEditor> editor);
	~PluginEditor();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update();

private:
	const quickAPI::PluginHolder plugin;
	const juce::Component::SafePointer<juce::AudioProcessorEditor> editor;
	const PluginType type;

	std::unique_ptr<juce::Viewport> editorViewport = nullptr;
	std::unique_ptr<juce::Viewport> configViewport = nullptr;
	std::unique_ptr<juce::Drawable> bypassIcon = nullptr;
	std::unique_ptr<juce::Drawable> bypassIconOn = nullptr;
	std::unique_ptr<juce::Drawable> configIcon = nullptr;
	std::unique_ptr<juce::Drawable> configIconOn = nullptr;
	std::unique_ptr<juce::DrawableButton> bypassButton = nullptr;
	std::unique_ptr<juce::DrawableButton> configButton = nullptr;

	void componentBeingDeleted(juce::Component&) override;

	void bypass();
	void config();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

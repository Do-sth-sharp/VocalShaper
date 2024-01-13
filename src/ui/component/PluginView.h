#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>

class PluginView final 
	: public flowUI::FlowComponent,
	public juce::DragAndDropContainer {
public:
	PluginView();

	void resized() override;
	void paint(juce::Graphics& g) override;

private:
	std::unique_ptr<juce::Drawable> searchIcon = nullptr;
	std::unique_ptr<juce::TextEditor> searchBox = nullptr;
	std::unique_ptr<juce::TreeView> pluginTree = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginView)
};

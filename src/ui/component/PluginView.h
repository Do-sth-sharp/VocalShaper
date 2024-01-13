#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "../dataModel/PluginTreeModel.h"

class PluginView final 
	: public flowUI::FlowComponent,
	public juce::DragAndDropContainer {
public:
	PluginView();
	~PluginView();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update();

private:
	std::unique_ptr<juce::Drawable> searchIcon = nullptr;
	std::unique_ptr<juce::TextEditor> searchBox = nullptr;
	std::unique_ptr<juce::TreeView> pluginTree = nullptr;
	std::unique_ptr<PluginTreeModel> pluginModel = nullptr;

	juce::String searchingMes;

	enum class GroupType {
		Format, Manufacturer, Category
	};
	GroupType groupType = GroupType::Manufacturer;

	void searchStart();
	void searchEnd();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginView)
};

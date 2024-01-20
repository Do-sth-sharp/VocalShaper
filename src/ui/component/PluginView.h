#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "../dataModel/PluginTreeModel.h"
#include "../Utils.h"

class PluginView final 
	: public flowUI::FlowComponent,
	public juce::DragAndDropContainer {
public:
	PluginView();
	~PluginView();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update();
	void searchUpdate();
	void rescan();
	void expandAll();
	void foldAll();

	void showGroupMenu(const juce::String& name);
	void showPluginMenu(const juce::PluginDescription& plugin);

private:
	std::unique_ptr<juce::Drawable> searchIcon = nullptr;
	std::unique_ptr<juce::TextEditor> searchBox = nullptr;
	std::unique_ptr<juce::TreeView> pluginTree = nullptr;
	std::unique_ptr<PluginTreeModel> pluginModel = nullptr;

	juce::String searchingMes;

	utils::PluginGroupType groupType = utils::PluginGroupType::Manufacturer;

	void searchStart();
	void searchEnd();

	juce::PopupMenu createGroupMenu() const;
	juce::PopupMenu createPluginMenu() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginView)
};

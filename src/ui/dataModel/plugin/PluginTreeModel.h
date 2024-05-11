#pragma once

#include <JuceHeader.h>
#include "../../Utils.h"

class PluginTreeModel final : public juce::TreeViewItem {
public:
	PluginTreeModel() = delete;
	using PluginClass = utils::PluginGroup;
	PluginTreeModel(const juce::Array<PluginClass>& plugins,
		const std::function<void(const juce::String&)>& groupMenuCallback,
		const std::function<void(const juce::PluginDescription&)>& pluginMenuCallback);

	bool mightContainSubItems() override;
	bool canBeSelected() const override;
	bool isInterestedInFileDrag(const juce::StringArray&) override;
	bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails&) override;

	void changeAllOpenness(bool open);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginTreeModel)
};

#pragma once

#include <JuceHeader.h>

class PluginTreeModel final : public juce::TreeViewItem {
public:
	PluginTreeModel() = delete;
	using PluginClass = std::tuple<const juce::String&, const juce::Array<juce::PluginDescription>&>;
	PluginTreeModel(const juce::Array<PluginClass>& plugins);

	bool mightContainSubItems() override;
	bool canBeSelected() const override;
	bool isInterestedInFileDrag(const juce::StringArray&) override;
	bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails&) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginTreeModel)
};

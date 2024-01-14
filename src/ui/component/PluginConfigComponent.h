#pragma once

#include <JuceHeader.h>
#include "../dataModel/EditableStringListModel.h"

class PluginConfigComponent final : public juce::Component {
public:
	PluginConfigComponent();

	int getHeightPrefered() const;

	void resized() override;

	void addPath(int index);
	void removePath(int index, const juce::String& path);
	void addBlackList(int index);
	void removeBlackList(int index, const juce::String& path);

	void rescan();

private:
	std::unique_ptr<juce::Label> tipLabel = nullptr;
	std::unique_ptr<juce::TextButton> rescanButton = nullptr;
	std::unique_ptr<juce::Label> pathLabel = nullptr;
	std::unique_ptr<EditableStringListModel> pathModel = nullptr;
	std::unique_ptr<juce::ListBox> pathList = nullptr;
	std::unique_ptr<juce::Label> blackListLabel = nullptr;
	std::unique_ptr<EditableStringListModel> blackListModel = nullptr;
	std::unique_ptr<juce::ListBox> blackList = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginConfigComponent)
};

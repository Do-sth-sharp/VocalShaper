#pragma once

#include <JuceHeader.h>

class PluginConfigComponent final : public juce::Component {
public:
	PluginConfigComponent();

	int getHeightPrefered() const;

	void resized() override;

private:
	std::unique_ptr<juce::Label> pathLabel = nullptr;
	std::unique_ptr<juce::ListBox> pathList = nullptr;
	std::unique_ptr<juce::Label> blackListLabel = nullptr;
	std::unique_ptr<juce::ListBox> blackList = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginConfigComponent)
};

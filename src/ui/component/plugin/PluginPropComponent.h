#pragma once

#include <JuceHeader.h>
#include "../../dataModel/plugin/PluginAutomationModel.h"
#include "../../misc/PluginType.h"

class PluginPropComponent final : public juce::Component {
public:
	PluginPropComponent() = delete;
	PluginPropComponent(PluginType type);

	int getPreferedHeight() const;

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update(int type, int track, int index);

private:
	const PluginType type;
	int trackType = -1, track = -1, index = -1;

	juce::String midiTitle, automaticTitle, channelLabel;
	std::unique_ptr<juce::ComboBox> midiChannel = nullptr;
	std::unique_ptr<juce::ToggleButton> midiCCIntercept = nullptr;
	std::unique_ptr<juce::ToggleButton> midiOutput = nullptr;
	std::unique_ptr<juce::TableListBox> automaticList = nullptr;
	std::unique_ptr<PluginAutomationModel> automaticModel = nullptr;

	void changeMIDIChannel();
	void changeMIDICCIntercept();
	void changeMIDIOutput();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginPropComponent)
};

#pragma once

#include <JuceHeader.h>
#include "../misc/PluginType.h"
#include "../../audioCore/AC_API.h"

class PluginPropComponent final : public juce::Component {
public:
	PluginPropComponent() = delete;
	PluginPropComponent(
		PluginType type, quickAPI::PluginHolder plugin);

	int getPreferedHeight() const;

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update();

private:
	const PluginType type;
	const quickAPI::PluginHolder plugin;

	juce::String midiTitle, automaticTitle, channelLabel;
	std::unique_ptr<juce::ComboBox> midiChannel = nullptr;
	std::unique_ptr<juce::ToggleButton> midiCCIntercept = nullptr;
	std::unique_ptr<juce::ToggleButton> midiOutput = nullptr;
	std::unique_ptr<juce::TableListBox> automaticList = nullptr;

	void changeMIDIChannel();
	void changeMIDICCIntercept();
	void changeMIDIOutput();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginPropComponent)
};

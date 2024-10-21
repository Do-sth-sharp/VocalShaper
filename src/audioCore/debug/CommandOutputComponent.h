#pragma once

#include <JuceHeader.h>

class CommandOutputComponent final : public juce::Component {
public:
	CommandOutputComponent() = default;

	void add(const juce::String& line);

	void paint(juce::Graphics& g) override;

private:
	juce::StringArray outList;
	juce::String strTemp;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandOutputComponent)
};

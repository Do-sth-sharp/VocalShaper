#pragma once

#include <JuceHeader.h>

class ConfigComponent final : public juce::Component {
public:
	ConfigComponent();

	void paint(juce::Graphics& g) override;

	void setPage(int index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigComponent)
};

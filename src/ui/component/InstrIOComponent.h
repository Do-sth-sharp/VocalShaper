#pragma once

#include <JuceHeader.h>

class InstrIOComponent final : public juce::Component {
public:
	InstrIOComponent() = delete;
	InstrIOComponent(bool isInput);

	void paint(juce::Graphics& g) override;

	void update(int index);

private:
	const bool isInput;
	int index = -1;
	bool linked = false;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrIOComponent)
};

#pragma once

#include <JuceHeader.h>

class MIDISourceEditor final : public juce::Component {
public:
	MIDISourceEditor();

	void paint(juce::Graphics& g) override;

	void update(uint64_t ref);

private:
	uint64_t ref = 0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDISourceEditor)
};

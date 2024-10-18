#pragma once

#include <JuceHeader.h>

class AudioSourceEditor final : public juce::Component {
public:
	AudioSourceEditor();

	void paint(juce::Graphics& g) override;

	void update(uint64_t ref);

private:
	uint64_t ref = 0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSourceEditor)
};

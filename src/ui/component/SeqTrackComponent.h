#pragma once

#include <JuceHeader.h>

class SeqTrackComponent final : public juce::Component {
public:
	SeqTrackComponent();

	void update(int index);

private:
	int index = -1;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackComponent)
};

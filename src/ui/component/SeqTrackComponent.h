#pragma once

#include <JuceHeader.h>

class SeqTrackComponent final : public juce::Component {
public:
	SeqTrackComponent();

	void update(int index);
	void updateBlock(int blockIndex);
	void updateHPos(double pos, double itemSize);

	void paint(juce::Graphics& g) override;

private:
	int index = -1;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackComponent)
};

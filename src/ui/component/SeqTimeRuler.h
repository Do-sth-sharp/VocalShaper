#pragma once

#include <JuceHeader.h>

class SeqTimeRuler final : public juce::Component {
public:
	SeqTimeRuler();

	void updateHPos(double pos, double itemSize);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTimeRuler)
};

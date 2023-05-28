#pragma once

#include <JuceHeader.h>
#include "CloneableSource.h"

class CloneableMIDISource final : public CloneableSource {
public:
	CloneableMIDISource() = default;
	~CloneableMIDISource() override = default;

private:
	bool clone(const CloneableSource* src) override;
	bool load(const juce::File& file) override;
	bool save(const juce::File& file) const override;
	double getLength() const override;

private:
	juce::MidiFile buffer;

	static double convertTicksToSeconds(double time,
		const juce::MidiMessageSequence& tempoEvents,
		int timeFormat);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableMIDISource)
};

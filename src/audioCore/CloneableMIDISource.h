#pragma once

#include <JuceHeader.h>
#include "CloneableSource.h"

class CloneableMIDISource final : public CloneableSource {
public:
	CloneableMIDISource() = default;
	~CloneableMIDISource() override = default;

	void readData(
		juce::MidiBuffer& buffer, double baseTime,
		double startTime, double endTime) const;
	int getTrackNum() const;

private:
	bool clone(const CloneableSource* src) override;
	bool load(const juce::File& file) override;
	bool save(const juce::File& file) const override;
	double getLength() const override;

private:
	juce::MidiFile buffer;
	juce::ReadWriteLock lock;

	static void convertSecondsToTicks(juce::MidiFile& file);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableMIDISource)
};

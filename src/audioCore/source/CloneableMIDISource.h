#pragma once

#include <JuceHeader.h>
#include "CloneableSource.h"

class CloneableMIDISource final : public CloneableSource {
public:
	CloneableMIDISource() = default;
	~CloneableMIDISource() override = default;

	void readData(
		juce::MidiBuffer& buffer, int baseTime,
		int startTime, int endTime) const;
	int getTrackNum() const;
	int getEventNum() const;

public:
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<google::protobuf::Message> serialize() const override;

private:
	std::unique_ptr<CloneableSource> createThisType() const override;
	bool clone(CloneableSource* dst) const override;
	bool load(const juce::File& file) override;
	bool save(const juce::File& file) const override;
	double getLength() const override;

private:
	friend class SourceRecordProcessor;
	void prepareToRecord(
		int inputChannels, double sampleRate,
		int blockSize, bool updateOnly) override;
	void recordingFinished() override;
	void writeData(const juce::MidiBuffer& buffer, int offset);

private:
	juce::MidiFile buffer;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableMIDISource)
};

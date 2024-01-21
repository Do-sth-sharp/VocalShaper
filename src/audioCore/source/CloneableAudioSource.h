#pragma once

#include <JuceHeader.h>
#include "CloneableSource.h"

class CloneableAudioSource final : public CloneableSource {
public:
	CloneableAudioSource() = default;
	~CloneableAudioSource() override = default;

	double getSourceSampleRate() const override;

	void readData(juce::AudioBuffer<float>& buffer, int bufferOffset,
		int dataOffset, int length) const;
	int getChannelNum() const override;

public:
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<google::protobuf::Message> serialize() const override;

private:
	std::unique_ptr<CloneableSource> createThisType() const override;
	bool clone(CloneableSource* dst) const override;
	bool load(const juce::File& file) override;
	bool save(const juce::File& file) const override;
	double getLength() const override;
	void sampleRateChanged() override;
	void init(double sampleRate, int channelNum, int sampleNum) override;

private:
	friend class SourceRecordProcessor;
	void prepareToRecord(
		int inputChannels, double sampleRate,
		int blockSize, bool updateOnly) override;
	void recordingFinished() override;
	void writeData(juce::AudioBuffer<float>& buffer, int offset);

private:
	juce::AudioSampleBuffer buffer;
	std::unique_ptr<juce::MemoryAudioSource> memorySource = nullptr;
	std::unique_ptr<juce::ResamplingAudioSource> source = nullptr;
	double sourceSampleRate = 0;

	juce::AudioSampleBuffer* getAudioContentPtr() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableAudioSource)
};
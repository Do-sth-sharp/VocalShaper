#pragma once

#include <JuceHeader.h>
#include "CloneableSource.h"
#include "SynthRenderThread.h"

class CloneableSynthSource final : public CloneableSource {
public:
	CloneableSynthSource();
	~CloneableSynthSource() override = default;

	int getTrackNum() const;

	double getSourceSampleRate() const;
	void readData(juce::AudioBuffer<float>& buffer, int bufferOffset,
		int dataOffset, int length) const;
	int getChannelNum() const;
	int getEventNum() const;

	void setSynthesizer(
		std::unique_ptr<juce::AudioPluginInstance> synthesizer,
		const juce::String& identifier);
	const juce::String getSynthesizerName() const;
	void stopSynth();
	void synth();

public:
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<google::protobuf::Message> serialize() const override;

private:
	std::unique_ptr<CloneableSource> createThisType() const override;
	bool clone(CloneableSource* dst) const override;
	bool load(const juce::File& file) override;
	bool save(const juce::File& file) const override;
	bool exportt(const juce::File& file) const override;
	double getLength() const override;
	void sampleRateChanged() override;

private:
	friend class SourceRecordProcessor;
	void prepareToRecord(
		int inputChannels, double sampleRate,
		int blockSize, bool updateOnly) override;
	void recordingFinished() override;
	void writeData(const juce::MidiBuffer& buffer, int offset);

private:
	juce::MidiFile buffer;

	juce::AudioSampleBuffer audioBuffer;
	std::unique_ptr<juce::MemoryAudioSource> memorySource = nullptr;
	std::unique_ptr<juce::ResamplingAudioSource> source = nullptr;
	std::atomic<double> sourceSampleRate = 0;

	int audioChannels = 1;

	friend class SynthRenderThread;
	std::unique_ptr<SynthRenderThread> synthThread = nullptr;
	std::unique_ptr<juce::AudioPluginInstance> synthesizer = nullptr;
	juce::String pluginIdentifier;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableSynthSource)
};
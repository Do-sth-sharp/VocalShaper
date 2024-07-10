#pragma once

#include <JuceHeader.h>

class SourceManager final : private juce::DeletedAtShutdown {
public:
	SourceManager() = default;

	enum class SourceType {
		Undefined, MIDI, Audio
	};
	uint64_t applySource(SourceType type);
	void releaseSource(uint64_t ref);

	const juce::String getFileName(uint64_t ref, SourceType type) const;
	void changed(uint64_t ref, SourceType type);
	void saved(uint64_t ref, SourceType type);
	bool isSaved(uint64_t ref, SourceType type) const;
	bool isValid(uint64_t ref, SourceType type) const;

	int getMIDITrackNum(uint64_t ref) const;
	double getLength(uint64_t ref, SourceType type) const;

	void initAudio(uint64_t ref, int channelNum, double sampleRate, double length,
		int blockSize, double playSampleRate);
	void initMIDI(uint64_t ref);
	void setAudio(uint64_t ref, double sampleRate, const juce::AudioSampleBuffer& data, const juce::String& name,
		int blockSize, double playSampleRate);
	void setMIDI(uint64_t ref, const juce::MidiFile& data, const juce::String& name);
	const std::tuple<double, juce::AudioSampleBuffer> getAudio(uint64_t ref) const;
	const juce::MidiMessageSequence getMIDI(uint64_t ref, int trackIndex) const;
	const juce::MidiFile getMIDIFile(uint64_t ref) const;

	void prepareAudioPlay(uint64_t ref, int blockSize, double sampleRate);
	void prepareMIDIPlay(uint64_t ref);
	void prepareAudioRecord(uint64_t ref, int channelNum,
		int blockSize, double playSampleRate);
	void prepareMIDIRecord(uint64_t ref);

public:
	void readAudioData(uint64_t ref, juce::AudioBuffer<float>& buffer, int bufferOffset,
		int dataOffset, int length) const;
	void readMIDIData(uint64_t ref, juce::MidiBuffer& buffer, double baseTime,
		double startTime, double endTime, int trackIndex, double sampleRate) const;
	void writeAudioData(uint64_t ref, juce::AudioBuffer<float>& buffer, int offset,
		int trackChannelNum, int blockSize, double sampleRate);
	void writeMIDIData(uint64_t ref, const juce::MidiBuffer& buffer, int offset, int trackIndex, double sampleRate);

public:
	void sampleRateChanged(double sampleRate, int blockSize);

private:
	class SourceItem final {
	public:
		SourceType type = SourceType::Undefined;
		std::unique_ptr<juce::MidiFile> midiData = nullptr;
		std::unique_ptr<juce::AudioSampleBuffer> audioData = nullptr;
		std::unique_ptr<juce::MemoryAudioSource> memSource = nullptr;
		std::unique_ptr<juce::ResamplingAudioSource> resampleSource = nullptr;
		double audioSampleRate = 0;
		juce::String fileName;
		std::atomic_bool savedFlag = true;

		void initAudio(int channelNum, double sampleRate, double length,
			int blockSize, double playSampleRate);
		void initMIDI();
		void setAudio(double sampleRate, const juce::AudioSampleBuffer& data, const juce::String& name,
			int blockSize, double playSampleRate);
		void setMIDI(const juce::MidiFile& data, const juce::String& name);
		const std::tuple<double, juce::AudioSampleBuffer> getAudio() const;
		const juce::MidiMessageSequence getMIDI(int trackIndex) const;
		const juce::MidiFile getMIDIFile() const;

		void changed();
		void saved();
		bool isSaved() const;

		void prepareAudioPlay(int blockSize, double sampleRate);
		void prepareMIDIPlay();
		void prepareAudioRecord(int channelNum,
			int blockSize, double playSampleRate);
		void prepareMIDIRecord();

	public:
		void readAudioData(juce::AudioBuffer<float>& buffer, int bufferOffset,
			int dataOffset, int length) const;
		void readMIDIData(juce::MidiBuffer& buffer, double baseTime,
			double startTime, double endTime, int trackIndex, double sampleRate) const;
		void writeAudioData(juce::AudioBuffer<float>& buffer, int offset,
			int trackChannelNum, int blockSize, double sampleRate);
		void writeMIDIData(const juce::MidiBuffer& buffer, int offset, int trackIndex, double sampleRate);

	private:
		const double recordInitLength = 30;
		juce::AudioSampleBuffer recordBuffer, recordBufferTemp;

		void updateAudioResampler(int blockSize, double sampleRate);

		void prepareAudioData(double length, int channelNum,
			int blockSize, double playSampleRate);
		void prepareMIDIData();
	};
	std::map<uint64_t, std::shared_ptr<SourceItem>> sources;

	double sampleRate = 0;
	int blockSize = 0;

	SourceItem* getSource(uint64_t ref, SourceType type) const;
	SourceItem* getSourceFast(uint64_t ref, SourceType type) const;

public:
	static SourceManager* getInstance();
	static void releaseInstance();

private:
	static SourceManager* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceManager)
};

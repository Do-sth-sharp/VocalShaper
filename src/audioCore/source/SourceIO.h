#pragma once

#include <JuceHeader.h>

class SourceIO final : public juce::Thread,
	private juce::DeletedAtShutdown {
public:
	SourceIO();
	~SourceIO();

	enum class TaskType {
		Read, Write
	};
	/** Type, SeqPtr, Path, GetTempo */
	using Task = std::tuple<TaskType, uint64_t, juce::String, bool>;
	void addTask(const Task& task);

protected:
	void run() override;

private:
	const juce::StringArray audioFormatsIn, midiFormatsIn;
	const juce::StringArray audioFormatsOut, midiFormatsOut;

	juce::CriticalSection lock;
	using TaskStruct = std::tuple<Task, juce::MidiMessageSequence>;
	std::queue<TaskStruct> list;

	static const juce::StringArray trimFormat(const juce::StringArray& list);
	static int getQualityForFormat(const juce::String& format);
	static int getBitDepthForFormat(const juce::String& format);

	static const std::tuple<double, juce::AudioSampleBuffer, int> loadAudio(const juce::File& file);
	static const std::tuple<bool, juce::MidiFile> loadMIDI(const juce::File& file);
	static bool saveAudio(const juce::File& file,
		double sampleRate, const juce::AudioSampleBuffer& buffer, int bitDepth, int quality);
	static bool saveMIDI(const juce::File& file, const juce::MidiFile& data);

	static const std::tuple<juce::MidiMessageSequence, juce::MidiFile>
		splitMIDI(const juce::MidiFile& data);
	static const juce::MidiFile mergeMIDI(const juce::MidiFile& data,
		const juce::MidiMessageSequence& timeSeq);
	static void copyMIDITimeFormat(juce::MidiFile& dst, const juce::MidiFile& src);

public:
	static SourceIO* getInstance();
	static void releaseInstance();

private:
	static SourceIO* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceIO)
};

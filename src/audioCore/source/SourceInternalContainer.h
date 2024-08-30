#pragma once

#include <JuceHeader.h>

class SourceInternalContainer {
public:
	enum class SourceType {
		Undefined, MIDI, Audio
	};

	SourceInternalContainer(const SourceType type, const juce::String& name);
	SourceInternalContainer(const SourceInternalContainer& other);

	SourceType getType() const;
	const juce::String getName() const;

	juce::MidiFile* getMidiData() const;
	juce::AudioSampleBuffer* getAudioData() const;
	double getAudioSampleRate() const;

	void changed();
	void saved();
	bool isSaved() const;

	const juce::String getFormat() const;
	const juce::StringPairArray getMetaData() const;
	int getBitsPerSample() const;
	int getQuality() const;

	void initMidiData();
	void initAudioData(int channelNum, double sampleRate, double length);

private:
	const SourceType type;
	const juce::String name;

	std::unique_ptr<juce::MidiFile> midiData = nullptr;
	std::unique_ptr<juce::AudioSampleBuffer> audioData = nullptr;
	double audioSampleRate = 0;
	std::atomic_bool savedFlag = true;

	juce::String format;
	juce::StringPairArray metaData;
	int bitsPerSample = 0;
	int quality = 0;

	void initAudioFormat();

	static const juce::String getForkName(const juce::String& name);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceInternalContainer)
};

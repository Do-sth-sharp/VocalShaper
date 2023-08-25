#pragma once

#include <JuceHeader.h>

namespace utils {
	/**
	 * @brief	Get the channel index and size of the audio bus.
	 */
	std::tuple<int, int> getChannelIndexAndNumOfBus(
		const juce::AudioProcessor* processor, int busIndex, bool isInput);

	void convertSecondsToTicks(juce::MidiFile& file);
	double convertSecondsToTicks(double time,
		const juce::MidiMessageSequence& tempoEvents,
		int timeFormat);
	double convertTicksToSeconds(double time,
		const juce::MidiMessageSequence& tempoEvents,
		int timeFormat);
	double convertSecondsToTicksWithObjectiveTempoTime(double time,
		const juce::MidiMessageSequence& tempoEvents,
		int timeFormat);
	double convertTicksToSecondsWithObjectiveTempoTime(double time,
		const juce::MidiMessageSequence& tempoEvents,
		int timeFormat);

	std::tuple<int, double> getBarBySecond(double time,
		const juce::MidiMessageSequence& tempoEvents);

	class AudioSaveConfig final : private juce::DeletedAtShutdown {
	public:
		AudioSaveConfig() = default;

		const int getBitsPerSample(const juce::String& format) const;
		const juce::StringPairArray getMetaData(const juce::String& format) const;
		const int getQualityOptionIndex(const juce::String& format) const;

		void setBitsPerSample(const juce::String& format, int value);
		void setMetaData(const juce::String& format,
			const juce::StringPairArray& data);
		void setQualityOptionIndex(const juce::String& format, int value);

	private:
		std::map<juce::String, int> bitsPerSample;
		std::map<juce::String, juce::StringPairArray> metaData;
		std::map<juce::String, int> qualityOptionIndex;
		juce::ReadWriteLock lock;

	public:
		static AudioSaveConfig* getInstance();

	private:
		static AudioSaveConfig* instance;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSaveConfig)
	};

	juce::AudioFormat* findAudioFormat(const juce::File& file);
	std::unique_ptr<juce::AudioFormatReader> createAudioReader(const juce::File& file);
	std::unique_ptr<juce::AudioFormatWriter> createAudioWriter(const juce::File& file,
		double sampleRateToUse, const juce::AudioChannelSet& channelLayout,
		int bitsPerSample, const juce::StringPairArray& metadataValues, int qualityOptionIndex);
}

#define UNUSED(var) (void)var

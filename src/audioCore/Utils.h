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

	juce::AudioFormat* findAudioFormat(const juce::File& file, bool isWrite);
	std::unique_ptr<juce::AudioFormatReader> createAudioReader(const juce::File& file);
	std::unique_ptr<juce::AudioFormatWriter> createAudioWriter(const juce::File& file,
		double sampleRateToUse, const juce::AudioChannelSet& channelLayout);

	enum class TrackType {
		DISABLED = 0,
		MONO = 10,
		STEREO = 20,
		LCR = 30,
		LRS = 31,
		LCRS = 40,
		SUR_5_0 = 50,
		SUR_5_1 = 51,
		SUR_5_0_2 = 502,
		SUR_5_1_2 = 512,
		SUR_5_0_4 = 504,
		SUR_5_1_4 = 514,
		SUR_6_0 = 60,
		SUR_6_1 = 61,
		SUR_6_0_M = 600,
		SUR_6_1_M = 610,
		SUR_7_0 = 70,
		SUR_7_0_SDSS = 700,
		SUR_7_1 = 71,
		SUR_7_1_SDSS = 710,
		SUR_7_0_2 = 702,
		SUR_7_1_2 = 712,
		SUR_7_0_4 = 704,
		SUR_7_1_4 = 714,
		SUR_7_0_6 = 706,
		SUR_7_1_6 = 716,
		SUR_9_0_4 = 904,
		SUR_9_1_4 = 914,
		SUR_9_0_6 = 906,
		SUR_9_1_6 = 916,
		QUADRAPHONIC = 4000,
		PENTAGONAL = 5000,
		HEXAGONAL = 6000,
		OCTAGONAL = 8000,
		AMBISONIC_0 = 100,
		AMBISONIC_1 = 101,
		AMBISONIC_2 = 102,
		AMBISONIC_3 = 103,
		AMBISONIC_4 = 104,
		AMBISONIC_5 = 105,
		AMBISONIC_6 = 106,
		AMBISONIC_7 = 107
	};

	const juce::AudioChannelSet getChannelSet(TrackType type);
	TrackType getTrackType(const juce::AudioChannelSet& channels);

	using Version = std::tuple<uint32_t, uint32_t, uint32_t>;
	int versionCompare(const Version& v1, const Version& v2);

	uint32_t getCurrentTime();
	juce::String getAudioPlatformName();
	Version getAudioPlatformVersion();
	Version getAudioPlatformVersionMinimumSupported();
	juce::String getAudioPlatformVersionString();
	juce::String getAudioPlatformCompileTime();
	juce::String getSystemNameAndVersion();
	juce::String getReleaseBranch();
	juce::String getReleaseName();
	juce::String createPlatformInfoString();
	juce::String getUserName();

	juce::String getJUCEVersion();
	juce::String getVST3SDKVersion();
	juce::String getVST2SDKVersion();
	juce::String getLV2Version();
	juce::String getDMDAVersion();

	juce::String getLegalFileName(const juce::String& name);
	juce::String getSourceDefaultPathForAudio(int id, const juce::String& name);
	juce::String getSourceDefaultPathForMIDI(int id, const juce::String& name);

	bool projectVersionHighEnough(const Version& version);
	bool projectVersionLowEnough(const Version& version);

	juce::File getProjectDir();
	bool setProjectDir(const juce::File& dir);
	juce::File getSourceFile(const juce::String& path);
	juce::File getDefaultWorkingDir();

	using AudioConnection = std::tuple<int, int, int, int>;
	using AudioConnectionList = juce::Array<AudioConnection>;

	using MidiConnection = std::tuple<int, int>;
	using MidiConnectionList = juce::Array<MidiConnection>;
}

#define UNUSED(var) (void)var

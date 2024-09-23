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

	struct AudioFormatInfo final {
		double sampleRate = 0;
		unsigned int bitsPerSample = 0;
		int64_t lengthInSamples = 0;
		unsigned int numChannels = 0;
		bool usesFloatingPointData = false;
		juce::StringPairArray metadataValues;
	};

	const juce::StringArray getAudioFormatsSupported(bool isWrite);
	const juce::StringArray getMidiFormatsSupported(bool isWrite);
	juce::AudioFormat* findAudioFormatForExtension(const juce::String& extension, bool isWrite);
	juce::AudioFormat* findAudioFormat(const juce::File& file, bool isWrite);
	std::unique_ptr<juce::AudioFormatReader> createAudioReader(const juce::File& file);
	std::unique_ptr<juce::AudioFormatWriter> createAudioWriter(const juce::File& file,
		double sampleRateToUse, const juce::AudioChannelSet& channelLayout,
		const juce::StringPairArray& metaData, int bitDepth, int quality);
	juce::StringArray getQualityOptionsForExtension(const juce::String& extension);
	juce::Array<int> getPossibleBitDepthsForExtension(const juce::String& extension);
	juce::Array<int> getPossibleSampleRatesForExtension(const juce::String& extension);
	juce::StringArray getPossibleMetaKeyForExtension(const juce::String& extension);
	int getBestQualityOptionIndexForExtension(const juce::String& extension);
	const AudioFormatInfo getAudioFormatData(const juce::File& file);

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
	const juce::Array<TrackType> getAllTrackTypes();
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
	juce::String getARASDKVersion();
	juce::String getLV2Version();
	juce::String getCompilerVersion();

	juce::String getLegalFileName(const juce::String& name);

	bool projectVersionHighEnough(const Version& version);
	bool projectVersionLowEnough(const Version& version);

	juce::File getProjectDir();
	bool setProjectDir(const juce::File& dir);
	juce::File getDefaultWorkingDir();

	const juce::StringArray getProjectFormatsSupported(bool isWrite);
	const juce::StringArray getPluginFormatsSupported();
	const juce::Array<double> getSourceSampleRateSupported();

	juce::Colour getDefaultColour();

	using AudioConnection = std::tuple<int, int, int, int>;
	using AudioConnectionList = juce::Array<AudioConnection>;

	using MidiConnection = std::tuple<int, int>;
	using MidiConnectionList = juce::Array<MidiConnection>;

	void bufferOutputResampledFixed(juce::AudioSampleBuffer& dst, const juce::AudioSampleBuffer& src,
		juce::AudioSampleBuffer& temp1, juce::AudioSampleBuffer& temp2,
		double resampleRatio, int channels, double dstSampleRate,
		int srcStart, int dstStart, int length);

	const juce::String getMIDICCChannelName(int channel);

	bool readFileToBlock(const juce::String& path, juce::MemoryBlock& block,
		const juce::File& base = utils::getProjectDir());
	bool writeBlockToFile(const juce::String& path, const juce::MemoryBlock& block,
		const juce::File& base = utils::getProjectDir());
}

#define UNUSED(var) (void)var

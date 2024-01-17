#pragma once

#include <JuceHeader.h>

namespace quickAPI {
	juce::Component* getAudioDebugger();
	juce::Component* getMidiDebugger();

	const juce::File getProjectDir();

	double getCPUUsage();
	bool getReturnToStartOnStop();
	bool getAnonymousMode();
	std::unique_ptr<juce::Component> createAudioDeviceSelector();

	std::tuple<int64_t, double> getTimeInBeat();
	double getTimeInSecond();
	const juce::Array<float> getAudioOutputLevel();
	bool isPlaying();
	bool isRecording();

	enum SourceType {
		UnknownSource, AudioSource, MIDISource, SynthSource
	};
	int getSourceNum();
	int getSourceId(int index);
	const juce::String getSourceName(int index);
	SourceType getSourceType(int index);
	const juce::String getSourceTypeName(int index);
	const juce::StringArray getAllSourceTypeName();
	double getSourceLength(int index);
	int getSourceChannelNum(int index);
	int getSourceTrackNum(int index);
	const juce::String getSourceSynthesizerName(int index);
	double getSourceSampleRate(int index);
	int getSourceEventNum(int index);
	const juce::StringArray getSourceNames();

	using TrackInfo = std::tuple<juce::String, juce::String>;
	const juce::Array<TrackInfo> getMixerTrackInfos();

	const juce::StringArray getFormatQualityOptionsForExtension(const juce::String& extension);
	const juce::Array<int> getFormatPossibleBitDepthsForExtension(const juce::String& extension);
	const juce::Array<int> getFormatPossibleSampleRatesForExtension(const juce::String& extension);
	const juce::StringArray getFormatPossibleMetaKeyForExtension(const juce::String& extension);

	const juce::StringArray getAudioFormatsSupported(bool isWrite);
	const juce::StringArray getMidiFormatsSupported(bool isWrite);
	const juce::StringArray getProjectFormatsSupported(bool isWrite);
	const juce::StringArray getPluginFormatsSupported(bool isWrite);

	int getFormatBitsPerSample(const juce::String& extension);
	const juce::StringPairArray getFormatMetaData(const juce::String& extension);
	int getFormatQualityOptionIndex(const juce::String& extension);

	const juce::Array<double> getSampleRateSupported();

	const juce::StringArray getPluginBlackList();
	const juce::StringArray getPluginSearchPath();

	int getSIMDLevel();
	const juce::String getSIMDInsName();
	const juce::StringArray getAllSIMDInsName();

	const std::tuple<bool, juce::Array<juce::PluginDescription>>
		getPluginList(bool filter = false, bool instr = true);
}
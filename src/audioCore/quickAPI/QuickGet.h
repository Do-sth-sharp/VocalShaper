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

	using TrackInfo = std::tuple<juce::String, juce::String>;
	const juce::StringArray getSourceNames();
	const juce::Array<TrackInfo> getMixerTrackInfos();

	const juce::StringArray getFormatQualityOptionsForExtension(const juce::String& extension);
	const juce::Array<int> getFormatPossibleBitDepthsForExtension(const juce::String& extension);
	const juce::Array<int> getFormatPossibleSampleRatesForExtension(const juce::String& extension);
	const juce::StringArray getFormatPossibleMetaKeyForExtension(const juce::String& extension);

	const juce::StringArray getAudioFormatsSupported(bool isWrite);
	const juce::StringArray getMidiFormatsSupported(bool isWrite);

	int getFormatBitsPerSample(const juce::String& extension);
	const juce::StringPairArray getFormatMetaData(const juce::String& extension);
	int getFormatQualityOptionIndex(const juce::String& extension);

	const juce::StringArray getPluginBlackList();
	const juce::StringArray getPluginSearchPath();
}
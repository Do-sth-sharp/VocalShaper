#pragma once
#include <JuceHeader.h>

class AudioConfig final : private juce::DeletedAtShutdown {
public:
	AudioConfig() = default;

	static const juce::String getPluginSearchPathListFilePath();
	static const juce::String getPluginListTemporaryFilePath();
	static const juce::String getPluginBlackListFilePath();
	static const juce::String getDeadPluginListPath();

	static void setPluginSearchPathListFilePath(const juce::String& path);
	static void setPluginListTemporaryFilePath(const juce::String& path);
	static void setPluginBlackListFilePath(const juce::String& path);
	static void setDeadPluginListPath(const juce::String& path);

	static const int getAudioSaveBitsPerSample();
	static const juce::StringPairArray getAudioSaveMetaData();
	static const int getAudioSaveQualityOptionIndex();

	static void setAudioSaveBitsPerSample(int value);
	static void setAudioSaveMetaData(const juce::StringPairArray& data);
	static void setAudioSaveQualityOptionIndex(int value);

private:
	juce::String pluginSearchPathListFilePath;
	juce::String pluginListTemporaryFilePath;
	juce::String pluginBlackListFilePath;
	juce::String deadPluginListPath;

	int audioSaveBitsPerSample = 24;
	juce::StringPairArray audioSaveMetaData;
	int audioSaveQualityOptionIndex = 0;

public:
	static AudioConfig* getInstance();

private:
	static AudioConfig* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConfig)
};

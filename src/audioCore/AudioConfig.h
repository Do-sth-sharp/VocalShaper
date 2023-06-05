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

private:
	juce::String pluginSearchPathListFilePath;
	juce::String pluginListTemporaryFilePath;
	juce::String pluginBlackListFilePath;
	juce::String deadPluginListPath;

public:
	static AudioConfig* getInstance();

private:
	static AudioConfig* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConfig)
};

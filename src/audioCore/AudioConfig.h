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

	static void setAnonymous(bool anonymous);
	static bool getAnonymous();

	static void setMidiTail(double time);
	static double getMidiTail();

private:
	juce::String pluginSearchPathListFilePath;
	juce::String pluginListTemporaryFilePath;
	juce::String pluginBlackListFilePath;
	juce::String deadPluginListPath;

	bool anonymous = false;
	std::atomic<double> midiTailTime = 2;

public:
	static AudioConfig* getInstance();

private:
	static AudioConfig* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConfig)
};

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

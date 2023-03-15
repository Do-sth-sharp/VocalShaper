#pragma once
#include <JuceHeader.h>

class AudioPluginSearchThread final : public juce::Thread {
public:
	AudioPluginSearchThread();
	~AudioPluginSearchThread();

	const juce::Array<juce::AudioPluginFormat*> getFormats() const;

	std::tuple<bool, juce::KnownPluginList&> getPluginList();
	void clearList();
	void clearTemporary();

	const juce::StringArray getBlackList() const;
	void addToBlackList(const juce::String& plugin) const;
	void removeFromBlackList(const juce::String& plugin) const;

	const juce::StringArray getSearchPath() const;
	void addToSearchPath(const juce::String& path) const;
	void removeFromSearchPath(const juce::String& path) const;

private:
	void run() override;

private:
	juce::KnownPluginList pluginList;
	std::atomic<bool> pluginListValidFlag = false;
	std::unique_ptr<juce::AudioPluginFormatManager> audioPluginManager = nullptr;

	void clearTemporaryInternal() const;
	const juce::File getBlackListFileInternal() const;
	void saveBlackListInternal(const juce::StringArray& blackList) const;
	const juce::File getSearchPathFileInternal() const;
	void saveSearchPathInternal(const juce::StringArray& paths) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginSearchThread)
};

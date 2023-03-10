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

private:
	void run() override;

private:
	juce::KnownPluginList pluginList;
	std::atomic<bool> pluginListValidFlag = false;
	std::unique_ptr<juce::AudioPluginFormatManager> audioPluginManager = nullptr;

	void clearTemporaryInternal();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginSearchThread)
};

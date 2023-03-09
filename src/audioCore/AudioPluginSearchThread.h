#pragma once
#include <JuceHeader.h>

class AudioPluginManagerHelper final : public juce::AudioPluginFormatManager {
	JUCE_DECLARE_WEAK_REFERENCEABLE(AudioPluginManagerHelper)
};

class AudioPluginSearchThread final : public juce::Thread {
public:
	AudioPluginSearchThread(juce::WeakReference<AudioPluginManagerHelper> manager);

	std::tuple<bool, juce::KnownPluginList&> getPluginList();
	void clearList();
	void clearTemporary();

private:
	void run() override;

private:
	const juce::WeakReference<AudioPluginManagerHelper> manager = nullptr;
	juce::KnownPluginList pluginList;
	std::atomic<bool> pluginListValidFlag = false;

	void clearTemporaryInternal();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginSearchThread)
};

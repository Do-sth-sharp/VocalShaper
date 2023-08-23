#pragma once

#include <JuceHeader.h>

class PluginLoadThread final : public juce::Thread {
public:
	PluginLoadThread();
	~PluginLoadThread() override;

	using PluginLoadCallback =
		std::function<void(std::unique_ptr<juce::AudioPluginInstance>)>;
	void load(const juce::PluginDescription& pluginInfo,
		const PluginLoadCallback& callback, double sampleRate, int blockSize);

private:
	void run() override;

private:
	using PluginLoadTask =
		std::tuple<juce::PluginDescription, PluginLoadCallback, double, int>;
	std::queue<PluginLoadTask> list;
	juce::CriticalSection lock;

	std::unique_ptr<juce::AudioPluginFormatManager> pluginFormatManager;
	std::unique_ptr<juce::MessageListener> messageHelper = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginLoadThread)
};

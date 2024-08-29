#pragma once

#include <JuceHeader.h>
#include "../graph/PluginDecorator.h"

class PluginLoadThread final : public juce::Thread {
public:
	PluginLoadThread();
	~PluginLoadThread() override;

	using Callback = std::function<void()>;
	using DstPointer = PluginDecorator::SafePointer;
	void load(const juce::PluginDescription& pluginInfo, bool addARA,
		DstPointer ptr, const Callback& callback, double sampleRate, int blockSize);

private:
	void run() override;

private:
	using PluginLoadTask =
		std::tuple<juce::PluginDescription, bool, DstPointer, double, int, Callback>;
	std::queue<PluginLoadTask> list;
	juce::CriticalSection lock;

	std::unique_ptr<juce::AudioPluginFormatManager> pluginFormatManager;
	std::unique_ptr<juce::MessageListener> messageHelper = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginLoadThread)
};

#pragma once

#include <JuceHeader.h>
#include "../graph/PluginDecorator.h"
#include "../source/CloneableSynthSource.h"

class PluginLoadThread final : public juce::Thread {
public:
	PluginLoadThread();
	~PluginLoadThread() override;

	using Callback = std::function<void()>;
	struct DstPointer {
		enum class Type { Plugin, Synth } type;
		PluginDecorator::SafePointer pluginPtr;
		CloneableSource::SafePointer<CloneableSynthSource> synthPtr;
	};
	void load(const juce::PluginDescription& pluginInfo,
		DstPointer ptr, const Callback& callback, double sampleRate, int blockSize);

private:
	void run() override;

private:
	using PluginLoadTask =
		std::tuple<juce::PluginDescription, DstPointer, double, int, Callback>;
	std::queue<PluginLoadTask> list;
	juce::CriticalSection lock;

	std::unique_ptr<juce::AudioPluginFormatManager> pluginFormatManager;
	std::unique_ptr<juce::MessageListener> messageHelper = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginLoadThread)
};

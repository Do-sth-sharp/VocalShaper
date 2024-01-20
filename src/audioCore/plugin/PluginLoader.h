#pragma once

#include <JuceHeader.h>
#include "PluginLoadThread.h"

class PluginLoader final : private juce::DeletedAtShutdown {
public:
	PluginLoader();
	~PluginLoader() override = default;

	using Callback = PluginLoadThread::Callback;
	void loadPlugin(const juce::PluginDescription& pluginInfo,
		PluginDecorator::SafePointer ptr,
		const Callback& callback = [] {});
	void loadPlugin(const juce::PluginDescription& pluginInfo,
		CloneableSource::SafePointer<> ptr,
		const Callback& callback = [] {});

	bool isRunning() const;

private:
	std::unique_ptr<PluginLoadThread> loadThread = nullptr;

public:
	static PluginLoader* getInstance();

private:
	static PluginLoader* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginLoader)
};
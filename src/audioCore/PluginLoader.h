#pragma once

#include <JuceHeader.h>
#include "PluginLoadThread.h"

class PluginLoader final : private juce::DeletedAtShutdown {
public:
	PluginLoader();
	~PluginLoader() override = default;

	using PluginLoadCallback = PluginLoadThread::PluginLoadCallback;
	void loadPlugin(const juce::PluginDescription& pluginInfo,
		const PluginLoadCallback& callback);

private:
	std::unique_ptr<juce::AudioPluginFormatManager> pluginFormatManager;

public:
	static PluginLoader* getInstance();

private:
	static PluginLoader* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginLoader)
};
#pragma once

#include <JuceHeader.h>

class PluginLoader final : public juce::DeletedAtShutdown {
public:
	PluginLoader() = default;
	~PluginLoader() override = default;

private:
	juce::OwnedArray<juce::AudioPluginInstance> pluginInstanceList;

public:
	static PluginLoader* getInstance();

private:
	static PluginLoader* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginLoader)
};
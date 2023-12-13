#pragma once

#include <JuceHeader.h>

class ConfigManager final : private juce::DeletedAtShutdown {
public:
	ConfigManager() = default;

	void clear();
	void loadConfigs();
	bool saveConfig(const juce::String& name) const;
	bool saveConfigs() const;

	juce::var& get(const juce::String& name);

private:
	std::map<juce::String, juce::var> confList;
	juce::var empty;

	static bool save(const juce::String& name, const juce::var& var);

public:
	static ConfigManager* getInstance();
	static void releaseInstance();

private:
	static ConfigManager* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigManager)
};

#pragma once

#include <JuceHeader.h>
#include "AudioPluginSearchThread.h"

class Plugin final : private juce::DeletedAtShutdown {
public:
	Plugin();

	const juce::StringArray getPluginTypeList() const;
	const std::tuple<bool, juce::KnownPluginList&> getPluginList() const;
	void clearPluginList();
	void clearPluginTemporary();
	bool pluginSearchThreadIsRunning() const;
	const std::unique_ptr<juce::PluginDescription> findPlugin(
		const juce::String& identifier, bool isInstrument = false) const;

	/**
	 * @attention	Don't change plugin black list while searching plugin.
	 */
	const juce::StringArray getPluginBlackList() const;
	/**
	 * @attention	Don't change plugin black list while searching plugin.
	 */
	void addToPluginBlackList(const juce::String& plugin) const;
	/**
	 * @attention	Don't change plugin black list while searching plugin.
	 */
	void removeFromPluginBlackList(const juce::String& plugin) const;

	/**
	 * @attention	Don't change plugin search path while searching plugin.
	 */
	const juce::StringArray getPluginSearchPath() const;
	/**
	 * @attention	Don't change plugin search path while searching plugin.
	 */
	void addToPluginSearchPath(const juce::String& path) const;
	/**
	 * @attention	Don't change plugin search path while searching plugin.
	 */
	void removeFromPluginSearchPath(const juce::String& path) const;

private:
	std::unique_ptr<AudioPluginSearchThread> audioPluginSearchThread = nullptr;

public:
	static Plugin* getInstance();
	static void releaseInstance();

private:
	static Plugin* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Plugin)
};

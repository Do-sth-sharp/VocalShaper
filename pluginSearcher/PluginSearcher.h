#pragma once

#include <JuceHeader.h>

class PluginSearcher final {
public:
	PluginSearcher() = delete;
	PluginSearcher(
		const juce::String& pluginSearchPathListFilePath,
		const juce::String& pluginListTemporaryFilePath,
		const juce::String& pluginBlackListFilePath,
		const juce::String& deadPluginListPath);

	void start();

private:
	const juce::String pluginSearchPathListFilePath;
	const juce::String pluginListTemporaryFilePath;
	const juce::String pluginBlackListFilePath;
	const juce::String deadPluginListPath;

	const juce::File getSearchPathFile() const;
	const juce::StringArray getSearchPath() const;
	const juce::File getBlackListFile() const;
	const juce::StringArray getBlackList() const;
	void saveBlackList(const juce::StringArray& list) const;
	const juce::File getTemporaryFile() const;
	void saveTemporaryFile(const juce::XmlElement* data) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginSearcher)
};

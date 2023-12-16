#pragma once

#include <JuceHeader.h>

namespace quickAPI {
	void setPluginSearchPathListFilePath(const juce::String& path);
	void setPluginListTemporaryFilePath(const juce::String& path);
	void setPluginBlackListFilePath(const juce::String& path);
	void setDeadPluginListPath(const juce::String& path);

	void setReturnToStartOnStop(bool value);
	void setAnonymousMode(bool value);

	void setFormatBitsPerSample(const juce::String& extension, int value);
	void setFormatMetaData(const juce::String& extension,
		const juce::StringPairArray& data);
	void setFormatQualityOptionIndex(const juce::String& extension, int value);
}
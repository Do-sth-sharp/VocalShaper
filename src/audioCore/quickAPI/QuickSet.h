#pragma once

#include <JuceHeader.h>

namespace quickAPI {
	void setPluginSearchPathListFilePath(const juce::String& path);
	void setPluginListTemporaryFilePath(const juce::String& path);
	void setPluginBlackListFilePath(const juce::String& path);
	void setDeadPluginListPath(const juce::String& path);
}
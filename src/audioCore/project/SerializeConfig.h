#pragma once

#include <JuceHeader.h>

struct SerializeConfig {
	juce::String projectFilePath;
	juce::String projectFileName;
	juce::String projectDir;
	juce::String araDir;
};

struct ParseConfig {
	juce::String projectFilePath;
	juce::String projectFileName;
	juce::String projectDir;
	juce::String araDir;
};

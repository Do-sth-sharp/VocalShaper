#pragma once

#include <JuceHeader.h>

namespace quickAPI {
	juce::Component* getAudioDebugger();
	juce::Component* getMidiDebugger();

	const juce::File getProjectDir();

	double getCPUUsage();
	bool getReturnToStartOnStop();
	std::unique_ptr<juce::Component> createAudioDeviceSelector();

	using TrackInfo = std::tuple<juce::String, juce::String>;
	const juce::StringArray getSourceNames();
	const juce::Array<TrackInfo> getMixerTrackInfos();
}
#pragma once

#include <JuceHeader.h>

namespace quickAPI {
	juce::Component* getAudioDebugger();
	juce::Component* getMidiDebugger();

	const juce::File getProjectDir();

	using TrackInfo = std::tuple<juce::String, juce::String>;
	const juce::StringArray getSourceNames();
	const juce::Array<TrackInfo> getMixerTrackInfos();
}
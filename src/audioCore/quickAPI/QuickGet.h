#pragma once

#include <JuceHeader.h>

namespace quickAPI {
	juce::Component* getAudioDebugger();
	juce::Component* getMidiDebugger();

	const juce::File getProjectDir();
}
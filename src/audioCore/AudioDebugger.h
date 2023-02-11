#pragma once

#include <JuceHeader.h>

class AudioCore;

class AudioDebugger : public juce::Component {
public:
	AudioDebugger() = delete;
	AudioDebugger(AudioCore* parent);

	void resized() override;
	void paint(juce::Graphics& g) override;

	void userTriedToCloseWindow() override;

private:
	AudioCore* const parent = nullptr;

	std::unique_ptr<juce::AudioDeviceSelectorComponent> deviceSelector = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioDebugger)
};

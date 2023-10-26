#pragma once

#include <JuceHeader.h>

class AudioCore;

class AudioDebugger final : public juce::Component {
public:
	AudioDebugger();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void userTriedToCloseWindow() override;

	void output(const juce::String& mes);

private:
	std::unique_ptr<juce::AudioDeviceSelectorComponent> deviceSelector = nullptr;
	std::unique_ptr<juce::TextEditor> commandInput = nullptr, commandOutput = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioDebugger)
};

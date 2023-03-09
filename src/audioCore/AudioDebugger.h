#pragma once

#include <JuceHeader.h>

class AudioCore;
class AudioCommand;

class AudioDebugger final : public juce::Component {
public:
	AudioDebugger() = delete;
	AudioDebugger(AudioCore* parent, AudioCommand* commandProcessor);

	void resized() override;
	void paint(juce::Graphics& g) override;

	void userTriedToCloseWindow() override;

private:
	AudioCore* const parent = nullptr;
	AudioCommand* const commandProcessor = nullptr;

	std::unique_ptr<juce::AudioDeviceSelectorComponent> deviceSelector = nullptr;
	std::unique_ptr<juce::TextEditor> commandInput = nullptr, commandOutput = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioDebugger)
};

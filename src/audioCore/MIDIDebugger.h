#pragma once

#include <JuceHeader.h>

class MIDIDebugger final : public juce::Component {
public:
	MIDIDebugger();

	void resized() override;

	void addMessage(const juce::MidiMessage& message);

private:
	std::unique_ptr<juce::TextEditor> messageOutput = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIDebugger)
};

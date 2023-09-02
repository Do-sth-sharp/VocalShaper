#pragma once

#include <JuceHeader.h>

class MIDIDebugger final : public juce::Component {
public:
	MIDIDebugger();

	void resized() override;

	void addMessage(const juce::MidiMessage& message, bool isInput = true);

	void setMaxNum(int num = 30);
	int getMaxNum() const;

private:
	std::unique_ptr<juce::TextEditor> messageOutput = nullptr;
	int maxNum = 30;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIDebugger)
};

#pragma once

#include <JuceHeader.h>

class MIDIDebugger final : public juce::AnimatedAppComponent {
public:
	MIDIDebugger();

	void update() override;
	void paint(juce::Graphics& g) override;

	void addMessage(const juce::MidiMessage& message, bool isInput = true);

	void setMaxNum(int num = 30);
	int getMaxNum() const;

private:
	std::list<juce::String> mesList;
	int maxNum = 30;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIDebugger)
};

#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>

class MidiDebuggerComponent final : public flowUI::FlowComponent {
public:
	MidiDebuggerComponent();

	void resized() override;

private:
	juce::Component* comp = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiDebuggerComponent)
};

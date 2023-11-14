#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>

class AudioDebuggerComponent final : public flowUI::FlowComponent {
public:
	AudioDebuggerComponent();

	void resized() override;

private:
	juce::Component* comp = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioDebuggerComponent)
};

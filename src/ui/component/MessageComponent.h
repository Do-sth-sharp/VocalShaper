#pragma once

#include <JuceHeader.h>

class MessageComponent final
	: public juce::Component,
	public juce::ChangeListener {
public:
	MessageComponent();
	~MessageComponent();

	void paint(juce::Graphics& g) override;

private:
	void changeListenerCallback(juce::ChangeBroadcaster*) override;

	void mouseUp(const juce::MouseEvent& event) override;

private:
	std::unique_ptr<juce::Drawable> mesIcon = nullptr;
	bool showNoticeDot = false;
	juce::String mes, emptyMes;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageComponent)
};

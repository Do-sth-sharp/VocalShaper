#pragma once

#include <JuceHeader.h>

class MessageViewer final 
	: public juce::Component,
	public juce::ChangeListener {
public:
	MessageViewer();
	~MessageViewer();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update();
	void clear();

private:
	std::unique_ptr<juce::DrawableButton> clearButton = nullptr;
	std::unique_ptr<juce::Drawable> clearIcon = nullptr;
	std::unique_ptr<juce::Viewport> messageViewPort = nullptr;

	juce::Rectangle<int> boundsTemp;

	void changeListenerCallback(juce::ChangeBroadcaster*) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageViewer)
};

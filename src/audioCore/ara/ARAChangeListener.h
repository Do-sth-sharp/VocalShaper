#pragma once

#include <JuceHeader.h>

class ARAVirtualDocument;

class ARAChangeListener : public juce::ChangeListener {
public:
	ARAChangeListener(ARAVirtualDocument* document);

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
	ARAVirtualDocument* const document;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAChangeListener)
};

class ARARegionChangeListener : public juce::ChangeListener {
public:
	ARARegionChangeListener(ARAVirtualDocument* document);

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
	ARAVirtualDocument* const document;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARARegionChangeListener)
};

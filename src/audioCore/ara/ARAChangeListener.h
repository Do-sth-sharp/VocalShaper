#pragma once

#include <JuceHeader.h>

class ARAVirtualDocument;

class ARARegionChangeListener : public juce::ChangeListener {
public:
	ARARegionChangeListener(ARAVirtualDocument* document);

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
	ARAVirtualDocument* const document;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARARegionChangeListener)
};

class ARAContextChangeListener : public juce::ChangeListener {
public:
	ARAContextChangeListener(ARAVirtualDocument* document);

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
	ARAVirtualDocument* const document;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAContextChangeListener)
};

class ARATrackInfoChangeListener : public juce::ChangeListener {
public:
	ARATrackInfoChangeListener(ARAVirtualDocument* document);

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
	ARAVirtualDocument* const document;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARATrackInfoChangeListener)
};

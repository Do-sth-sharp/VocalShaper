#pragma once

#include <JuceHeader.h>

class ARAVirtualDocument {
public:
	ARAVirtualDocument() = delete;
	ARAVirtualDocument(ARA::Host::DocumentController* controller);

private:
	ARA::Host::DocumentController* const controller = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualDocument)
};

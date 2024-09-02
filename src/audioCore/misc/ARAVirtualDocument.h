#pragma once

#include <JuceHeader.h>

class SeqSourceProcessor;

class ARAVirtualDocument {
public:
	ARAVirtualDocument() = delete;
	ARAVirtualDocument(SeqSourceProcessor* seq,
		ARA::Host::DocumentController* controller);

private:
	SeqSourceProcessor* const seq = nullptr;
	ARA::Host::DocumentController* const controller = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualDocument)
};

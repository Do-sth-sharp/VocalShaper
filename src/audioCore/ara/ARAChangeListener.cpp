#include "ARAChangeListener.h"
#include "ARAVirtualDocument.h"

ARAChangeListener::ARAChangeListener(ARAVirtualDocument* document)
	: document(document) {}

void ARAChangeListener::changeListenerCallback(juce::ChangeBroadcaster* source) {
	this->document->update();
}

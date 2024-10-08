#include "ARAChangeListener.h"
#include "ARAVirtualDocument.h"

ARARegionChangeListener::ARARegionChangeListener(ARAVirtualDocument* document)
	: document(document) {}

void ARARegionChangeListener::changeListenerCallback(juce::ChangeBroadcaster* source) {
	this->document->updateRegions();
}

ARAContextChangeListener::ARAContextChangeListener(ARAVirtualDocument* document)
	: document(document) {}

void ARAContextChangeListener::changeListenerCallback(juce::ChangeBroadcaster* source) {
	this->document->updateAudioAndContext();
}

ARATrackInfoChangeListener::ARATrackInfoChangeListener(ARAVirtualDocument* document)
	: document(document) {}

void ARATrackInfoChangeListener::changeListenerCallback(juce::ChangeBroadcaster* source) {
	this->document->updateTrackBaseInfo();
}

#include "SourceMIDITemp.h"

void SourceMIDITemp::setData(const juce::MidiFile& data) {
	this->sourceData = data;

	this->update();
}

void SourceMIDITemp::addTrack(const juce::MidiMessageSequence& track) {
	this->sourceData.addTrack(track);

	this->update();
}

void SourceMIDITemp::update() {
	/** For Each Track */
	for (int i = 0; i < this->sourceData.getNumTracks(); i++) {
		auto track = this->sourceData.getTrack(i);
		double endTime = track->getEndTime();

		/** For Each Event */
		juce::Array<Note> noteTrack;
		juce::String lyricsTemp;
		double lyricsTimeTemp = 0;
		for (int i = 0; i < track->getNumEvents(); i++) {
			auto event = track->getEventPointer(i);

			/** Get Notes */
			if (event->message.isNoteOn(true)) {
				Note note;
				note.startSec = event->message.getTimeStamp();
				note.endSec = event->noteOffObject ? event->noteOffObject->message.getTimeStamp() : endTime;
				note.pitch = event->message.getNoteNumber();
				note.vel = event->message.getVelocity();
				if (note.startSec == lyricsTimeTemp) {
					note.lyrics = lyricsTemp;
				}
				note.noteOnEvent = i;

				noteTrack.add(note);
				continue;
			}
			/** Get Lyrics */
			if (event->message.isMetaEvent() && event->message.getMetaEventType() == 0x05) {
				lyricsTemp = event->message.getTextFromTextMetaEvent();
				lyricsTimeTemp = event->message.getTimeStamp();
				continue;
			}
			/** TODO */
		}

		this->noteList.add(noteTrack);
	}
}

juce::MidiFile* SourceMIDITemp::getSourceData() {
	return &(this->sourceData);
}

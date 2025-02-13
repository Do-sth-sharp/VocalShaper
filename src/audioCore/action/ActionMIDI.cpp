#include "ActionMIDI.h"

#include "../source/SourceManager.h"

ActionMIDIAddNote::ActionMIDIAddNote(
	uint64_t ref, int track, double startTime, double endTime, uint8_t channel,
	uint8_t pitch, uint8_t vel, const juce::String& lyrics)
	: ref(ref), track(track), startTime(startTime), endTime(endTime),
	channel(channel), pitch(pitch), vel(vel), lyrics(lyrics) {}

bool ActionMIDIAddNote::doAction() {
	this->index = SourceManager::getInstance()->addNote(
		this->ref, this->track, this->startTime, this->endTime,
		this->channel, this->pitch, this->vel, this->lyrics);
	return (this->index >= 0);
}

bool ActionMIDIAddNote::undoAction() {
	return SourceManager::getInstance()->removeNote(
		this->ref, this->track, this->index);
}

const juce::String ActionMIDIAddNote::getStatusStr() const {
	return "Source: " + juce::String{ this->ref } + "\n" +
		"Track: " + juce::String{ this->track } + "\n" +
		"Index: " + juce::String{ this->index } + "\n" +
		"Time: " + juce::String{ this->startTime, 2 } + " - " + juce::String{ this->endTime, 2 } + "\n" +
		"Channel: " + juce::String{ this->channel } + "\n" +
		"Pitch: " + juce::String{ this->pitch } + "\n" +
		"Velocity: " + juce::String{ this->vel } + "\n" +
		"Lyrics: " + this->lyrics + "\n";
}

void ActionMIDIAddNote::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt64((int64_t)this->ref);
	stream.writeInt(this->track);
	stream.writeInt(this->index);
	stream.writeDouble(this->startTime);
	stream.writeDouble(this->endTime);
	stream.writeByte((char)this->channel);
	stream.writeByte((char)this->pitch);
	stream.writeByte((char)this->vel);
	stream.writeString(this->lyrics);
}

ActionMIDISetNoteTime::ActionMIDISetNoteTime(
	uint64_t ref, int track, int index,
	double startTime, double endTime)
	: ref(ref), track(track), index(index),
	startTime(startTime), endTime(endTime) {}

bool ActionMIDISetNoteTime::doAction() {
	/** Get Old Index */
	auto note = SourceManager::getInstance()->getMIDINote(
		this->ref, this->track, this->index);
	if (note.eventOffIndex < 0) { return false; }
	this->oldEndIndex = note.eventOffIndex;
	this->oldStartTime = note.timeSec;
	this->oldEndTime = note.endSec;

	/** Set Time */
	this->newIndex = SourceManager::getInstance()->setNoteTime(
		this->ref, this->track, this->index,
		this->startTime, this->endTime);
	if (this->newIndex < 0) { return false; }

	return true;
}

bool ActionMIDISetNoteTime::undoAction() {
	int index = SourceManager::getInstance()->setNoteTime(
		this->ref, this->track, this->newIndex,
		this->oldStartTime, this->oldEndTime,
		this->index, this->oldEndIndex);
	return (index == this->index);
}

const juce::String ActionMIDISetNoteTime::getStatusStr() const {
	return "Source: " + juce::String{ this->ref } + "\n" +
		"Track: " + juce::String{ this->track } + "\n" +
		"Index: " + juce::String{ this->index } + "\n" +
		"Old Time: " + juce::String{ this->oldStartTime, 2 } + " - " + juce::String{ this->oldEndTime, 2 } + "\n" +
		"Time: " + juce::String{ this->startTime, 2 } + " - " + juce::String{ this->endTime, 2 } + "\n" +
		"New Index: " + juce::String{ this->newIndex } + "\n";
}

void ActionMIDISetNoteTime::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt64((int64_t)this->ref);
	stream.writeInt(this->track);
	stream.writeInt(this->index);
	stream.writeDouble(this->startTime);
	stream.writeDouble(this->endTime);

	stream.writeInt(this->oldEndIndex);
	stream.writeInt(this->newIndex);
	stream.writeDouble(this->oldStartTime);
	stream.writeDouble(this->oldEndTime);
}

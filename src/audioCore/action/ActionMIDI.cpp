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

ActionMIDISetNoteChannel::ActionMIDISetNoteChannel(
	uint64_t ref, int track, int index,
	uint8_t channel)
	: ref(ref), track(track), index(index), channel(channel) {}

bool ActionMIDISetNoteChannel::doAction() {
	/** Get Old Channel */
	auto note = SourceManager::getInstance()->getMIDINote(
		this->ref, this->track, this->index);
	if (note.eventOffIndex < 0) { return false; }
	this->oldChannel = note.channel;

	/** Set Channel */
	return SourceManager::getInstance()->setNoteChannel(
		this->ref, this->track, this->index, this->channel);
}

bool ActionMIDISetNoteChannel::undoAction() {
	return SourceManager::getInstance()->setNoteChannel(
		this->ref, this->track, this->index, this->oldChannel);
}

const juce::String ActionMIDISetNoteChannel::getStatusStr() const {
	return "Source: " + juce::String{ this->ref } + "\n" +
		"Track: " + juce::String{ this->track } + "\n" +
		"Index: " + juce::String{ this->index } + "\n" +
		"Old Channel: " + juce::String{ this->oldChannel } + "\n" +
		"Channel: " + juce::String{ this->channel } + "\n";
}

void ActionMIDISetNoteChannel::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt64((int64_t)this->ref);
	stream.writeInt(this->track);
	stream.writeInt(this->index);
	stream.writeByte((char)this->channel);
	stream.writeByte((char)this->oldChannel);
}

ActionMIDISetNotePitch::ActionMIDISetNotePitch(
	uint64_t ref, int track, int index,
	uint8_t pitch)
	: ref(ref), track(track), index(index), pitch(pitch) {}

bool ActionMIDISetNotePitch::doAction() {
	/** Get Old Pitch */
	auto note = SourceManager::getInstance()->getMIDINote(
		this->ref, this->track, this->index);
	if (note.eventOffIndex < 0) { return false; }
	this->oldPitch = note.pitch;

	/** Set Pitch */
	return SourceManager::getInstance()->setNotePitch(
		this->ref, this->track, this->index, this->pitch);
}

bool ActionMIDISetNotePitch::undoAction() {
	return SourceManager::getInstance()->setNotePitch(
		this->ref, this->track, this->index, this->oldPitch);
}

const juce::String ActionMIDISetNotePitch::getStatusStr() const {
	return "Source: " + juce::String{ this->ref } + "\n" +
		"Track: " + juce::String{ this->track } + "\n" +
		"Index: " + juce::String{ this->index } + "\n" +
		"Old Pitch: " + juce::String{ this->oldPitch } + "\n" +
		"Pitch: " + juce::String{ this->pitch } + "\n";
}

void ActionMIDISetNotePitch::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt64((int64_t)this->ref);
	stream.writeInt(this->track);
	stream.writeInt(this->index);
	stream.writeByte((char)this->pitch);
	stream.writeByte((char)this->oldPitch);
}

ActionMIDISetNoteVelocity::ActionMIDISetNoteVelocity(
	uint64_t ref, int track, int index,
	uint8_t velocity)
	: ref(ref), track(track), index(index), velocity(velocity) {}

bool ActionMIDISetNoteVelocity::doAction() {
	/** Get Old Velocity */
	auto note = SourceManager::getInstance()->getMIDINote(
		this->ref, this->track, this->index);
	if (note.eventOffIndex < 0) { return false; }
	this->oldVelocity = note.vel;

	/** Set Velocity */
	return SourceManager::getInstance()->setNoteVelocity(
		this->ref, this->track, this->index, this->velocity);
}

bool ActionMIDISetNoteVelocity::undoAction() {
	return SourceManager::getInstance()->setNoteVelocity(
		this->ref, this->track, this->index, this->oldVelocity);
}

const juce::String ActionMIDISetNoteVelocity::getStatusStr() const {
	return "Source: " + juce::String{ this->ref } + "\n" +
		"Track: " + juce::String{ this->track } + "\n" +
		"Index: " + juce::String{ this->index } + "\n" +
		"Old Velocity: " + juce::String{ this->oldVelocity } + "\n" +
		"Velocity: " + juce::String{ this->velocity } + "\n";
}

void ActionMIDISetNoteVelocity::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt64((int64_t)this->ref);
	stream.writeInt(this->track);
	stream.writeInt(this->index);
	stream.writeByte((char)this->velocity);
	stream.writeByte((char)this->oldVelocity);
}

ActionMIDISetNoteLyrics::ActionMIDISetNoteLyrics(
	uint64_t ref, int track, int index,
	const juce::String& lyrics)
	: ref(ref), track(track), index(index), lyrics(lyrics) {}

bool ActionMIDISetNoteLyrics::doAction() {
	/** Get Old Lyrics */
	auto note = SourceManager::getInstance()->getMIDINote(
		this->ref, this->track, this->index);
	if (note.eventOffIndex < 0) { return false; }
	this->oldLyrics = note.lyrics;

	/** Set Lyrics */
	return SourceManager::getInstance()->setNoteLyrics(
		this->ref, this->track, this->index, this->lyrics);
}

bool ActionMIDISetNoteLyrics::undoAction() {
	return SourceManager::getInstance()->setNoteLyrics(
		this->ref, this->track, this->index, this->oldLyrics);
}

const juce::String ActionMIDISetNoteLyrics::getStatusStr() const {
	return "Source: " + juce::String{ this->ref } + "\n" +
		"Track: " + juce::String{ this->track } + "\n" +
		"Index: " + juce::String{ this->index } + "\n" +
		"Old Lyrics: " + this->oldLyrics + "\n" +
		"Lyrics: " + this->lyrics + "\n";
}

void ActionMIDISetNoteLyrics::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt64((int64_t)this->ref);
	stream.writeInt(this->track);
	stream.writeInt(this->index);
	stream.writeString(this->lyrics);
	stream.writeString(this->oldLyrics);
}

ActionMIDIRemoveNote::ActionMIDIRemoveNote(
	uint64_t ref, int track, int index)
	: ref(ref), track(track), index(index) {}

bool ActionMIDIRemoveNote::doAction() {
	/** Get Note Data */
	auto note = SourceManager::getInstance()->getMIDINote(
		this->ref, this->track, this->index);
	if (note.eventOffIndex < 0) { return false; }

	this->startTime = note.timeSec;
	this->endTime = note.endSec;
	this->channel = note.channel;
	this->pitch = note.pitch;
	this->vel = note.vel;
	this->lyrics = note.lyrics;

	this->oldEndIndex = note.eventOffIndex;

	/** Remove Note */
	return SourceManager::getInstance()->removeNote(
		this->ref, this->track, this->index);
}

bool ActionMIDIRemoveNote::undoAction() {
	/** Restore Note */
	int index = SourceManager::getInstance()->addNote(
		this->ref, this->track, this->startTime, this->endTime,
		this->channel, this->pitch, this->vel, this->lyrics,
		this->index, this->oldEndIndex);
	return (index == this->index);
}

const juce::String ActionMIDIRemoveNote::getStatusStr() const {
	return "Source: " + juce::String{ this->ref } + "\n" +
		"Track: " + juce::String{ this->track } + "\n" +
		"Index: " + juce::String{ this->index } + "\n" +
		"Time: " + juce::String{ this->startTime, 2 } + " - " + juce::String{ this->endTime, 2 } + "\n" +
		"Channel: " + juce::String{ this->channel } + "\n" +
		"Pitch: " + juce::String{ this->pitch } + "\n" +
		"Velocity: " + juce::String{ this->vel } + "\n" +
		"Lyrics: " + this->lyrics + "\n";
}

void ActionMIDIRemoveNote::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt64((int64_t)this->ref);
	stream.writeInt(this->track);
	stream.writeInt(this->index);
	stream.writeDouble(this->startTime);
	stream.writeDouble(this->endTime);
	stream.writeByte((char)this->channel);
	stream.writeByte((char)this->pitch);
	stream.writeByte((char)this->vel);
	stream.writeString(this->lyrics);

	stream.writeInt(this->oldEndIndex);
}

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

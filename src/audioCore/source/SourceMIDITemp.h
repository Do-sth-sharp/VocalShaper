#pragma once

#include <JuceHeader.h>

class SourceMIDITemp final {
public:
	SourceMIDITemp() = default;

	void setData(const juce::MidiFile& data);
	void addTrack(const juce::MidiMessageSequence& track);
	void update();

	juce::MidiFile* getSourceData();
	
private:
	juce::MidiFile sourceData;

	struct Note {
		double startSec, endSec;
		uint8_t pitch;
		uint8_t vel;
		juce::String lyrics;

		int noteOnEvent;
	};
	juce::Array<juce::Array<Note>> noteList;
};

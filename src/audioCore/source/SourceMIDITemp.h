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
		uint8_t channel;
		double startSec, endSec;
		uint8_t pitch;
		uint8_t vel;
		juce::String lyrics;

		int noteOnEvent;
	};
	juce::Array<juce::Array<Note>> noteList;

	struct Pedal {
		uint8_t channel;
		double timeSec;
		bool value;

		int event;
	};
	juce::Array<juce::Array<Pedal>> sustainPedalList;
	juce::Array<juce::Array<Pedal>> sostenutoPedalList;
	juce::Array<juce::Array<Pedal>> softPedalList;

	struct IntParam {
		uint8_t channel;
		double timeSec;
		int value;

		int event;
	};
	juce::Array<juce::Array<IntParam>> pitchWheelList;
	juce::Array<juce::Array<IntParam>> afterTouchList;
	juce::Array<juce::Array<IntParam>> channelPressureList;

	struct Controller {
		uint8_t channel;
		double timeSec;
		uint8_t number;
		uint8_t value;

		int event;
	};
	juce::Array<std::unordered_map<uint8_t, juce::Array<Controller>>> controllerList;

	struct Misc {
		uint8_t channel;
		double timeSec;

		juce::MidiMessage message;

		int event;
	};
	juce::Array<juce::Array<Misc>> miscList;
};

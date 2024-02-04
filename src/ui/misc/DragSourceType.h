#pragma once

enum class DragSourceType {
	Plugin = 0x1000,
	Source,
	SourceSynth,
	InstrInput,
	InstrOutput,
	TrackMidiInput,
	TrackAudioInput,
	TrackMidiOutput,
	TrackAudioOutput
};

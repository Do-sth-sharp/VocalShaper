#pragma once

enum class DragSourceType {
	Plugin = 0x1000,
	Source,
	SourceSynth,
	TrackMidiInput,
	TrackAudioInput,
	TrackMidiOutput,
	TrackAudioOutput,
	Effect
};

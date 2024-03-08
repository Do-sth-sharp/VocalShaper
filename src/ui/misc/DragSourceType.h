#pragma once

enum class DragSourceType {
	Plugin = 0x1000,
	TrackMidiInput,
	TrackAudioInput,
	TrackMidiOutput,
	TrackAudioOutput,
	Effect
};

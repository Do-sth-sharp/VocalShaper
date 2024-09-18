#pragma once

#include <JuceHeader.h>

namespace ARAExtension {

	enum ARAContentType : uint32_t {
		ARAContentTypeUnknown = 0,

		ARAContentTypeNote = ARA::kARAContentTypeNotes,
		ARAContentTypeTempoEntry = ARA::kARAContentTypeTempoEntries,
		ARAContentTypeBarSignature = ARA::kARAContentTypeBarSignatures,
		ARAContentTypeStaticTuning = ARA::kARAContentTypeStaticTuning,
		ARAContentTypeKeySignature = ARA::kARAContentTypeKeySignatures,
		ARAContentTypeSheetChord = ARA::kARAContentTypeSheetChords,

		ARAContentTypeNotePlus = 0x8000,
		ARAContentTypeSustainPedal,
		ARAContentTypeSostenutoPedal,
		ARAContentTypeSoftPedal,
		ARAContentTypePitchWheel,
		ARAContentTypeAfterTouch,
		ARAContentTypeChannelPressure,
		ARAContentTypeController,
		ARAContentTypeMisc
	};

	struct ARAContentNote {
		uint8_t channel;
		double startSec, endSec;
		uint8_t pitch;
		uint8_t vel;
		juce::String lyrics;
	};

	struct ARAContentPedal {
		uint8_t channel;
		double timeSec;
		bool pedalOn;
	};

	struct ARAContentIntParam {
		uint8_t channel;
		double timeSec;
		int value;
	};

	struct ARAContentAfterTouch {
		uint8_t channel;
		double timeSec;
		uint8_t notePitch;
		uint8_t value;
	};

	struct ARAContentController {
		uint8_t channel;
		double timeSec;
		uint8_t number;
		uint8_t value;
	};

	struct ARAContentMisc {
		uint8_t channel;
		double timeSec;
		juce::MidiMessage message;
	};
}

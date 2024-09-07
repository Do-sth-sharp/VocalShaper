#pragma once

#include <JuceHeader.h>

class SeqSourceProcessor;

enum class ARAVirtualMidiEventType {
	Unknown, Note, TempoEntry, BarSignature, KeySignature
};

class ARAVirtualAudioSource {
public:
	ARAVirtualAudioSource() = delete;
	ARAVirtualAudioSource(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

	void update();
	bool readAudioSamples(
		float* const* buffers, int64_t startSample, int64_t numSamples) const;

	using Converter = juce::ARAHostModel::ConversionFunctions<ARAVirtualAudioSource*, ARA::ARAAudioSourceHostRef>;

	using MidiEventType = ARAVirtualMidiEventType;

public:
	int32_t getGlobalMidiEventCount();
	MidiEventType getGlobalMidiEventType(int32_t index);
	ARA::ARAContentTempoEntry getGlobalTempoEvent(int32_t index);
	ARA::ARAContentBarSignature getGlobalBarEvent(int32_t index);
	ARA::ARAContentKeySignature getGlobalKeyEvent(int32_t index);

private:
	SeqSourceProcessor* const seq = nullptr;

	juce::ARAHostModel::AudioSource audioSource;

	static const ARA::ARAAudioSourceProperties createProperties(SeqSourceProcessor* seq);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualAudioSource)
};

class ARAVirtualMusicalContext {
public:
	ARAVirtualMusicalContext() = delete;
	ARAVirtualMusicalContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

	void update();

	using Converter = juce::ARAHostModel::ConversionFunctions<ARAVirtualMusicalContext*, ARA::ARAMusicalContextHostRef>;

	using MidiEventType = ARAVirtualMidiEventType;

public:
	int32_t getGlobalMidiEventCount();
	MidiEventType getGlobalMidiEventType(int32_t index);
	ARA::ARAContentTempoEntry getGlobalTempoEvent(int32_t index);
	ARA::ARAContentBarSignature getGlobalBarEvent(int32_t index);
	ARA::ARAContentKeySignature getGlobalKeyEvent(int32_t index);

private:
	const int32_t id = 0;
	SeqSourceProcessor* const seq = nullptr;

	juce::ARAHostModel::MusicalContext musicalContext;

	ARA::ARAColor color;

	static int32_t contextIdCounter;

	static const ARA::ARAMusicalContextProperties createProperties(
		SeqSourceProcessor* seq, int32_t id, ARA::ARAColor* color);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualMusicalContext)
};

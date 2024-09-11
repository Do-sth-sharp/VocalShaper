#pragma once

#include <JuceHeader.h>
#include "ARASharedObject.h"

class SeqSourceProcessor;

enum class ARAVirtualMusicalContextType {
	Unknown, Global, Note, NotePlus,
	SustainPedal, SostenutoPedal, SoftPedal,
	PitchWheel, AfterTouch, ChannelPressure,
	Controller, Misc
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

	using MidiEventType = ARAExtension::ARAContentType;

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
		SeqSourceProcessor* seq,
		ARAVirtualMusicalContextType type);

	void update();

	ARAVirtualMusicalContextType getType() const;

	using Converter = juce::ARAHostModel::ConversionFunctions<ARAVirtualMusicalContext*, ARA::ARAMusicalContextHostRef>;
	using MidiEventType = ARAExtension::ARAContentType;

protected:
	SeqSourceProcessor* getSeq() const;

private:
	SeqSourceProcessor* const seq = nullptr;
	const ARAVirtualMusicalContextType type = ARAVirtualMusicalContextType::Unknown;

	juce::ARAHostModel::MusicalContext musicalContext;

	ARA::ARAColor color;

	static const ARA::ARAMusicalContextProperties createProperties(
		SeqSourceProcessor* seq, ARA::ARAColor* color, ARAVirtualMusicalContextType type);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualMusicalContext)
};

class ARAVirtualGlobalContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualGlobalContext() = delete;
	ARAVirtualGlobalContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getEventCount();
	MidiEventType getEventType(int32_t index);
	ARA::ARAContentTempoEntry getTempo(int32_t index);
	ARA::ARAContentBarSignature getBar(int32_t index);
	ARA::ARAContentKeySignature getKey(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualGlobalContext)
};

class ARAVirtualNoteContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualNoteContext() = delete;
	ARAVirtualNoteContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getNoteCount();
	ARA::ARAContentNote getNote(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualNoteContext)
};

class ARAVirtualNotePlusContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualNotePlusContext() = delete;
	ARAVirtualNotePlusContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getNoteCount();
	ARAExtension::ARAContentNote getNote(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualNotePlusContext)
};

class ARAVirtualSustainPedalContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualSustainPedalContext() = delete;
	ARAVirtualSustainPedalContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getSustainPedalCount();
	ARAExtension::ARAContentPedal getSustainPedal(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualSustainPedalContext)
};

class ARAVirtualSostenutoPedalContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualSostenutoPedalContext() = delete;
	ARAVirtualSostenutoPedalContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getSostenutoPedalCount();
	ARAExtension::ARAContentPedal getSostenutoPedal(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualSostenutoPedalContext)
};

class ARAVirtualSoftPedalContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualSoftPedalContext() = delete;
	ARAVirtualSoftPedalContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getSoftPedalCount();
	ARAExtension::ARAContentPedal getSoftPedal(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualSoftPedalContext)
};

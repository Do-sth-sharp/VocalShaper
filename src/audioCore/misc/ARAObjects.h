#pragma once

#include <JuceHeader.h>
#include "ARASharedObject.h"

class SeqSourceProcessor;

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
		ARAExtension::ARAContentType type);

	void update();

	ARAExtension::ARAContentType getType() const;

	using Converter = juce::ARAHostModel::ConversionFunctions<ARAVirtualMusicalContext*, ARA::ARAMusicalContextHostRef>;
	using MidiEventType = ARAExtension::ARAContentType;

public:
	virtual int32_t getEventCount() { return 0; };

protected:
	SeqSourceProcessor* getSeq() const;

private:
	SeqSourceProcessor* const seq = nullptr;
	const ARAExtension::ARAContentType type = ARAExtension::ARAContentTypeUnknown;

	juce::ARAHostModel::MusicalContext musicalContext;

	ARA::ARAColor color;

	static const ARA::ARAMusicalContextProperties createProperties(
		SeqSourceProcessor* seq, ARA::ARAColor* color, ARAExtension::ARAContentType type);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualMusicalContext)
};

class ARAVirtualTempoContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualTempoContext() = delete;
	ARAVirtualTempoContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getEventCount() override;
	ARA::ARAContentTempoEntry getTempo(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualTempoContext)
};

class ARAVirtualBarContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualBarContext() = delete;
	ARAVirtualBarContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getEventCount() override;
	ARA::ARAContentBarSignature getBar(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualBarContext)
};

class ARAVirtualKeyContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualKeyContext() = delete;
	ARAVirtualKeyContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getEventCount() override;
	ARA::ARAContentKeySignature getKey(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualKeyContext)
};

class ARAVirtualNoteContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualNoteContext() = delete;
	ARAVirtualNoteContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getEventCount() override;
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
	int32_t getEventCount() override;
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
	int32_t getEventCount() override;
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
	int32_t getEventCount() override;
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
	int32_t getEventCount() override;
	ARAExtension::ARAContentPedal getSoftPedal(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualSoftPedalContext)
};

class ARAVirtualPitchWheelContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualPitchWheelContext() = delete;
	ARAVirtualPitchWheelContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getEventCount() override;
	ARAExtension::ARAContentIntParam getPitchWheel(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualPitchWheelContext)
};

class ARAVirtualAfterTouchContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualAfterTouchContext() = delete;
	ARAVirtualAfterTouchContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getEventCount() override;
	ARAExtension::ARAContentAfterTouch getAfterTouch(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualAfterTouchContext)
};

class ARAVirtualChannelPressureContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualChannelPressureContext() = delete;
	ARAVirtualChannelPressureContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getEventCount() override;
	ARAExtension::ARAContentIntParam getChannelPressure(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualChannelPressureContext)
};

class ARAVirtualControllerContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualControllerContext() = delete;
	ARAVirtualControllerContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getEventCount() override;
	ARAExtension::ARAContentController getController(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualControllerContext)
};

class ARAVirtualMiscContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualMiscContext() = delete;
	ARAVirtualMiscContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getEventCount() override;
	ARAExtension::ARAContentMisc getMisc(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualMiscContext)
};

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
	double getLength() const;

	juce::ARAHostModel::AudioSource& getProperties();

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
	virtual ~ARAVirtualMusicalContext() = default;

	void update();

	ARAExtension::ARAContentType getType() const;

	juce::ARAHostModel::MusicalContext& getProperties();

	using Converter = juce::ARAHostModel::ConversionFunctions<ARAVirtualMusicalContext*, ARA::ARAMusicalContextHostRef>;
	using MidiEventType = ARAExtension::ARAContentType;

public:
	virtual int32_t getEventCount() { return 0; };
	virtual double getLength() { return 0; };

protected:
	SeqSourceProcessor* getSeq() const;

private:
	SeqSourceProcessor* const seq = nullptr;
	const ARAExtension::ARAContentType type = ARAExtension::ARAContentTypeUnknown;

	juce::ARAHostModel::MusicalContext musicalContext;

	ARA::ARAColor color{};

	static const ARA::ARAMusicalContextProperties createProperties(
		SeqSourceProcessor* seq, ARA::ARAColor* color, ARAExtension::ARAContentType type);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualMusicalContext)
};

class ARAVirtualEmptyContext : public ARAVirtualMusicalContext {
public:
	ARAVirtualEmptyContext() = delete;
	ARAVirtualEmptyContext(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq);

public:
	int32_t getEventCount() override;
	const char* getData(int32_t index);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualEmptyContext)
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

class ARAVirtualAudioModification {
public:
	ARAVirtualAudioModification(
		ARA::Host::DocumentController& dc,
		ARAVirtualAudioSource& source);

	ARAVirtualAudioSource& getSource();

	juce::ARAHostModel::AudioModification& getProperties();

	using Converter = juce::ARAHostModel::ConversionFunctions<ARAVirtualAudioModification*, ARA::ARAAudioModificationHostRef>;

private:
	ARAVirtualAudioSource& source;

	juce::ARAHostModel::AudioModification audioModification;

	static const ARA::ARAAudioModificationProperties createProperties();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualAudioModification)
};

class ARAVirtualRegionSequence {
public:
	ARAVirtualRegionSequence() = delete;
	ARAVirtualRegionSequence(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq,
		const ARA::ARARegionSequenceProperties& properties);
	virtual ~ARAVirtualRegionSequence() = default;

	virtual void update() = 0;

	juce::ARAHostModel::RegionSequence& getProperties();

	SeqSourceProcessor* getSeq() const;

	using Converter = juce::ARAHostModel::ConversionFunctions<ARAVirtualRegionSequence*, ARA::ARARegionSequenceHostRef>;

protected:
	SeqSourceProcessor* const seq = nullptr;
	juce::ARAHostModel::RegionSequence regionSequence;

	ARA::ARAColor color{};

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualRegionSequence)
};

class ARAVirtualMusicalContextRegionSequence : public ARAVirtualRegionSequence {
public:
	ARAVirtualMusicalContextRegionSequence(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq,
		ARAVirtualMusicalContext& context);

	void update() override;

	ARAVirtualMusicalContext& getContext();

private:
	ARAVirtualMusicalContext& context;

	static const ARA::ARARegionSequenceProperties createProperties(
		SeqSourceProcessor* seq, ARA::ARAColor* color, ARAVirtualMusicalContext& context);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualMusicalContextRegionSequence)
};

class ARAVirtualAudioSourceRegionSequence : public ARAVirtualRegionSequence {
public:
	ARAVirtualAudioSourceRegionSequence(
		ARA::Host::DocumentController& dc,
		SeqSourceProcessor* seq,
		ARAVirtualMusicalContext& emptyContext);

	void update() override;

	ARAVirtualMusicalContext& getEmptyContext();

private:
	ARAVirtualMusicalContext& emptyContext;

	static const ARA::ARARegionSequenceProperties createProperties(
		SeqSourceProcessor* seq, ARA::ARAColor* color, ARAVirtualMusicalContext& emptyContext);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualAudioSourceRegionSequence)
};

class ARAVirtualPlaybackRegion {
public:
	ARAVirtualPlaybackRegion() = delete;
	ARAVirtualPlaybackRegion(
		ARA::Host::DocumentController& dc,
		ARAVirtualRegionSequence& sequence,
		ARAVirtualAudioModification& modification,
		const ARA::ARAPlaybackRegionProperties& properties);
	virtual ~ARAVirtualPlaybackRegion() = default;

	ARAVirtualAudioModification& getModification();

	virtual void update() = 0;

	juce::ARAHostModel::PlaybackRegion& getProperties();

	using Converter = juce::ARAHostModel::ConversionFunctions<ARAVirtualPlaybackRegion*, ARA::ARAPlaybackRegionHostRef>;

protected:
	ARAVirtualRegionSequence& sequence;
	ARAVirtualAudioModification& modification;

	juce::ARAHostModel::PlaybackRegion playbackRegion;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualPlaybackRegion)
};

class ARAVirtualAudioPlaybackRegion : public ARAVirtualPlaybackRegion {
public:
	ARAVirtualAudioPlaybackRegion(
		ARA::Host::DocumentController& dc,
		ARAVirtualRegionSequence& sequence,
		ARAVirtualAudioModification& modification);

	void update() override;

private:
	static const ARA::ARAPlaybackRegionProperties createProperties(
		ARAVirtualAudioSourceRegionSequence& sequence,
		ARAVirtualAudioModification& modification);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualAudioPlaybackRegion)
};

class ARAVirtualMusicalContextPlaybackRegion : public ARAVirtualPlaybackRegion {
public:
	ARAVirtualMusicalContextPlaybackRegion(
		ARA::Host::DocumentController& dc,
		ARAVirtualRegionSequence& sequence,
		ARAVirtualAudioModification& modification);

	void update() override;

private:
	static const ARA::ARAPlaybackRegionProperties createProperties(
		ARAVirtualMusicalContextRegionSequence& sequence);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualMusicalContextPlaybackRegion)
};

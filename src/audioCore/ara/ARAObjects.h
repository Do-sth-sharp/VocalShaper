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
		SeqSourceProcessor* seq);

	void update();
	double getLength() const;

	juce::ARAHostModel::MusicalContext& getProperties();

	using Converter = juce::ARAHostModel::ConversionFunctions<ARAVirtualMusicalContext*, ARA::ARAMusicalContextHostRef>;

public:
	int32_t getTempoCount() const;
	ARA::ARAContentTempoEntry getTempo(int32_t index) const;

	int32_t getBarCount() const;
	ARA::ARAContentBarSignature getBar(int32_t index) const;

	int32_t getKeyCount() const;
	ARA::ARAContentKeySignature getKey(int32_t index) const;

	int32_t getNoteCount() const;
	ARA::ARAContentNote getNote(int32_t index) const;

	int32_t getNotePlusCount() const;
	ARAExtension::ARAContentNote getNotePlus(int32_t index) const;

	int32_t getSustainPedalCount() const;
	ARAExtension::ARAContentPedal getSustainPedal(int32_t index) const;

	int32_t getSostenutoPedalCount() const;
	ARAExtension::ARAContentPedal getSostenutoPedal(int32_t index) const;

	int32_t getSoftPedalCount() const;
	ARAExtension::ARAContentPedal getSoftPedal(int32_t index) const;

	int32_t getPitchWheelCount() const;
	ARAExtension::ARAContentIntParam getPitchWheel(int32_t index) const;

	int32_t getAfterTouchCount() const;
	ARAExtension::ARAContentAfterTouch getAfterTouch(int32_t index) const;

	int32_t getChannelPressureCount() const;
	ARAExtension::ARAContentIntParam getChannelPressure(int32_t index) const;

	int32_t getControllerCount() const;
	ARAExtension::ARAContentController getController(int32_t index) const;

	int32_t getMiscCount() const;
	ARAExtension::ARAContentMisc getMisc(int32_t index) const;

protected:
	SeqSourceProcessor* getSeq() const;

private:
	SeqSourceProcessor* const seq = nullptr;

	juce::ARAHostModel::MusicalContext musicalContext;

	ARA::ARAColor color{};

	static const ARA::ARAMusicalContextProperties createProperties(
		SeqSourceProcessor* seq, ARA::ARAColor* color);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualMusicalContext)
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
		ARAVirtualMusicalContext& context);

	void update();

	juce::ARAHostModel::RegionSequence& getProperties();

	ARAVirtualMusicalContext& getContext();

	SeqSourceProcessor* getSeq() const;

	using Converter = juce::ARAHostModel::ConversionFunctions<ARAVirtualRegionSequence*, ARA::ARARegionSequenceHostRef>;

private:
	SeqSourceProcessor* const seq = nullptr;
	ARAVirtualMusicalContext& context;

	juce::ARAHostModel::RegionSequence regionSequence;

	ARA::ARAColor color{};

	static const ARA::ARARegionSequenceProperties createProperties(
		SeqSourceProcessor* seq, ARA::ARAColor* color, ARAVirtualMusicalContext& context);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualRegionSequence)
};

class ARAVirtualPlaybackRegion {
public:
	ARAVirtualPlaybackRegion() = delete;
	ARAVirtualPlaybackRegion(
		ARA::Host::DocumentController& dc,
		ARAVirtualRegionSequence& sequence,
		ARAVirtualAudioModification& modification);

	ARAVirtualAudioModification& getModification();

	void update();

	juce::ARAHostModel::PlaybackRegion& getProperties();

	using Converter = juce::ARAHostModel::ConversionFunctions<ARAVirtualPlaybackRegion*, ARA::ARAPlaybackRegionHostRef>;

private:
	ARAVirtualRegionSequence& sequence;
	ARAVirtualAudioModification& modification;

	juce::ARAHostModel::PlaybackRegion playbackRegion;

	ARA::ARAColor color{};

	static const ARA::ARAPlaybackRegionProperties createProperties(
		ARAVirtualRegionSequence& sequence,
		ARAVirtualAudioModification& modification,
		ARA::ARAColor* color);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualPlaybackRegion)
};

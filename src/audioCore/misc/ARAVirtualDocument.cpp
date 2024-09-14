#include "ARAVirtualDocument.h"

ARAVirtualDocument::ARAVirtualDocument(
	SeqSourceProcessor* seq,
	ARA::Host::DocumentController& controller)
	: seq(seq), controller(controller) {}

ARAVirtualDocument::~ARAVirtualDocument() {
	this->clear();
}

void ARAVirtualDocument::update() {
	/** Lock Document */
	juce::ARAEditGuard locker(this->controller);

	/** Clear Virtual Objects */
	this->clearUnsafe();

	/** Audio Source */
	this->audioSource = std::make_unique<ARAVirtualAudioSource>(
		this->controller, this->seq);/**< TODO At Least 1 Sample In Audio Source */
	
	/** Audio Modification */
	this->audioModification = std::make_unique<ARAVirtualAudioModification>(
		this->controller, *(this->audioSource));

	/** Musical Contexts */
	this->musicalContexts.add(
		std::make_unique<ARAVirtualEmptyContext>(this->controller, this->seq));
	this->musicalContexts.add(
		std::make_unique<ARAVirtualTempoContext>(this->controller, this->seq));
	this->musicalContexts.add(
		std::make_unique<ARAVirtualBarContext>(this->controller, this->seq));
	this->musicalContexts.add(
		std::make_unique<ARAVirtualKeyContext>(this->controller, this->seq));/**< TODO At Least 1 Key Signature Event In Context */
	this->musicalContexts.add(
		std::make_unique<ARAVirtualNoteContext>(this->controller, this->seq));
	this->musicalContexts.add(
		std::make_unique<ARAVirtualNotePlusContext>(this->controller, this->seq));
	this->musicalContexts.add(
		std::make_unique<ARAVirtualSustainPedalContext>(this->controller, this->seq));
	this->musicalContexts.add(
		std::make_unique<ARAVirtualSostenutoPedalContext>(this->controller, this->seq));
	this->musicalContexts.add(
		std::make_unique<ARAVirtualSoftPedalContext>(this->controller, this->seq));
	this->musicalContexts.add(
		std::make_unique<ARAVirtualPitchWheelContext>(this->controller, this->seq));
	this->musicalContexts.add(
		std::make_unique<ARAVirtualAfterTouchContext>(this->controller, this->seq));
	this->musicalContexts.add(
		std::make_unique<ARAVirtualChannelPressureContext>(this->controller, this->seq));
	this->musicalContexts.add(
		std::make_unique<ARAVirtualControllerContext>(this->controller, this->seq));
	this->musicalContexts.add(
		std::make_unique<ARAVirtualMiscContext>(this->controller, this->seq));

	/** Region Sequence */
	for (auto i : this->musicalContexts) {
		if (auto p = dynamic_cast<ARAVirtualEmptyContext*>(i)) {
			this->regionSequences.add(std::make_unique<ARAVirtualAudioSourceRegionSequence>(
				this->controller, this->seq, *(this->audioModification), *i));
		}
		else {
			this->regionSequences.add(std::make_unique<ARAVirtualMusicalContextRegionSequence>(
				this->controller, this->seq, *i));
		}
	}

	/** Playback Region */
	for (auto i : this->regionSequences) {
		if (auto p = dynamic_cast<ARAVirtualAudioSourceRegionSequence*>(i)) {
			this->playbackRegions.add(std::make_unique<ARAVirtualAudioPlaybackRegion>(
				this->controller, *i, *(this->audioModification)));
		}
		else if (auto p = dynamic_cast<ARAVirtualMusicalContextRegionSequence*>(i)) {
			this->playbackRegions.add(std::make_unique<ARAVirtualMusicalContextPlaybackRegion>(
				this->controller, *i, *(this->audioModification)));
		}
	}
}

void ARAVirtualDocument::clear() {
	juce::ARAEditGuard locker(this->controller);
	this->clearUnsafe();
}

void ARAVirtualDocument::clearUnsafe() {
	this->playbackRegions.clear();
	this->regionSequences.clear();
	this->musicalContexts.clear();
	this->audioModification = nullptr;
	this->audioSource = nullptr;
}

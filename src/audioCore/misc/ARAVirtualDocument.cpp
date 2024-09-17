#include "ARAVirtualDocument.h"

ARAVirtualDocument::ARAVirtualDocument(
	SeqSourceProcessor* seq,
	ARA::Host::DocumentController& controller,
	juce::ARAHostModel::EditorRendererInterface& araEditorRenderer,
	juce::ARAHostModel::PlaybackRendererInterface& araPlaybackRenderer,
	const PluginOnOffFunc& pluginOnOff)
	: seq(seq), controller(controller), pluginOnOff(pluginOnOff),
	araEditorRenderer(araEditorRenderer), araPlaybackRenderer(araPlaybackRenderer) {}

ARAVirtualDocument::~ARAVirtualDocument() {
	this->clear();
}

void ARAVirtualDocument::update() {
	/** Turn Off Plugin */
	this->pluginOnOff(false);

	/** Lock Document */
	juce::ARAEditGuard locker(this->controller);

	/** Clear Virtual Objects */
	this->clearUnsafe();

	/** Audio Source */
	this->audioSource = std::make_unique<ARAVirtualAudioSource>(
		this->controller, this->seq);
	
	/** Audio Modification */
	this->audioModification = std::make_unique<ARAVirtualAudioModification>(
		this->controller, *(this->audioSource));

	/** Musical Contexts */
	this->musicalContexts.add(
		std::make_unique<ARAVirtualEmptyContext>(this->controller, this->seq));
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
				this->controller, this->seq, *i));
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

	/** Add Regions To Renderer */
	this->addRegions();

	/** Turn On Plugin */
	this->pluginOnOff(true);
}

void ARAVirtualDocument::clear() {
	/** Turn Off Plugin */
	this->pluginOnOff(false);

	/** Lock Document */
	juce::ARAEditGuard locker(this->controller);

	/** Clear Objects */
	this->clearUnsafe();

	/** Turn On Plugin */
	this->pluginOnOff(true);
}

void ARAVirtualDocument::clearUnsafe() {
	/** Remove Regions From Renderer */
	this->removeRegions();

	/** Clear Objects */
	this->playbackRegions.clear();
	this->regionSequences.clear();
	this->musicalContexts.clear();
	this->audioModification = nullptr;
	this->audioSource = nullptr;
}

void ARAVirtualDocument::removeRegions() {
	for (auto i : this->playbackRegions) {
		if (this->araEditorRenderer.isValid()) {
			this->araEditorRenderer.remove(i->getProperties());
		}
		if (this->araPlaybackRenderer.isValid()) {
			this->araPlaybackRenderer.remove(i->getProperties());
		}
	}
}

void ARAVirtualDocument::addRegions() {
	for (auto i : this->playbackRegions) {
		if (this->araEditorRenderer.isValid()) {
			this->araEditorRenderer.add(i->getProperties());
		}
		if (this->araPlaybackRenderer.isValid()) {
			this->araPlaybackRenderer.add(i->getProperties());
		}
	}
}

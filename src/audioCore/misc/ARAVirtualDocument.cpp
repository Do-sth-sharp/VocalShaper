﻿#include "ARAVirtualDocument.h"

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

	/** Musical Context */
	this->musicalContext =
		std::make_unique<ARAVirtualMusicalContext>(this->controller, this->seq);
	
	/** Region Sequence */
	this->regionSequence = std::make_unique<ARAVirtualRegionSequence>(
		this->controller, this->seq, *(this->musicalContext));

	/** Playback Region */
	this->playbackRegion = std::make_unique<ARAVirtualPlaybackRegion>(
		this->controller, *(this->regionSequence), *(this->audioModification));

	/** Add Regions To Renderer */
	this->addRegionToRenderer();

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
	this->removeRegionToRenderer();

	/** Clear Objects */
	this->playbackRegion = nullptr;
	this->regionSequence = nullptr;
	this->musicalContext = nullptr;
	this->audioModification = nullptr;
	this->audioSource = nullptr;
}

void ARAVirtualDocument::removeRegionToRenderer() {
	if (this->playbackRegion) {
		if (this->araEditorRenderer.isValid()) {
			this->araEditorRenderer.remove(this->playbackRegion->getProperties());
		}
		if (this->araPlaybackRenderer.isValid()) {
			this->araPlaybackRenderer.remove(this->playbackRegion->getProperties());
		}
	}
}

void ARAVirtualDocument::addRegionToRenderer() {
	if (this->playbackRegion) {
		if (this->araEditorRenderer.isValid()) {
			this->araEditorRenderer.add(this->playbackRegion->getProperties());
		}
		if (this->araPlaybackRenderer.isValid()) {
			this->araPlaybackRenderer.add(this->playbackRegion->getProperties());
		}
	}
}

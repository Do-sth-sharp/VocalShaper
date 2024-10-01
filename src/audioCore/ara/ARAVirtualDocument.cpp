#include "ARAVirtualDocument.h"
#include "../graph/SeqSourceProcessor.h"

ARAVirtualDocument::ARAVirtualDocument(
	SeqSourceProcessor* seq,
	ARA::Host::DocumentController& controller,
	juce::ARAHostModel::EditorRendererInterface& araEditorRenderer,
	juce::ARAHostModel::PlaybackRendererInterface& araPlaybackRenderer,
	const PluginOnOffFunc& pluginOnOff)
	: seq(seq), controller(controller), pluginOnOff(pluginOnOff),
	araEditorRenderer(araEditorRenderer), araPlaybackRenderer(araPlaybackRenderer) {
	this->listener = std::make_unique<ARAChangeListener>(this);
	this->regionListener = std::make_unique<ARARegionChangeListener>(this);
	this->contextListener = std::make_unique<ARAContextChangeListener>(this);
}

ARAVirtualDocument::~ARAVirtualDocument() {
	this->clear();
}

void ARAVirtualDocument::updateRegions() {
	/** Invoke This On Message Thread */
	JUCE_ASSERT_MESSAGE_THREAD

	/** Turn Off Plugin */
	this->lockPlugin(true);

	/** Lock Document */
	juce::ARAEditGuard locker(this->controller);

	/** Clear Regions */
	this->clearRegionsUnsafe();

	/** Playback Regions */
	for (int i = 0; i < this->seq->getSeqNum(); i++) {
		auto [startTime, endTime, offset] = this->seq->getSeq(i);
		this->playbackRegions.add(new ARAVirtualPlaybackRegion{ { startTime, startTime + offset, endTime - startTime },
			this->controller, *(this->regionSequence), *(this->audioModification) });
	}

	/** Add Regions To Renderer */
	this->addRegionsToRenderer();

	/** Turn On Plugin */
	this->lockPlugin(false);
}

void ARAVirtualDocument::updateAudioAndContext() {
	/** Invoke This On Message Thread */
	JUCE_ASSERT_MESSAGE_THREAD

	/** Turn Off Plugin */
	this->lockPlugin(true);

	/** Lock Document */
	juce::ARAEditGuard locker(this->controller);

	/** Remove Audio Modification */
	this->audioModification = nullptr;

	/** Update Audio Source */
	if (this->audioSource) {
		this->audioSource->update();
	}

	/** Create New Audio Modification */
	this->audioModification = std::make_unique<ARAVirtualAudioModification>(
		this->controller, *(this->audioSource));

	/** Update Musical Context */
	if (this->musicalContext) {
		this->musicalContext->update();
	}

	/** Turn On Plugin */
	this->lockPlugin(false);
}

void ARAVirtualDocument::update() {
	/** Invoke This On Message Thread */
	JUCE_ASSERT_MESSAGE_THREAD

	/** Turn Off Plugin */
	this->lockPlugin(true);

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

	/** Playback Regions */
	for (int i = 0; i < this->seq->getSeqNum(); i++) {
		auto [startTime, endTime, offset] = this->seq->getSeq(i);
		this->playbackRegions.add(new ARAVirtualPlaybackRegion{ { startTime, startTime + offset, endTime - startTime },
			this->controller, *(this->regionSequence), *(this->audioModification) });
	}

	/** Add Regions To Renderer */
	this->addRegionsToRenderer();

	/** Turn On Plugin */
	this->lockPlugin(false);
}

void ARAVirtualDocument::clear() {
	/** Turn Off Plugin */
	this->lockPlugin(true);

	/** Lock Document */
	juce::ARAEditGuard locker(this->controller);

	/** Clear Objects */
	this->clearUnsafe();

	/** Turn On Plugin */
	this->lockPlugin(false);
}

juce::ChangeListener* ARAVirtualDocument::getListener() const {
	return this->listener.get();
}

juce::ChangeListener* ARAVirtualDocument::getRegionListener() const {
	return this->regionListener.get();
}

juce::ChangeListener* ARAVirtualDocument::getContextListener() const {
	return this->contextListener.get();
}

void ARAVirtualDocument::clearUnsafe() {
	/** Remove Regions */
	this->clearRegionsUnsafe();
	
	/** Clear Objects */
	this->regionSequence = nullptr;
	this->musicalContext = nullptr;
	this->audioModification = nullptr;
	this->audioSource = nullptr;
}

void ARAVirtualDocument::clearRegionsUnsafe() {
	/** Remove Regions From Renderer */
	this->removeRegionsFromRenderer();

	/** Clear Objects */
	this->playbackRegions.clear();
}

void ARAVirtualDocument::removeRegionsFromRenderer() {
	for (auto region : this->playbackRegions) {
		if (this->araEditorRenderer.isValid()) {
			this->araEditorRenderer.remove(region->getProperties());
		}
		if (this->araPlaybackRenderer.isValid()) {
			this->araPlaybackRenderer.remove(region->getProperties());
		}
	}
}

void ARAVirtualDocument::addRegionsToRenderer() {
	for (auto region : this->playbackRegions) {
		if (this->araEditorRenderer.isValid()) {
			this->araEditorRenderer.add(region->getProperties());
		}
		if (this->araPlaybackRenderer.isValid()) {
			this->araPlaybackRenderer.add(region->getProperties());
		}
	}
}

void ARAVirtualDocument::lockPlugin(bool locked) {
	juce::GenericScopedLock locker(this->pluginLockMutex);

	if (locked) {
		if (this->pluginLockCount == 0) {
			this->pluginOnOff(false);
		}
		this->pluginLockCount++;
	}
	else {
		this->pluginLockCount--;
		if (this->pluginLockCount == 0) {
			this->pluginOnOff(true);
		}
	}
}

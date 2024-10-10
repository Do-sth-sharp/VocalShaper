#include "ARAVirtualDocument.h"
#include "ARAController.h"
#include "ARAArchive.h"
#include "../graph/SeqSourceProcessor.h"

ARAVirtualDocument::ARAVirtualDocument(
	SeqSourceProcessor* seq,
	ARA::Host::DocumentController& controller,
	juce::ARAHostModel::EditorRendererInterface& araEditorRenderer,
	juce::ARAHostModel::PlaybackRendererInterface& araPlaybackRenderer,
	const PluginOnOffFunc& pluginOnOff)
	: seq(seq), controller(controller), pluginOnOff(pluginOnOff),
	araEditorRenderer(araEditorRenderer), araPlaybackRenderer(araPlaybackRenderer) {
	/** Listeners */
	this->regionListener = std::make_unique<ARARegionChangeListener>(this);
	this->contextListener = std::make_unique<ARAContextChangeListener>(this);
	this->infoListener = std::make_unique<ARATrackInfoChangeListener>(this);
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

	/** Update Audio Source */
	if (this->audioSource) {
		this->audioSource->update();
	}

	/** Update Musical Context */
	if (this->musicalContext) {
		this->musicalContext->update();
	}

	/** Turn On Plugin */
	this->lockPlugin(false);
}

void ARAVirtualDocument::updateTrackBaseInfo() {
	/** Invoke This On Message Thread */
	JUCE_ASSERT_MESSAGE_THREAD

		/** Turn Off Plugin */
		this->lockPlugin(true);

	/** Lock Document */
	juce::ARAEditGuard locker(this->controller);

	/** Update Audio Source */
	if (this->audioSource) {
		this->audioSource->update();
	}

	/** Update Musical Context */
	if (this->musicalContext) {
		this->musicalContext->update();
	}

	/** Update Region Sequence */
	if (this->regionSequence) {
		this->regionSequence->update();
	}

	/** Update Playback Regions */
	for (auto i : this->playbackRegions) {
		i->update();
	}

	/** Turn On Plugin */
	this->lockPlugin(false);
}

void ARAVirtualDocument::init() {
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

juce::ChangeListener* ARAVirtualDocument::getRegionListener() const {
	return this->regionListener.get();
}

juce::ChangeListener* ARAVirtualDocument::getContextListener() const {
	return this->contextListener.get();
}

juce::ChangeListener* ARAVirtualDocument::getTrackInfoListener() const {
	return this->infoListener.get();
}

void ARAVirtualDocument::storeToStream(juce::MemoryOutputStream& stream) const {
	/** Invoke This On Message Thread */
	JUCE_ASSERT_MESSAGE_THREAD

	/** Store State */
	ARA::ARAStoreObjectsFilter filter{};
	ARAVirtualDocument::initStoreFilter(filter,
		this->audioSource.get(), this->audioModification.get());

	/** Archive Writer */
	ARAArchiveWriter writer(stream, this->getArchiveID());

	/** ARA Doesn't Export It's Symbols. Using C API To Make ARA Library Symbols Happy */
	this->storeObjectsToArchive(
		ARAArchivingController::WriterConverter::toHostRef(&writer), &filter);

	ARAVirtualDocument::destoryStoreFilter(filter);
}

void ARAVirtualDocument::restoreFromBlock(const juce::MemoryBlock& block) {
	/** Invoke This On Message Thread */
	JUCE_ASSERT_MESSAGE_THREAD

	/** Data Size Must Greater Than 0 */
	if (block.getSize() == 0) { return; }

	/** Turn Off Plugin */
	this->lockPlugin(true);

	/** Lock Document */
	juce::ARAEditGuard locker(this->controller);

	/** Restore State */
	ARA::ARARestoreObjectsFilter filter{};
	ARAVirtualDocument::initRestoreFilter(filter,
		this->audioSource.get(), this->audioModification.get());

	/** Archive Reader */
	ARAArchiveReader reader(block);
	if (reader.isValid() && (reader.getSize() > 0)) {
		/** ARA Doesn't Export It's Symbols. Using C API To Make ARA Library Symbols Happy */
		this->restoreObjectsFromArchive(
			ARAArchivingController::ReaderConverter::toHostRef(&reader), &filter);
	}

	ARAVirtualDocument::destoryRestoreFilter(filter);

	/** Turn On Plugin */
	this->lockPlugin(false);
}

void ARAVirtualDocument::initStoreFilter(
	ARA::ARAStoreObjectsFilter& filter,
	ARAVirtualAudioSource* source, ARAVirtualAudioModification* modification) {
	filter.structSize = ARA::kARAStoreObjectsFilterMinSize;
	filter.documentData = true;

	if (source) {
		filter.audioSourceRefsCount = 1;

		ARA::ARAAudioSourceRef* list = static_cast<ARA::ARAAudioSourceRef*>(
			std::malloc(sizeof(ARA::ARAAudioSourceRef) * filter.audioSourceRefsCount));
		if (list) {
			list[0] = source->getProperties().getPluginRef();
		}

		filter.audioSourceRefs = list;
		
	}
	if (modification) {
		filter.audioModificationRefsCount = 1;

		ARA::ARAAudioModificationRef* list = static_cast<ARA::ARAAudioModificationRef*>(
			std::malloc(sizeof(ARA::ARAAudioModificationRef) * filter.audioModificationRefsCount));
		if (list) {
			list[0] = modification->getProperties().getPluginRef();
		}

		filter.audioModificationRefs = list;
	}
}

void ARAVirtualDocument::initRestoreFilter(
	ARA::ARARestoreObjectsFilter& filter,
	ARAVirtualAudioSource* source, ARAVirtualAudioModification* modification) {
	filter.structSize = ARA::kARAStoreObjectsFilterMinSize;
	filter.documentData = true;

	if (source) {
		filter.audioSourceIDsCount = 1;

		ARA::ARAPersistentID* list = static_cast<ARA::ARAPersistentID*>(
			std::malloc(sizeof(ARA::ARAPersistentID) * filter.audioSourceIDsCount));
		if (list) {
			list[0] = ARAVirtualAudioSource::defaultID;
		}

		filter.audioSourceArchiveIDs = list;
	}
	if (modification) {
		filter.audioModificationIDsCount = 1;

		ARA::ARAPersistentID* list = static_cast<ARA::ARAPersistentID*>(
			std::malloc(sizeof(ARA::ARAPersistentID) * filter.audioModificationIDsCount));
		if (list) {
			list[0] = ARAVirtualAudioModification::defaultID;
		}

		filter.audioModificationArchiveIDs = list;
	}
}

void ARAVirtualDocument::destoryStoreFilter(ARA::ARAStoreObjectsFilter& filter) {
	filter.audioSourceRefsCount = 0;
	std::free((void*)(filter.audioSourceRefs));
	filter.audioSourceRefs = nullptr;

	filter.audioModificationRefsCount = 0;
	std::free((void*)(filter.audioModificationRefs));
	filter.audioModificationRefs = nullptr;
}

void ARAVirtualDocument::destoryRestoreFilter(ARA::ARARestoreObjectsFilter& filter) {
	filter.audioSourceIDsCount = 0;
	std::free((void*)(filter.audioSourceArchiveIDs));
	filter.audioSourceArchiveIDs = nullptr;
	std::free((void*)(filter.audioSourceCurrentIDs));
	filter.audioSourceCurrentIDs = nullptr;

	filter.audioModificationIDsCount = 0;
	std::free((void*)(filter.audioModificationArchiveIDs));
	filter.audioModificationArchiveIDs = nullptr;
	std::free((void*)(filter.audioModificationCurrentIDs));
	filter.audioModificationCurrentIDs = nullptr;
}

bool ARAVirtualDocument::supportsPartialPersistency() const {
	/** ARA Doesn't Export It's Symbols. Using C API To Make ARA Library Symbols Happy */
	return this->controller.getInterface()
		.implements<ARA_STRUCT_MEMBER(ARADocumentControllerInterface, storeObjectsToArchive)>();
}

bool ARAVirtualDocument::restoreObjectsFromArchive(
	ARA::ARAArchiveReaderHostRef archiveReaderHostRef, const ARA::ARARestoreObjectsFilter* filter) {
	if (!this->supportsPartialPersistency()) {
		return false;
	}	

	/** ARA Doesn't Export It's Symbols. Using C API To Make ARA Library Symbols Happy */
	return this->controller.getInterface()
		->restoreObjectsFromArchive(this->controller.getRef(), archiveReaderHostRef, filter);
}

bool ARAVirtualDocument::storeObjectsToArchive(
	ARA::ARAArchiveWriterHostRef archiveWriterHostRef,
	const ARA::ARAStoreObjectsFilter* filter) const {
	if (!this->supportsPartialPersistency()) {
		return false;
	}

	/** ARA Doesn't Export It's Symbols. Using C API To Make ARA Library Symbols Happy */
	return this->controller.getInterface()
		->storeObjectsToArchive(this->controller.getRef(), archiveWriterHostRef, filter);
}

const ARA::ARAFactory* ARAVirtualDocument::getFactory() const {
	/** ARA Doesn't Export It's Symbols. Using C API To Make ARA Library Symbols Happy */
	return this->controller.getInterface()
		->getFactory(this->controller.getRef());
}

const char* ARAVirtualDocument::getArchiveID() const {
	/** ARA Doesn't Export It's Symbols. Using C API To Make ARA Library Symbols Happy */
	if (auto ptrFactory = this->getFactory()) {
		return ptrFactory->documentArchiveID;
	}
	return nullptr;
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

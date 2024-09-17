#include "ARAController.h"
#include "../AudioCore.h"

ARA::ARAAudioReaderHostRef ARAAudioAccessController::createAudioReaderForSource(
	ARA::ARAAudioSourceHostRef audioSourceHostRef,
	bool use64BitSamples) noexcept {
	auto audioReader = std::make_unique<AudioReader>(audioSourceHostRef, use64BitSamples);
	auto audioReaderHostRef = Converter::toHostRef(audioReader.get());

	this->audioReaders.emplace(audioReader.get(), std::move(audioReader));

	return audioReaderHostRef;
}

bool ARAAudioAccessController::readAudioSamples(
	ARA::ARAAudioReaderHostRef audioReaderHostRef,
	ARA::ARASamplePosition samplePosition,
	ARA::ARASampleCount samplesPerChannel,
	void* const buffers[]) noexcept {
	const bool use64BitSamples = Converter::fromHostRef(audioReaderHostRef)->use64Bit;
	auto* audioSource = SourceConverter::fromHostRef(Converter::fromHostRef(audioReaderHostRef)->sourceHostRef);
	
	if (!use64BitSamples) {
		return audioSource->readAudioSamples(
			reinterpret_cast<float* const*>(buffers), samplePosition, samplesPerChannel);
	}

	return false;
}

void ARAAudioAccessController::destroyAudioReader(
	ARA::ARAAudioReaderHostRef audioReaderHostRef) noexcept {
	this->audioReaders.erase(Converter::fromHostRef(audioReaderHostRef));
}

ARA::ARASize ARAArchivingController::getArchiveSize(
	ARA::ARAArchiveReaderHostRef archiveReaderHostRef) noexcept {
	return (ARA::ARASize)ReaderConverter::fromHostRef(archiveReaderHostRef)->getSize();
}

bool ARAArchivingController::readBytesFromArchive(
	ARA::ARAArchiveReaderHostRef archiveReaderHostRef,
	ARA::ARASize position, ARA::ARASize length,
	ARA::ARAByte buffer[]) noexcept {
	auto* archiveReader = ReaderConverter::fromHostRef(archiveReaderHostRef);

	if ((position + length) <= archiveReader->getSize()) {
		std::memcpy(buffer, juce::addBytesToPointer(
			archiveReader->getData(), position), length);
		return true;
	}

	return false;
}

bool ARAArchivingController::writeBytesToArchive(
	ARA::ARAArchiveWriterHostRef archiveWriterHostRef,
	ARA::ARASize position, ARA::ARASize length,
	const ARA::ARAByte buffer[]) noexcept {
	auto* archiveWriter = WriterConverter::fromHostRef(archiveWriterHostRef);
	return (archiveWriter->setPosition((int64_t)position) && archiveWriter->write(buffer, length));
}

void ARAArchivingController::notifyDocumentArchivingProgress(
	float /*value*/) noexcept {
	/** Nothing To Do */
}

void ARAArchivingController::notifyDocumentUnarchivingProgress(
	float /*value*/) noexcept {
	/** Nothing To Do */
}

ARA::ARAPersistentID ARAArchivingController::getDocumentArchiveID(
	ARA::ARAArchiveReaderHostRef /*archiveReaderHostRef*/) noexcept {
	/** Nothing To Do */
	return nullptr;
}

bool ARAContentAccessController::isMusicalContextContentAvailable(
	ARA::ARAMusicalContextHostRef /*musicalContextHostRef*/,
	ARA::ARAContentType type) noexcept {
	return this->allowedContentTypes.contains((ARAExtension::ARAContentType)type);
}

ARA::ARAContentGrade ARAContentAccessController::getMusicalContextContentGrade(
	ARA::ARAMusicalContextHostRef /*musicalContextHostRef*/,
	ARA::ARAContentType /*type*/) noexcept {
	return ARA::kARAContentGradeInitial;
}

ARA::ARAContentReaderHostRef ARAContentAccessController::createMusicalContextContentReader(
	ARA::ARAMusicalContextHostRef musicalContextHostRef,
	ARA::ARAContentType type, const ARA::ARAContentTimeRange* /*range*/) noexcept {
	auto contextReader = std::make_unique<ContextReader>(
		(ARAExtension::ARAContentType)type, musicalContextHostRef);
	auto audioReaderHostRef = Converter::toHostRef(contextReader.get());

	this->contextReaders.emplace(contextReader.get(), std::move(contextReader));

	return audioReaderHostRef;
}

bool ARAContentAccessController::isAudioSourceContentAvailable(
	ARA::ARAAudioSourceHostRef /*audioSourceHostRef*/,
	ARA::ARAContentType /*type*/) noexcept {
	/** Nothing To Do */
	return false;
}

ARA::ARAContentGrade ARAContentAccessController::getAudioSourceContentGrade(
	ARA::ARAAudioSourceHostRef /*audioSourceHostRef*/,
	ARA::ARAContentType /*type*/) noexcept {
	/** Nothing To Do */
	return ARA::kARAContentGradeInitial;
}

ARA::ARAContentReaderHostRef ARAContentAccessController::createAudioSourceContentReader(
	ARA::ARAAudioSourceHostRef /*audioSourceHostRef*/,
	ARA::ARAContentType /*type*/, const ARA::ARAContentTimeRange* /*range*/) noexcept {
	return nullptr;
}

ARA::ARAInt32 ARAContentAccessController::getContentReaderEventCount(
	ARA::ARAContentReaderHostRef contentReaderHostRef) noexcept {
	auto contextReader = Converter::fromHostRef(contentReaderHostRef);

	if (contextReader->contextHostRef) {
		auto midiContext = ContextConverter::fromHostRef(contextReader->contextHostRef);

		/** Shit Code! It Blames to ARA. */

		switch (contextReader->type) {
		case ARAExtension::ARAContentTypeTempoEntry: {
			return midiContext->getTempoCount();
		}

		case ARAExtension::ARAContentTypeBarSignature: {
			return midiContext->getBarCount();
		}

		case ARAExtension::ARAContentTypeKeySignature: {
			return midiContext->getKeyCount();
		}

		case ARAExtension::ARAContentTypeNote: {
			return midiContext->getNoteCount();
		}

		case ARAExtension::ARAContentTypeNotePlus: {
			return midiContext->getNotePlusCount();
		}

		case ARAExtension::ARAContentTypeSustainPedal: {
			return midiContext->getSustainPedalCount();
		}

		case ARAExtension::ARAContentTypeSostenutoPedal: {
			return midiContext->getSostenutoPedalCount();
		}

		case ARAExtension::ARAContentTypeSoftPedal: {
			return midiContext->getSoftPedalCount();
		}

		case ARAExtension::ARAContentTypePitchWheel: {
			return midiContext->getPitchWheelCount();
		}

		case ARAExtension::ARAContentTypeAfterTouch: {
			return midiContext->getAfterTouchCount();
		}

		case ARAExtension::ARAContentTypeChannelPressure: {
			return midiContext->getChannelPressureCount();
		}

		case ARAExtension::ARAContentTypeController: {
			return midiContext->getControllerCount();
		}

		case ARAExtension::ARAContentTypeMisc: {
			return midiContext->getMiscCount();
		}
		}
	}

	return 0;
}

const void* ARAContentAccessController::getContentReaderDataForEvent(
	ARA::ARAContentReaderHostRef contentReaderHostRef,
	ARA::ARAInt32 eventIndex) noexcept {
	auto contextReader = Converter::fromHostRef(contentReaderHostRef);

	if (contextReader->contextHostRef) {
		auto midiContext = ContextConverter::fromHostRef(contextReader->contextHostRef);

		/** Shit Code! It Blames to ARA. */

		switch (contextReader->type) {
		case ARAExtension::ARAContentTypeTempoEntry: {
			this->tempoTemp = midiContext->getTempo(eventIndex);
			return &(this->tempoTemp);
		}

		case ARAExtension::ARAContentTypeBarSignature: {
			this->barTemp = midiContext->getBar(eventIndex);
			return &(this->barTemp);
		}

		case ARAExtension::ARAContentTypeKeySignature: {
			this->keyTemp = midiContext->getKey(eventIndex);
			return &(this->keyTemp);
		}

		case ARAExtension::ARAContentTypeNote: {
			this->noteTemp = midiContext->getNote(eventIndex);
			return &(this->noteTemp);
		}

		case ARAExtension::ARAContentTypeNotePlus: {
			this->notePlusTemp = midiContext->getNotePlus(eventIndex);
			return &(this->notePlusTemp);
		}

		case ARAExtension::ARAContentTypeSustainPedal: {
			this->pedalTemp = midiContext->getSustainPedal(eventIndex);
			return &(this->pedalTemp);
		}

		case ARAExtension::ARAContentTypeSostenutoPedal: {
			this->pedalTemp = midiContext->getSostenutoPedal(eventIndex);
			return &(this->pedalTemp);
		}

		case ARAExtension::ARAContentTypeSoftPedal: {
			this->pedalTemp = midiContext->getSoftPedal(eventIndex);
			return &(this->pedalTemp);
		}

		case ARAExtension::ARAContentTypePitchWheel: {
			this->intParamTemp = midiContext->getPitchWheel(eventIndex);
			return &(this->intParamTemp);
		}

		case ARAExtension::ARAContentTypeAfterTouch: {
			this->afterTouchTemp = midiContext->getAfterTouch(eventIndex);
			return &(this->afterTouchTemp);
		}

		case ARAExtension::ARAContentTypeChannelPressure: {
			this->intParamTemp = midiContext->getChannelPressure(eventIndex);
			return &(this->intParamTemp);
		}

		case ARAExtension::ARAContentTypeController: {
			this->controllerTemp = midiContext->getController(eventIndex);
			return &(this->controllerTemp);
		}

		case ARAExtension::ARAContentTypeMisc: {
			this->miscTemp = midiContext->getMisc(eventIndex);
			return &(this->miscTemp);
		}
		}
	}

	return nullptr;
}

void ARAContentAccessController::destroyContentReader(
	ARA::ARAContentReaderHostRef contentReaderHostRef) noexcept {
	this->contextReaders.erase(Converter::fromHostRef(contentReaderHostRef));
}

void ARAModelUpdateController::notifyAudioSourceAnalysisProgress(
	ARA::ARAAudioSourceHostRef /*audioSourceHostRef*/,
	ARA::ARAAnalysisProgressState /*state*/, float /*value*/) noexcept {
	/** Nothing To Do */
}

void ARAModelUpdateController::notifyAudioSourceContentChanged(
	ARA::ARAAudioSourceHostRef /*audioSourceHostRef*/,
	const ARA::ARAContentTimeRange* /*range*/,
	ARA::ContentUpdateScopes /*scopeFlags*/) noexcept {
	/** Nothing To Do */
}

void ARAModelUpdateController::notifyAudioModificationContentChanged(
	ARA::ARAAudioModificationHostRef /*audioModificationHostRef*/,
	const ARA::ARAContentTimeRange* /*range*/,
	ARA::ContentUpdateScopes /*scopeFlags*/) noexcept {
	/** Nothing To Do */
}

void ARAModelUpdateController::notifyPlaybackRegionContentChanged(
	ARA::ARAPlaybackRegionHostRef /*playbackRegionHostRef*/,
	const ARA::ARAContentTimeRange* /*range*/,
	ARA::ContentUpdateScopes /*scopeFlags*/) noexcept {
	/** Nothing To Do */
}

void ARAModelUpdateController::notifyDocumentDataChanged() noexcept {
	/** TODO */
}

void ARAPlaybackController::requestStartPlayback() noexcept {
	AudioCore::getInstance()->play();
}

void ARAPlaybackController::requestStopPlayback() noexcept {
	AudioCore::getInstance()->stop();
}

void ARAPlaybackController::requestSetPlaybackPosition(
	ARA::ARATimePosition timePosition) noexcept {
	AudioCore::getInstance()->setPositon(timePosition);
}

void ARAPlaybackController::requestSetCycleRange(
	ARA::ARATimePosition startTime,
	ARA::ARATimeDuration duration) noexcept {
	this->loopStartTime = startTime;
	this->loopEndTime = startTime + duration;

	if (this->shouldLoop) {
		AudioCore::getInstance()->setLoop(
			this->loopStartTime, this->loopEndTime);
	}
}

void ARAPlaybackController::requestEnableCycle(bool enable) noexcept {
	this->shouldLoop = enable;

	if (enable) {
		AudioCore::getInstance()->setLoop(
			this->loopStartTime, this->loopEndTime);
	}
	else {
		AudioCore::getInstance()->setLoop(-1, -1);
	}
}

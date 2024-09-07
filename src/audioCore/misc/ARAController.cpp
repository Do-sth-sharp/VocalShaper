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
	ARA::ARAArchiveReaderHostRef archiveReaderHostRef) noexcept {
	/** Nothing To Do */
	return nullptr;
}

bool ARAContentAccessController::isMusicalContextContentAvailable(
	ARA::ARAMusicalContextHostRef musicalContextHostRef,
	ARA::ARAContentType /*type*/) noexcept {
	return true;
}

ARA::ARAContentGrade ARAContentAccessController::getMusicalContextContentGrade(
	ARA::ARAMusicalContextHostRef musicalContextHostRef,
	ARA::ARAContentType /*type*/) noexcept {
	return ARA::kARAContentGradeInitial;
}

ARA::ARAContentReaderHostRef ARAContentAccessController::createMusicalContextContentReader(
	ARA::ARAMusicalContextHostRef musicalContextHostRef,
	ARA::ARAContentType /*type*/, const ARA::ARAContentTimeRange* /*range*/) noexcept {
	auto contextReader = std::make_unique<ContextReader>(musicalContextHostRef, nullptr);
	auto audioReaderHostRef = Converter::toHostRef(contextReader.get());

	this->contextReaders.emplace(contextReader.get(), std::move(contextReader));

	return audioReaderHostRef;
}

bool ARAContentAccessController::isAudioSourceContentAvailable(
	ARA::ARAAudioSourceHostRef audioSourceHostRef,
	ARA::ARAContentType /*type*/) noexcept {
	return true;
}

ARA::ARAContentGrade ARAContentAccessController::getAudioSourceContentGrade(
	ARA::ARAAudioSourceHostRef audioSourceHostRef,
	ARA::ARAContentType /*type*/) noexcept {
	return ARA::kARAContentGradeInitial;
}

ARA::ARAContentReaderHostRef ARAContentAccessController::createAudioSourceContentReader(
	ARA::ARAAudioSourceHostRef audioSourceHostRef,
	ARA::ARAContentType /*type*/, const ARA::ARAContentTimeRange* /*range*/) noexcept {
	auto contextReader = std::make_unique<ContextReader>(nullptr, audioSourceHostRef);
	auto audioReaderHostRef = Converter::toHostRef(contextReader.get());

	this->contextReaders.emplace(contextReader.get(), std::move(contextReader));

	return audioReaderHostRef;
}

ARA::ARAInt32 ARAContentAccessController::getContentReaderEventCount(
	ARA::ARAContentReaderHostRef contentReaderHostRef) noexcept {
	/** TODO */
	return 0;
}

const void* ARAContentAccessController::getContentReaderDataForEvent(
	ARA::ARAContentReaderHostRef contentReaderHostRef,
	ARA::ARAInt32 eventIndex) noexcept {
	/** TODO */
	return nullptr;
}

void ARAContentAccessController::destroyContentReader(
	ARA::ARAContentReaderHostRef contentReaderHostRef) noexcept {
	this->contextReaders.erase(Converter::fromHostRef(contentReaderHostRef));
}

void ARAModelUpdateController::notifyAudioSourceAnalysisProgress(
	ARA::ARAAudioSourceHostRef audioSourceHostRef,
	ARA::ARAAnalysisProgressState state, float value) noexcept {
	/** TODO */
}

void ARAModelUpdateController::notifyAudioSourceContentChanged(
	ARA::ARAAudioSourceHostRef audioSourceHostRef,
	const ARA::ARAContentTimeRange* range,
	ARA::ContentUpdateScopes scopeFlags) noexcept {
	/** TODO */
}

void ARAModelUpdateController::notifyAudioModificationContentChanged(
	ARA::ARAAudioModificationHostRef audioModificationHostRef,
	const ARA::ARAContentTimeRange* range,
	ARA::ContentUpdateScopes scopeFlags) noexcept {
	/** TODO */
}

void ARAModelUpdateController::notifyPlaybackRegionContentChanged(
	ARA::ARAPlaybackRegionHostRef playbackRegionHostRef,
	const ARA::ARAContentTimeRange* range,
	ARA::ContentUpdateScopes scopeFlags) noexcept {
	/** TODO */
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

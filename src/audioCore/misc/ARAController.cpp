#include "ARAController.h"

ARAAudioAccessController::ARAAudioAccessController(
	juce::AudioProcessor* track)
	: track(track) {}

ARA::ARAAudioReaderHostRef ARAAudioAccessController::createAudioReaderForSource(
	ARA::ARAAudioSourceHostRef audioSourceHostRef,
	bool use64BitSamples) noexcept {
	/** TODO */
	return nullptr;
}

bool ARAAudioAccessController::readAudioSamples(
	ARA::ARAAudioReaderHostRef audioReaderHostRef,
	ARA::ARASamplePosition samplePosition,
	ARA::ARASampleCount samplesPerChannel,
	void* const buffers[]) noexcept {
	/** TODO */
	return false;
}

void ARAAudioAccessController::destroyAudioReader(
	ARA::ARAAudioReaderHostRef audioReaderHostRef) noexcept {
	/** TODO */
}

ARAArchivingController::ARAArchivingController(
	juce::AudioProcessor* track)
	: track(track) {}

ARA::ARASize ARAArchivingController::getArchiveSize(
	ARA::ARAArchiveReaderHostRef archiveReaderHostRef) noexcept {
	/** TODO */
	return 0;
}

bool ARAArchivingController::readBytesFromArchive(
	ARA::ARAArchiveReaderHostRef archiveReaderHostRef,
	ARA::ARASize position, ARA::ARASize length,
	ARA::ARAByte buffer[]) noexcept {
	/** TODO */
	return false;
}

bool ARAArchivingController::writeBytesToArchive(
	ARA::ARAArchiveWriterHostRef archiveWriterHostRef,
	ARA::ARASize position, ARA::ARASize length,
	const ARA::ARAByte buffer[]) noexcept {
	/** TODO */
	return false;
}

void ARAArchivingController::notifyDocumentArchivingProgress(
	float value) noexcept {
	/** TODO */
}

void ARAArchivingController::notifyDocumentUnarchivingProgress(
	float value) noexcept {
	/** TODO */
}

ARA::ARAPersistentID ARAArchivingController::getDocumentArchiveID(
	ARA::ARAArchiveReaderHostRef archiveReaderHostRef) noexcept {
	/** TODO */
	return "";
}

ARAContentAccessController::ARAContentAccessController(
	juce::AudioProcessor* track)
	: track(track) {}

bool ARAContentAccessController::isMusicalContextContentAvailable(
	ARA::ARAMusicalContextHostRef musicalContextHostRef,
	ARA::ARAContentType type) noexcept {
	/** TODO */
	return false;
}

ARA::ARAContentGrade ARAContentAccessController::getMusicalContextContentGrade(
	ARA::ARAMusicalContextHostRef musicalContextHostRef,
	ARA::ARAContentType type) noexcept {
	/** TODO */
	return 0;
}

ARA::ARAContentReaderHostRef ARAContentAccessController::createMusicalContextContentReader(
	ARA::ARAMusicalContextHostRef musicalContextHostRef,
	ARA::ARAContentType type, const ARA::ARAContentTimeRange* range) noexcept {
	/** TODO */
	return nullptr;
}

bool ARAContentAccessController::isAudioSourceContentAvailable(
	ARA::ARAAudioSourceHostRef audioSourceHostRef,
	ARA::ARAContentType type) noexcept {
	/** TODO */
	return false;
}

ARA::ARAContentGrade ARAContentAccessController::getAudioSourceContentGrade(
	ARA::ARAAudioSourceHostRef audioSourceHostRef,
	ARA::ARAContentType type) noexcept {
	/** TODO */
	return 0;
}

ARA::ARAContentReaderHostRef ARAContentAccessController::createAudioSourceContentReader(
	ARA::ARAAudioSourceHostRef audioSourceHostRef,
	ARA::ARAContentType type, const ARA::ARAContentTimeRange* range) noexcept {
	/** TODO */
	return nullptr;
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
	/** TODO */
}

ARAModelUpdateController::ARAModelUpdateController(
	juce::AudioProcessor* track)
	: track(track) {}

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

ARAPlaybackController::ARAPlaybackController(
	juce::AudioProcessor* track)
	: track(track) {}

void ARAPlaybackController::requestStartPlayback() noexcept {
	/** TODO */
}

void ARAPlaybackController::requestStopPlayback() noexcept {
	/** TODO */
}

void ARAPlaybackController::requestSetPlaybackPosition(
	ARA::ARATimePosition timePosition) noexcept {
	/** TODO */
}

void ARAPlaybackController::requestSetCycleRange(
	ARA::ARATimePosition startTime,
	ARA::ARATimeDuration duration) noexcept {
	/** TODO */
}

void ARAPlaybackController::requestEnableCycle(bool enable) noexcept {
	/** TODO */
}


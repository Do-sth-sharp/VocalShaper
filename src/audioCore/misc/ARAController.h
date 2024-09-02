#pragma once

#include <JuceHeader.h>
#include "ARAObjects.h"

class ARAAudioAccessController : public ARA::Host::AudioAccessControllerInterface {
public:
	ARAAudioAccessController() = default;

private:
	ARA::ARAAudioReaderHostRef createAudioReaderForSource(
		ARA::ARAAudioSourceHostRef audioSourceHostRef,
		bool use64BitSamples) noexcept override;
	bool readAudioSamples(
		ARA::ARAAudioReaderHostRef audioReaderHostRef, 
		ARA::ARASamplePosition samplePosition, 
		ARA::ARASampleCount samplesPerChannel, 
		void* const buffers[]) noexcept override;
	void destroyAudioReader(
		ARA::ARAAudioReaderHostRef audioReaderHostRef) noexcept override;

private:
	struct AudioReader {
		AudioReader(ARA::ARAAudioSourceHostRef source, bool use64BitSamples)
			: sourceHostRef(source), use64Bit(use64BitSamples) {}

		ARA::ARAAudioSourceHostRef sourceHostRef;
		bool use64Bit;
	};

	using Converter = juce::ARAHostModel::ConversionFunctions<AudioReader*, ARA::ARAAudioReaderHostRef>;
	using SourceConverter = juce::ARAHostModel::ConversionFunctions<ARAVirtualAudioSource*, ARA::ARAAudioSourceHostRef>;

	std::map<AudioReader*, std::unique_ptr<AudioReader>> audioReaders;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAAudioAccessController)
};

class ARAArchivingController : public ARA::Host::ArchivingControllerInterface {
public:
	ARAArchivingController() = default;

private:
	ARA::ARASize getArchiveSize(
		ARA::ARAArchiveReaderHostRef archiveReaderHostRef) noexcept override;
	bool readBytesFromArchive(
		ARA::ARAArchiveReaderHostRef archiveReaderHostRef,
		ARA::ARASize position, ARA::ARASize length,
		ARA::ARAByte buffer[]) noexcept override;
	bool writeBytesToArchive(
		ARA::ARAArchiveWriterHostRef archiveWriterHostRef,
		ARA::ARASize position, ARA::ARASize length,
		const ARA::ARAByte buffer[]) noexcept override;
	void notifyDocumentArchivingProgress(
		float value) noexcept override;
	void notifyDocumentUnarchivingProgress(
		float value) noexcept override;
	ARA::ARAPersistentID getDocumentArchiveID(
		ARA::ARAArchiveReaderHostRef archiveReaderHostRef) noexcept override;

private:
	using ReaderConverter = juce::ARAHostModel::ConversionFunctions<juce::MemoryBlock*, ARA::ARAArchiveReaderHostRef>;
	using WriterConverter = juce::ARAHostModel::ConversionFunctions<juce::MemoryOutputStream*, ARA::ARAArchiveWriterHostRef>;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAArchivingController)
};

class ARAContentAccessController : public ARA::Host::ContentAccessControllerInterface {
public:
	ARAContentAccessController() = default;

private:
	bool isMusicalContextContentAvailable(
		ARA::ARAMusicalContextHostRef musicalContextHostRef,
		ARA::ARAContentType type) noexcept override;
	ARA::ARAContentGrade getMusicalContextContentGrade(
		ARA::ARAMusicalContextHostRef musicalContextHostRef,
		ARA::ARAContentType type) noexcept override;
	ARA::ARAContentReaderHostRef createMusicalContextContentReader(
		ARA::ARAMusicalContextHostRef musicalContextHostRef,
		ARA::ARAContentType type, const ARA::ARAContentTimeRange* range) noexcept override;
	bool isAudioSourceContentAvailable(
		ARA::ARAAudioSourceHostRef audioSourceHostRef,
		ARA::ARAContentType type) noexcept override;
	ARA::ARAContentGrade getAudioSourceContentGrade(
		ARA::ARAAudioSourceHostRef audioSourceHostRef,
		ARA::ARAContentType type) noexcept override;
	ARA::ARAContentReaderHostRef createAudioSourceContentReader(
		ARA::ARAAudioSourceHostRef audioSourceHostRef,
		ARA::ARAContentType type, const ARA::ARAContentTimeRange* range) noexcept override;
	ARA::ARAInt32 getContentReaderEventCount(
		ARA::ARAContentReaderHostRef contentReaderHostRef) noexcept override;
	const void* getContentReaderDataForEvent(
		ARA::ARAContentReaderHostRef contentReaderHostRef,
		ARA::ARAInt32 eventIndex) noexcept override;
	void destroyContentReader(
		ARA::ARAContentReaderHostRef contentReaderHostRef) noexcept override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAContentAccessController)
};

class ARAModelUpdateController : public ARA::Host::ModelUpdateControllerInterface {
public:
	ARAModelUpdateController() = default;

private:
	void notifyAudioSourceAnalysisProgress(
		ARA::ARAAudioSourceHostRef audioSourceHostRef,
		ARA::ARAAnalysisProgressState state, float value) noexcept override;
	void notifyAudioSourceContentChanged(
		ARA::ARAAudioSourceHostRef audioSourceHostRef,
		const ARA::ARAContentTimeRange* range,
		ARA::ContentUpdateScopes scopeFlags) noexcept override;
	void notifyAudioModificationContentChanged(
		ARA::ARAAudioModificationHostRef audioModificationHostRef,
		const ARA::ARAContentTimeRange* range,
		ARA::ContentUpdateScopes scopeFlags) noexcept override;
	void notifyPlaybackRegionContentChanged(
		ARA::ARAPlaybackRegionHostRef playbackRegionHostRef,
		const ARA::ARAContentTimeRange* range,
		ARA::ContentUpdateScopes scopeFlags) noexcept override;
	void notifyDocumentDataChanged() noexcept override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAModelUpdateController)
};

class ARAPlaybackController : public ARA::Host::PlaybackControllerInterface {
public:
	ARAPlaybackController() = default;

private:
	void requestStartPlayback() noexcept override;
	void requestStopPlayback() noexcept override;
	void requestSetPlaybackPosition(
		ARA::ARATimePosition timePosition) noexcept override;
	void requestSetCycleRange(
		ARA::ARATimePosition startTime,
		ARA::ARATimeDuration duration) noexcept override;
	void requestEnableCycle(bool enable) noexcept override;

private:
	double loopStartTime = -1, loopEndTime = -1;
	bool shouldLoop = false;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAPlaybackController)
};

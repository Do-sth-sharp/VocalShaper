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

	using ReaderConverter = juce::ARAHostModel::ConversionFunctions<juce::MemoryBlock*, ARA::ARAArchiveReaderHostRef>;
	using WriterConverter = juce::ARAHostModel::ConversionFunctions<juce::MemoryOutputStream*, ARA::ARAArchiveWriterHostRef>;

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
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAArchivingController)
};

class ARAContentAccessController : public ARA::Host::ContentAccessControllerInterface {
public:
	ARAContentAccessController() = default;
	~ARAContentAccessController() = default;

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
	struct ContextReader {
		ContextReader() = delete;
		ContextReader(
			ARAExtension::ARAContentType type,
			ARA::ARAMusicalContextHostRef context)
			: type(type), contextHostRef(context) {}

		const ARAExtension::ARAContentType type = ARAExtension::ARAContentTypeUnknown;
		const ARA::ARAMusicalContextHostRef contextHostRef;
	};

	using Converter = juce::ARAHostModel::ConversionFunctions<ContextReader*, ARA::ARAContentReaderHostRef>;
	using ContextConverter = juce::ARAHostModel::ConversionFunctions<ARAVirtualMusicalContext*, ARA::ARAMusicalContextHostRef>;

	std::map<ContextReader*, std::unique_ptr<ContextReader>> contextReaders;

	const std::unordered_set<ARAExtension::ARAContentType> allowedContentTypes{
		ARAExtension::ARAContentTypeUnknown,
		ARAExtension::ARAContentTypeTempoEntry,
		ARAExtension::ARAContentTypeBarSignature,
		//ARAExtension::ARAContentTypeKeySignature,
		ARAExtension::ARAContentTypeNote,
		ARAExtension::ARAContentTypeNotePlus,
		ARAExtension::ARAContentTypeSustainPedal,
		ARAExtension::ARAContentTypeSostenutoPedal,
		ARAExtension::ARAContentTypeSoftPedal,
		ARAExtension::ARAContentTypePitchWheel,
		ARAExtension::ARAContentTypeAfterTouch,
		ARAExtension::ARAContentTypeChannelPressure,
		ARAExtension::ARAContentTypeController,
		ARAExtension::ARAContentTypeMisc
	};

	/** Shit! Can't using union because of the destructor auto deletion. Blame to ARA. */

	ARA::ARAContentNote noteTemp;
	ARA::ARAContentTempoEntry tempoTemp;
	ARA::ARAContentBarSignature barTemp;
	ARA::ARAContentKeySignature keyTemp;

	ARAExtension::ARAContentNote notePlusTemp;
	ARAExtension::ARAContentPedal pedalTemp;
	ARAExtension::ARAContentIntParam intParamTemp;
	ARAExtension::ARAContentAfterTouch afterTouchTemp;
	ARAExtension::ARAContentController controllerTemp;
	ARAExtension::ARAContentMisc miscTemp;

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

#pragma once

#include <JuceHeader.h>

class ARAAudioAccessController : public ARA::Host::AudioAccessControllerInterface {
public:
	ARAAudioAccessController(juce::AudioProcessor* track);

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
	juce::AudioProcessor* const track = nullptr;

	struct AudioReader {
		AudioReader(ARA::ARAAudioSourceHostRef source, bool use64BitSamples)
			: sourceHostRef(source), use64Bit(use64BitSamples) {}

		ARA::ARAAudioSourceHostRef sourceHostRef;
		bool use64Bit;
	};

	using Converter = juce::ARAHostModel::ConversionFunctions<AudioReader*, ARA::ARAAudioReaderHostRef>;
	using SourceConverter = juce::ARAHostModel::ConversionFunctions<juce::AudioSampleBuffer*, ARA::ARAAudioSourceHostRef>;

	std::map<AudioReader*, std::unique_ptr<AudioReader>> audioReaders;
};

class ARAArchivingController : public ARA::Host::ArchivingControllerInterface {
public:
	ARAArchivingController(juce::AudioProcessor* track);

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
	juce::AudioProcessor* const track = nullptr;
};

class ARAContentAccessController : public ARA::Host::ContentAccessControllerInterface {
public:
	ARAContentAccessController(juce::AudioProcessor* track);

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
	juce::AudioProcessor* const track = nullptr;
};

class ARAModelUpdateController : public ARA::Host::ModelUpdateControllerInterface {
public:
	ARAModelUpdateController(juce::AudioProcessor* track);

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
	juce::AudioProcessor* const track = nullptr;
};

class ARAPlaybackController : public ARA::Host::PlaybackControllerInterface {
public:
	ARAPlaybackController(juce::AudioProcessor* track);

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
	juce::AudioProcessor* const track = nullptr;

	double loopStartTime = -1, loopEndTime = -1;
	bool shouldLoop = false;
};

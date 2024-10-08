#pragma once

#include <JuceHeader.h>
#include "ARAObjects.h"
#include "ARAChangeListener.h"

class SeqSourceProcessor;

class ARAVirtualDocument {
public:
	using PluginOnOffFunc = std::function<void(bool)>;

	ARAVirtualDocument() = delete;
	ARAVirtualDocument(SeqSourceProcessor* seq,
		ARA::Host::DocumentController& controller,
		juce::ARAHostModel::EditorRendererInterface& araEditorRenderer,
		juce::ARAHostModel::PlaybackRendererInterface& araPlaybackRenderer,
		const PluginOnOffFunc& pluginOnOff);
	~ARAVirtualDocument();

	void updateRegions();
	void updateAudioAndContext();
	void updateTrackBaseInfo();
	void init();
	void clear();

	juce::ChangeListener* getRegionListener() const;
	juce::ChangeListener* getContextListener() const;
	juce::ChangeListener* getTrackInfoListener() const;

	void storeToStream(juce::MemoryOutputStream& stream) const;
	void restoreFromBlock(juce::MemoryBlock& block);

private:
	SeqSourceProcessor* const seq = nullptr;
	ARA::Host::DocumentController& controller;
	juce::ARAHostModel::EditorRendererInterface& araEditorRenderer;
	juce::ARAHostModel::PlaybackRendererInterface& araPlaybackRenderer;

	const PluginOnOffFunc pluginOnOff;
	int pluginLockCount = 0;
	juce::SpinLock pluginLockMutex;

	std::unique_ptr<ARAVirtualAudioSource> audioSource = nullptr;
	std::unique_ptr<ARAVirtualAudioModification> audioModification = nullptr;
	std::unique_ptr<ARAVirtualMusicalContext> musicalContext = nullptr;
	std::unique_ptr<ARAVirtualRegionSequence> regionSequence = nullptr;
	juce::OwnedArray<ARAVirtualPlaybackRegion> playbackRegions;

	std::unique_ptr<ARARegionChangeListener> regionListener = nullptr;
	std::unique_ptr<ARAContextChangeListener> contextListener = nullptr;
	std::unique_ptr<ARATrackInfoChangeListener> infoListener = nullptr;

	static void initStoreFilter(
		ARA::ARAStoreObjectsFilter& filter,
		ARAVirtualAudioSource* source, ARAVirtualAudioModification* modification);
	static void initRestoreFilter(
		ARA::ARARestoreObjectsFilter& filter,
		ARAVirtualAudioSource* source, ARAVirtualAudioModification* modification);
	static void destoryStoreFilter(ARA::ARAStoreObjectsFilter& filter);
	static void destoryRestoreFilter(ARA::ARARestoreObjectsFilter& filter);

	bool supportsPartialPersistency() const;
	bool restoreObjectsFromArchive(
		ARA::ARAArchiveReaderHostRef archiveReaderHostRef,
		const ARA::ARARestoreObjectsFilter* filter);
	bool storeObjectsToArchive(
		ARA::ARAArchiveWriterHostRef archiveWriterHostRef,
		const ARA::ARAStoreObjectsFilter* filter) const;

	void clearUnsafe();
	void clearRegionsUnsafe();
	void removeRegionsFromRenderer();
	void addRegionsToRenderer();

	void lockPlugin(bool locked);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualDocument)
};

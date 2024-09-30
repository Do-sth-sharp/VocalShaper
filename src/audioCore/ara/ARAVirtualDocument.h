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
	void update();
	void clear();

	juce::ChangeListener* getListener() const;
	juce::ChangeListener* getRegionListener() const;

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

	std::unique_ptr<ARAChangeListener> listener = nullptr;
	std::unique_ptr<ARARegionChangeListener> regionListener = nullptr;

	void clearUnsafe();
	void clearRegionsUnsafe();
	void removeRegionsFromRenderer();
	void addRegionsToRenderer();

	void lockPlugin(bool locked);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualDocument)
};

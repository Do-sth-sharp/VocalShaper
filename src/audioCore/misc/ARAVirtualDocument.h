#pragma once

#include <JuceHeader.h>
#include "ARAObjects.h"

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

	void update();
	void clear();

private:
	SeqSourceProcessor* const seq = nullptr;
	ARA::Host::DocumentController& controller;
	juce::ARAHostModel::EditorRendererInterface& araEditorRenderer;
	juce::ARAHostModel::PlaybackRendererInterface& araPlaybackRenderer;
	const PluginOnOffFunc pluginOnOff;

	std::unique_ptr<ARAVirtualAudioSource> audioSource = nullptr;
	std::unique_ptr<ARAVirtualAudioModification> audioModification = nullptr;
	juce::OwnedArray<ARAVirtualMusicalContext> musicalContexts;
	juce::OwnedArray<ARAVirtualRegionSequence> regionSequences;
	juce::OwnedArray<ARAVirtualPlaybackRegion> playbackRegions;

	void clearUnsafe();
	void removeRegions();
	void addRegions();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAVirtualDocument)
};

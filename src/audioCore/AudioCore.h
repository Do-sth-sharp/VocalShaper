#pragma once

#include <Defs.h>

class AudioCore final : public juce::DeletedAtShutdown {
public:
	AudioCore();

private:
	std::unique_ptr<juce::AudioProcessorGraph> mainAudioGraph = nullptr;
	juce::AudioProcessorGraph::Node::Ptr sequencer;
	juce::AudioProcessorGraph::Node::Ptr mixer;

private:
	static AudioCore* getInstance();
	static AudioCore* instance;
};

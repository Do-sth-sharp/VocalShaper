#pragma once

#include <Defs.h>

class AudioCore final : public juce::DeletedAtShutdown {
public:
	AudioCore();

private:
	std::unique_ptr<juce::AudioDeviceManager> audioDeviceManager = nullptr;
	std::unique_ptr<juce::AudioProcessorGraph> mainAudioGraph = nullptr;
	std::unique_ptr<juce::AudioProcessorPlayer> mainGraphPlayer = nullptr;

	juce::AudioProcessorGraph::Node::Ptr sequencer;
	juce::AudioProcessorGraph::Node::Ptr mixer;
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode;

	void initAudioDevice();
	void updateAudioBuses();

private:
	static AudioCore* getInstance();
	static AudioCore* instance;
};

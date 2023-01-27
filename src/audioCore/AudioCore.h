#pragma once

#include <Defs.h>

class AudioCore final : public juce::DeletedAtShutdown {
public:
	AudioCore();

private:
	std::unique_ptr<juce::AudioDeviceManager> audioDeviceManager = nullptr;
	std::unique_ptr<juce::AudioProcessorGraph> mainAudioGraph = nullptr;
	std::unique_ptr<juce::AudioProcessorPlayer> mainGraphPlayer = nullptr;

	void initAudioDevice();
	void updateAudioBuses();

private:
	static AudioCore* getInstance();
	static AudioCore* instance;
};

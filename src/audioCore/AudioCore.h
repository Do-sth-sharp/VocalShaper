#pragma once

#include <Defs.h>

class AudioCore final : public juce::DeletedAtShutdown {
public:
	AudioCore();

public:
	enum class AudioDeviceType {
		CoreAudio,
		IOSAudio,
		WASAPIShared,
		WASAPIExclusive,
		WASAPISharedLowLatency,
		DirectSound,
		ASIO,
		ALSA,
		JACK,
		Android,
		OpenSLES,
		Oboe,
		Bela
	};

	static const juce::StringArray getAudioDeviceList(AudioDeviceType type);
	static const juce::StringArray getAllAudioDeviceList();

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

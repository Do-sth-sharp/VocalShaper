#pragma once

#include <Defs.h>

class AudioDeviceChangeListener;

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

	void setAudioInputDevice(const juce::String& deviceName);
	void setAudioOutputDevice(const juce::String& deviceName);

private:
	std::unique_ptr<juce::AudioDeviceManager> audioDeviceManager = nullptr;
	std::unique_ptr<juce::AudioProcessorGraph> mainAudioGraph = nullptr;
	std::unique_ptr<juce::AudioProcessorPlayer> mainGraphPlayer = nullptr;

	friend class AudioDeviceChangeListener;
	void initAudioDevice();
	void updateAudioBuses();

	std::unique_ptr<AudioDeviceChangeListener> audioDeviceListener = nullptr;

private:
	static AudioCore* getInstance();
	static AudioCore* instance;
};

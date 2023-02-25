#pragma once

#include <JuceHeader.h>
#include "AudioCommand.h"

class AudioDeviceChangeListener;

class AudioCore final : public juce::DeletedAtShutdown {
public:
	AudioCore();
	~AudioCore() override;

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

	static const juce::StringArray getAudioDeviceList(AudioDeviceType type, bool isInput = false);
	static const juce::StringArray getAllAudioDeviceList(bool isInput = false);

	static const juce::StringArray getAllAudioInputDeviceList();
	static const juce::StringArray getAllAudioOutputDeviceList();

	const juce::String setAudioInputDevice(const juce::String& deviceName);
	const juce::String setAudioOutputDevice(const juce::String& deviceName);
	const juce::String getAudioInputDeviceName() const;
	const juce::String getAudioOutputDeviceName() const;

	void setCurrentAudioDeviceType(const juce::String& typeName);
	const juce::String getCurrentAudioDeivceType() const;

	const juce::String setAudioSampleRate(double value);
	const juce::String setAudioBufferSize(int value);
	double getAudioSampleRate() const;
	int getAudioBufferSize() const;

	void playTestSound() const;

	void setMIDIInputDeviceEnabled(const juce::String& deviceID, bool enabled);
	void setMIDIOutputDevice(const juce::String& deviceID);

	static const juce::StringArray getAllMIDIDeviceList(bool isInput = false);

	static const juce::StringArray getAllMIDIInputDeviceList();
	static const juce::StringArray getAllMIDIOutputDeviceList();

	juce::Component* getAudioDebugger() const;

private:
	friend class AudioDebugger;
	friend class CommandParser;
	std::unique_ptr<juce::AudioDeviceManager> audioDeviceManager = nullptr;
	std::unique_ptr<juce::AudioProcessorGraph> mainAudioGraph = nullptr;
	std::unique_ptr<juce::AudioProcessorPlayer> mainGraphPlayer = nullptr;
	std::unique_ptr<juce::Component> audioDebugger = nullptr;
	std::unique_ptr<AudioCommand> commandProcessor = nullptr;

	friend class AudioDeviceChangeListener;
	void initAudioDevice();
	void updateAudioBuses();

	std::unique_ptr<AudioDeviceChangeListener> audioDeviceListener = nullptr;

public:
	static AudioCore* getInstance();

private:
	static AudioCore* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCore)
};

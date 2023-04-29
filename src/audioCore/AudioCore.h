#pragma once

#include <JuceHeader.h>
#include "AudioPluginSearchThread.h"
#include "AudioConfig.h"
#include "MainGraph.h"

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
	bool getMIDIInputDeviceEnabled(const juce::String& deviceID) const;
	const juce::String getMIDIOutputDevice() const;

	static const juce::StringArray getAllMIDIDeviceList(bool isInput = false);

	static const juce::StringArray getAllMIDIInputDeviceList();
	static const juce::StringArray getAllMIDIOutputDeviceList();

	juce::Component* getAudioDebugger() const;
	juce::Component* getMIDIDebugger() const;

	const juce::StringArray getPluginTypeList() const;
	const std::tuple<bool, juce::KnownPluginList&> getPluginList() const;
	void clearPluginList();
	void clearPluginTemporary();
	bool pluginSearchThreadIsRunning() const;

	/**
	 * @attention	Don't change plugin black list while searching plugin.
	 */
	const juce::StringArray getPluginBlackList() const;
	/**
	 * @attention	Don't change plugin black list while searching plugin.
	 */
	void addToPluginBlackList(const juce::String& plugin) const;
	/**
	 * @attention	Don't change plugin black list while searching plugin.
	 */
	void removeFromPluginBlackList(const juce::String& plugin) const;

	/**
	 * @attention	Don't change plugin search path while searching plugin.
	 */
	const juce::StringArray getPluginSearchPath() const;
	/**
	 * @attention	Don't change plugin search path while searching plugin.
	 */
	void addToPluginSearchPath(const juce::String& path) const;
	/**
	 * @attention	Don't change plugin search path while searching plugin.
	 */
	void removeFromPluginSearchPath(const juce::String& path) const;

	MainGraph* getGraph() const;

private:
	friend class AudioDebugger;
	friend class CommandBase;
	std::unique_ptr<juce::AudioDeviceManager> audioDeviceManager = nullptr;
	std::unique_ptr<MainGraph> mainAudioGraph = nullptr;
	std::unique_ptr<juce::AudioProcessorPlayer> mainGraphPlayer = nullptr;
	std::unique_ptr<juce::Component> audioDebugger = nullptr;
	std::unique_ptr<juce::Component> midiDebugger = nullptr;
	std::unique_ptr<AudioPluginSearchThread> audioPluginSearchThread = nullptr;
	std::unique_ptr<juce::AudioPlayHead> playHead = nullptr;

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

#pragma once

#include <JuceHeader.h>

class Device final : private juce::DeletedAtShutdown {
public:
	Device();
	~Device();

	void addChangeListener(juce::ChangeListener* listener);

	juce::String initialise(int numInputChannelsNeeded,
		int numOutputChannelsNeeded,
		const juce::XmlElement* savedState,
		bool selectDefaultDeviceOnFailure,
		const juce::String& preferredDefaultDeviceName = juce::String{},
		const juce::AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions = nullptr);

	void setAudioCallback(juce::AudioIODeviceCallback* newCallback);
	void updateMidiCallback(juce::MidiInputCallback* callback);

	juce::AudioDeviceManager::AudioDeviceSetup getAudioDeviceSetup() const;

	juce::MidiOutput* getDefaultMidiOutput() const;

	const juce::OwnedArray<juce::AudioIODeviceType>& getAvailableDeviceTypes();
	juce::AudioIODevice* getCurrentAudioDevice() const;
	juce::String getCurrentAudioDeviceType() const;
	bool isMidiInputDeviceEnabled(const juce::String& deviceIdentifier) const;

	juce::AudioDeviceManager* getManager() const;

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

	static const juce::Array<juce::MidiDeviceInfo> getAllMIDIDevices(bool isInput = false);

	static const juce::Array<juce::MidiDeviceInfo> getAllMIDIInputDevices();
	static const juce::Array<juce::MidiDeviceInfo> getAllMIDIOutputDevices();

	static const juce::StringArray getAllMIDIDeviceList(bool isInput = false);

	static const juce::StringArray getAllMIDIInputDeviceList();
	static const juce::StringArray getAllMIDIOutputDeviceList();

private:
	std::unique_ptr<juce::AudioDeviceManager> audioDeviceManager = nullptr;
	juce::AudioIODeviceCallback* callback = nullptr;

public:
	static Device* getInstance();
	static void releaseInstance();

private:
	static Device* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Device)
};

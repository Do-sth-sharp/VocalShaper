#include "AudioCore.h"
#include "MainGraph.h"

#include "AudioDebugger.h"

class AudioDeviceChangeListener : public juce::ChangeListener {
public:
	AudioDeviceChangeListener(AudioCore* parent) : parent(parent) {};
	void changeListenerCallback(juce::ChangeBroadcaster* /*source*/) override {
		if (parent) {
			/** Update Audio Buses */
			parent->updateAudioBuses();
		}
	};

private:
	AudioCore* const parent = nullptr;
};

AudioCore::AudioCore() {
	/** Main Audio Device Manager */
	this->audioDeviceManager = std::make_unique<juce::AudioDeviceManager>();

	/** Main Audio Graph Of The Audio Core */
	this->mainAudioGraph = std::make_unique<MainGraph>();

	/** Main Graph Player */
	this->mainGraphPlayer = std::make_unique<juce::AudioProcessorPlayer>();

	/** Audio Device Listener */
	this->audioDeviceListener = std::make_unique<AudioDeviceChangeListener>(this);
	this->audioDeviceManager->addChangeListener(this->audioDeviceListener.get());

	/** Audio Debug */
	this->commandProcessor = std::make_unique<AudioCommand>(this);
	this->audioDebugger = std::make_unique<AudioDebugger>(this, this->commandProcessor.get());

	/** Init Audio Device */
	this->initAudioDevice();
}

AudioCore::~AudioCore() {
	this->audioDeviceManager->removeAllChangeListeners();
	this->audioDeviceManager->removeAudioCallback(this->mainGraphPlayer.get());
	this->mainGraphPlayer->setProcessor(nullptr);
}

const juce::StringArray AudioCore::getAudioDeviceList(AudioCore::AudioDeviceType type, bool isInput) {
	std::unique_ptr<juce::AudioIODeviceType> ptrType = nullptr;
	switch (type) {
	case AudioCore::AudioDeviceType::CoreAudio:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_CoreAudio());
		break;
	case AudioCore::AudioDeviceType::IOSAudio:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_iOSAudio());
		break;
	case AudioCore::AudioDeviceType::WASAPIShared:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_WASAPI(juce::WASAPIDeviceMode::shared));
		break;
	case AudioCore::AudioDeviceType::WASAPIExclusive:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_WASAPI(juce::WASAPIDeviceMode::exclusive));
		break;
	case AudioCore::AudioDeviceType::WASAPISharedLowLatency:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_WASAPI(juce::WASAPIDeviceMode::sharedLowLatency));
		break;
	case AudioCore::AudioDeviceType::DirectSound:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_DirectSound());
		break;
	case AudioCore::AudioDeviceType::ASIO:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_ASIO());
		break;
	case AudioCore::AudioDeviceType::ALSA:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_ALSA());
		break;
	case AudioCore::AudioDeviceType::JACK:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_JACK());
		break;
	case AudioCore::AudioDeviceType::Android:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_Android());
		break;
	case AudioCore::AudioDeviceType::OpenSLES:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_OpenSLES());
		break;
	case AudioCore::AudioDeviceType::Oboe:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_Oboe());
		break;
	case AudioCore::AudioDeviceType::Bela:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_Bela());
		break;
	default:
		break;
	}

	if (!ptrType) { return juce::StringArray(); }
	ptrType->scanForDevices();
	return ptrType->getDeviceNames(isInput);
}

const juce::StringArray AudioCore::getAllAudioDeviceList(bool isInput) {
	juce::StringArray result, resultTemp;

	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::CoreAudio, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[CoreAudio]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::IOSAudio, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[iOSAudio]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::WASAPIShared, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[WASAPI(shared)]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::WASAPIExclusive, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[WASAPI(exclusive)]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::WASAPISharedLowLatency, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[WASAPI(sharedLowLatency)]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::DirectSound, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[DirectSound]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::ASIO, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[ASIO]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::ALSA, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[ALSA]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::JACK, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[JACK]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::Android, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[Android]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::OpenSLES, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[OpenSLES]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::Oboe, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[Oboe]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::Bela, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[Bela]") + s);
	}

	return result;
}

const juce::StringArray AudioCore::getAllAudioInputDeviceList() {
	return AudioCore::getAllAudioDeviceList(true);
}

const juce::StringArray AudioCore::getAllAudioOutputDeviceList() {
	return AudioCore::getAllAudioDeviceList(false);
}

const juce::String  AudioCore::setAudioInputDevice(const juce::String& deviceName) {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	deviceSetup.inputDeviceName = deviceName;
	deviceSetup.useDefaultInputChannels = true;
	return this->audioDeviceManager->setAudioDeviceSetup(deviceSetup, true);
}

const juce::String  AudioCore::setAudioOutputDevice(const juce::String& deviceName) {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	deviceSetup.outputDeviceName = deviceName;
	deviceSetup.useDefaultOutputChannels = true;
	return this->audioDeviceManager->setAudioDeviceSetup(deviceSetup, true);
}

const juce::String AudioCore::getAudioInputDeviceName() const {
	auto audioSetup = this->audioDeviceManager->getAudioDeviceSetup();
	return audioSetup.inputDeviceName;
}

const juce::String AudioCore::getAudioOutputDeviceName() const {
	auto audioSetup = this->audioDeviceManager->getAudioDeviceSetup();
	return audioSetup.outputDeviceName;
}

void AudioCore::setCurrentAudioDeviceType(const juce::String& typeName) {
	this->audioDeviceManager->setCurrentAudioDeviceType(typeName, true);
}

const juce::String AudioCore::getCurrentAudioDeivceType() const {
	return this->audioDeviceManager->getCurrentAudioDeviceType();
}

const juce::String  AudioCore::setAudioSampleRate(double value) {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	deviceSetup.sampleRate = value;
	return this->audioDeviceManager->setAudioDeviceSetup(deviceSetup, true);
}

const juce::String  AudioCore::setAudioBufferSize(int value) {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	deviceSetup.bufferSize = value;
	return this->audioDeviceManager->setAudioDeviceSetup(deviceSetup, true);
}

double AudioCore::getAudioSampleRate() const {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	return deviceSetup.sampleRate;
}

int AudioCore::getAudioBufferSize() const {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	return deviceSetup.bufferSize;
}

void AudioCore::playTestSound() const {
	this->audioDeviceManager->playTestSound();
}

void AudioCore::setMIDIInputDeviceEnabled(const juce::String& deviceID, bool enabled) {
	this->audioDeviceManager->setMidiInputDeviceEnabled(deviceID, enabled);
}

void AudioCore::setMIDIOutputDevice(const juce::String& deviceID) {
	this->audioDeviceManager->setDefaultMidiOutputDevice(deviceID);
}

bool AudioCore::getMIDIInputDeviceEnabled(const juce::String& deviceID) const {
	return this->audioDeviceManager->isMidiInputDeviceEnabled(deviceID);
}

const juce::String AudioCore::getMIDIOutputDevice() const {
	return this->audioDeviceManager->getDefaultMidiOutputIdentifier();
}

const juce::StringArray AudioCore::getAllMIDIDeviceList(bool isInput) {
	auto list = isInput
		? juce::MidiInput::getAvailableDevices()
		: juce::MidiOutput::getAvailableDevices();

	juce::StringArray result;
	for (auto& i : list) {
		result.add(i.name);
	}
	return result;
}

const juce::StringArray AudioCore::getAllMIDIInputDeviceList() {
	return AudioCore::getAllMIDIDeviceList(true);
}

const juce::StringArray AudioCore::getAllMIDIOutputDeviceList() {
	return AudioCore::getAllMIDIDeviceList(false);
}

juce::Component* AudioCore::getAudioDebugger() const {
	return this->audioDebugger.get();
}

void AudioCore::initAudioDevice() {
	/** Init With Default Device */
	this->audioDeviceManager->initialise(1, 2, nullptr, true);

	/** Add Main Graph To Main Player */
	this->mainGraphPlayer->setProcessor(this->mainAudioGraph.get());

	/** Add Player To Default Device */
	this->audioDeviceManager->addAudioCallback(this->mainGraphPlayer.get());

	/** Update Audio Buses */
	this->updateAudioBuses();
}

void AudioCore::updateAudioBuses() {
	/** Link Audio Bus To Sequencer And Mixer */
	auto mainGraph = dynamic_cast<MainGraph*>(this->mainAudioGraph.get());
	if (mainGraph) {
		/** Get Input Channel Num */
		auto audioDeviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
		auto inputChannelNum = audioDeviceSetup.inputChannels.toInteger();
		auto outputChannelNum = audioDeviceSetup.outputChannels.toInteger();

		/** Set Buses Layout Of Main Graph */
		mainGraph->setAudioLayout(inputChannelNum, outputChannelNum);
	}

	/* Add MIDI Callback  */
	{
		auto midiInputDevice = juce::MidiInput::getAvailableDevices();
		for (auto& i : midiInputDevice) {
			if (this->audioDeviceManager->isMidiInputDeviceEnabled(i.identifier)) {
				this->audioDeviceManager->addMidiInputDeviceCallback(
					i.identifier, this->mainGraphPlayer.get());
			}
			else {
				this->audioDeviceManager->removeMidiInputDeviceCallback(
					i.identifier, this->mainGraphPlayer.get());
			}
		}
	}
}

AudioCore* AudioCore::getInstance() {
	return AudioCore::instance ? AudioCore::instance : (AudioCore::instance = new AudioCore());
}

AudioCore* AudioCore::instance = nullptr;

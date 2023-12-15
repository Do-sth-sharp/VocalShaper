#include "Device.h"

Device::Device() {
	/** Main Audio Device Manager */
	this->audioDeviceManager = std::make_unique<juce::AudioDeviceManager>();
}

Device::~Device() {
	this->audioDeviceManager->removeAllChangeListeners();
	if (this->callback) {
		this->audioDeviceManager->removeAudioCallback(this->callback);
	}
}

void Device::addChangeListener(juce::ChangeListener* listener) {
	this->audioDeviceManager->addChangeListener(listener);
}

juce::String Device::initialise(int numInputChannelsNeeded,
	int numOutputChannelsNeeded,
	const juce::XmlElement* savedState,
	bool selectDefaultDeviceOnFailure,
	const juce::String& preferredDefaultDeviceName,
	const juce::AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions) {
	return this->audioDeviceManager->initialise(numInputChannelsNeeded,
		numOutputChannelsNeeded, savedState, selectDefaultDeviceOnFailure,
		preferredDefaultDeviceName, preferredSetupOptions);
}

void Device::setAudioCallback(juce::AudioIODeviceCallback* newCallback) {
	if (this->callback) {
		this->audioDeviceManager->removeAudioCallback(this->callback);
	}
	this->audioDeviceManager->addAudioCallback(this->callback = newCallback);
}

void Device::updateMidiCallback(juce::MidiInputCallback* callback) {
	auto midiInputDevice = Device::getAllMIDIInputDevices();
	for (auto& i : midiInputDevice) {
		if (this->audioDeviceManager->isMidiInputDeviceEnabled(i.identifier)) {
			this->audioDeviceManager->addMidiInputDeviceCallback(
				i.identifier, callback);
		}
		else {
			this->audioDeviceManager->removeMidiInputDeviceCallback(
				i.identifier, callback);
		}
	}
}

juce::AudioDeviceManager::AudioDeviceSetup Device::getAudioDeviceSetup() const {
	return this->audioDeviceManager->getAudioDeviceSetup();
}

juce::MidiOutput* Device::getDefaultMidiOutput() const {
	return this->audioDeviceManager->getDefaultMidiOutput();
}

const juce::OwnedArray<juce::AudioIODeviceType>& Device::getAvailableDeviceTypes() {
	return this->audioDeviceManager->getAvailableDeviceTypes();
}

juce::AudioIODevice* Device::getCurrentAudioDevice() const {
	return this->audioDeviceManager->getCurrentAudioDevice();
}

juce::String Device::getCurrentAudioDeviceType() const {
	return this->audioDeviceManager->getCurrentAudioDeviceType();
}

bool Device::isMidiInputDeviceEnabled(const juce::String& deviceIdentifier) const {
	return this->audioDeviceManager->isMidiInputDeviceEnabled(deviceIdentifier);
}

juce::AudioDeviceManager* Device::getManager() const {
	return this->audioDeviceManager.get();
}

const juce::StringArray Device::getAudioDeviceList(Device::AudioDeviceType type, bool isInput) {
	std::unique_ptr<juce::AudioIODeviceType> ptrType = nullptr;
	switch (type) {
	case Device::AudioDeviceType::CoreAudio:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_CoreAudio());
		break;
	case Device::AudioDeviceType::IOSAudio:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_iOSAudio());
		break;
	case Device::AudioDeviceType::WASAPIShared:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_WASAPI(juce::WASAPIDeviceMode::shared));
		break;
	case Device::AudioDeviceType::WASAPIExclusive:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_WASAPI(juce::WASAPIDeviceMode::exclusive));
		break;
	case Device::AudioDeviceType::WASAPISharedLowLatency:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_WASAPI(juce::WASAPIDeviceMode::sharedLowLatency));
		break;
	case Device::AudioDeviceType::DirectSound:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_DirectSound());
		break;
	case Device::AudioDeviceType::ASIO:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_ASIO());
		break;
	case Device::AudioDeviceType::ALSA:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_ALSA());
		break;
	case Device::AudioDeviceType::JACK:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_JACK());
		break;
	case Device::AudioDeviceType::Android:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_Android());
		break;
	case Device::AudioDeviceType::OpenSLES:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_OpenSLES());
		break;
	case Device::AudioDeviceType::Oboe:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_Oboe());
		break;
	case Device::AudioDeviceType::Bela:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_Bela());
		break;
	default:
		break;
	}

	if (!ptrType) { return juce::StringArray{}; }
	ptrType->scanForDevices();
	return ptrType->getDeviceNames(isInput);
}

const juce::StringArray Device::getAllAudioDeviceList(bool isInput) {
	juce::StringArray result, resultTemp;

	resultTemp = Device::getAudioDeviceList(AudioDeviceType::CoreAudio, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[CoreAudio]") + s);
	}
	resultTemp = Device::getAudioDeviceList(AudioDeviceType::IOSAudio, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[iOSAudio]") + s);
	}
	resultTemp = Device::getAudioDeviceList(AudioDeviceType::WASAPIShared, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[WASAPI(shared)]") + s);
	}
	resultTemp = Device::getAudioDeviceList(AudioDeviceType::WASAPIExclusive, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[WASAPI(exclusive)]") + s);
	}
	resultTemp = Device::getAudioDeviceList(AudioDeviceType::WASAPISharedLowLatency, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[WASAPI(sharedLowLatency)]") + s);
	}
	resultTemp = Device::getAudioDeviceList(AudioDeviceType::DirectSound, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[DirectSound]") + s);
	}
	resultTemp = Device::getAudioDeviceList(AudioDeviceType::ASIO, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[ASIO]") + s);
	}
	resultTemp = Device::getAudioDeviceList(AudioDeviceType::ALSA, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[ALSA]") + s);
	}
	resultTemp = Device::getAudioDeviceList(AudioDeviceType::JACK, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[JACK]") + s);
	}
	resultTemp = Device::getAudioDeviceList(AudioDeviceType::Android, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[Android]") + s);
	}
	resultTemp = Device::getAudioDeviceList(AudioDeviceType::OpenSLES, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[OpenSLES]") + s);
	}
	resultTemp = Device::getAudioDeviceList(AudioDeviceType::Oboe, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[Oboe]") + s);
	}
	resultTemp = Device::getAudioDeviceList(AudioDeviceType::Bela, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[Bela]") + s);
	}

	return result;
}

const juce::StringArray Device::getAllAudioInputDeviceList() {
	return Device::getAllAudioDeviceList(true);
}

const juce::StringArray Device::getAllAudioOutputDeviceList() {
	return Device::getAllAudioDeviceList(false);
}

const juce::String Device::setAudioInputDevice(const juce::String& deviceName) {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	deviceSetup.inputDeviceName = deviceName;
	deviceSetup.useDefaultInputChannels = true;
	return this->audioDeviceManager->setAudioDeviceSetup(deviceSetup, true);
}

const juce::String Device::setAudioOutputDevice(const juce::String& deviceName) {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	deviceSetup.outputDeviceName = deviceName;
	deviceSetup.useDefaultOutputChannels = true;
	return this->audioDeviceManager->setAudioDeviceSetup(deviceSetup, true);
}

const juce::String Device::getAudioInputDeviceName() const {
	auto audioSetup = this->audioDeviceManager->getAudioDeviceSetup();
	return audioSetup.inputDeviceName;
}

const juce::String Device::getAudioOutputDeviceName() const {
	auto audioSetup = this->audioDeviceManager->getAudioDeviceSetup();
	return audioSetup.outputDeviceName;
}

void Device::setCurrentAudioDeviceType(const juce::String& typeName) {
	this->audioDeviceManager->setCurrentAudioDeviceType(typeName, true);
}

const juce::String Device::getCurrentAudioDeivceType() const {
	return this->audioDeviceManager->getCurrentAudioDeviceType();
}

const juce::String Device::setAudioSampleRate(double value) {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	deviceSetup.sampleRate = value;
	return this->audioDeviceManager->setAudioDeviceSetup(deviceSetup, true);
}

const juce::String Device::setAudioBufferSize(int value) {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	deviceSetup.bufferSize = value;
	return this->audioDeviceManager->setAudioDeviceSetup(deviceSetup, true);
}

double Device::getAudioSampleRate() const {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	return deviceSetup.sampleRate;
}

int Device::getAudioBufferSize() const {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	return deviceSetup.bufferSize;
}

void Device::playTestSound() const {
	this->audioDeviceManager->playTestSound();
}

double Device::getCPUUsage() const {
	return this->audioDeviceManager->getCpuUsage();
}

void Device::setMIDIInputDeviceEnabled(const juce::String& deviceID, bool enabled) {
	this->audioDeviceManager->setMidiInputDeviceEnabled(deviceID, enabled);
}

void Device::setMIDIOutputDevice(const juce::String& deviceID) {
	this->audioDeviceManager->setDefaultMidiOutputDevice(deviceID);
}

bool Device::getMIDIInputDeviceEnabled(const juce::String& deviceID) const {
	return this->audioDeviceManager->isMidiInputDeviceEnabled(deviceID);
}

const juce::String Device::getMIDIOutputDevice() const {
	return this->audioDeviceManager->getDefaultMidiOutputIdentifier();
}

const juce::Array<juce::MidiDeviceInfo> Device::getAllMIDIDevices(bool isInput) {
	return isInput
		? juce::MidiInput::getAvailableDevices()
		: juce::MidiOutput::getAvailableDevices();
}

const juce::Array<juce::MidiDeviceInfo> Device::getAllMIDIInputDevices() {
	return Device::getAllMIDIDevices(true);
}

const juce::Array<juce::MidiDeviceInfo> Device::getAllMIDIOutputDevices() {
	return Device::getAllMIDIDevices(false);
}

const juce::StringArray Device::getAllMIDIDeviceList(bool isInput) {
	auto list = Device::getAllMIDIDevices(isInput);

	juce::StringArray result;
	for (auto& i : list) {
		result.add(i.name);
	}
	return result;
}

const juce::StringArray Device::getAllMIDIInputDeviceList() {
	return Device::getAllMIDIDeviceList(true);
}

const juce::StringArray Device::getAllMIDIOutputDeviceList() {
	return Device::getAllMIDIDeviceList(false);
}

std::unique_ptr<
	juce::AudioDeviceSelectorComponent> Device::createDeviceSelector() {
	return std::make_unique<juce::AudioDeviceSelectorComponent>(
		*(Device::getInstance()->getManager()), 0, 1024, 2, 1024,
		true, true, false, false);
}

Device* Device::getInstance() {
	return Device::instance ? Device::instance : (Device::instance = new Device);
}

void Device::releaseInstance() {
	if (Device::instance) {
		delete Device::instance;
		Device::instance = nullptr;
	}
}

Device* Device::instance = nullptr;

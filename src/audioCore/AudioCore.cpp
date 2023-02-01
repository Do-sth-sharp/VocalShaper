#include "AudioCore.h"
#include "MainGraph.h"

AudioCore::AudioCore() {
	/** Main Audio Device Manager */
	this->audioDeviceManager = std::make_unique<juce::AudioDeviceManager>();

	/** Main Audio Graph Of The Audio Core */
	this->mainAudioGraph = std::make_unique<MainGraph>();

	/** Main Graph Player */
	this->mainGraphPlayer = std::make_unique<juce::AudioProcessorPlayer>();

	/** Init Audio Device */
	this->initAudioDevice();
}

const juce::StringArray AudioCore::getAudioDeviceList(AudioCore::AudioDeviceType type) {
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
	return ptrType->getDeviceNames();
}

const juce::StringArray AudioCore::getAllAudioDeviceList() {
	juce::StringArray result, resultTemp;

	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::CoreAudio);
	for (auto& s : resultTemp) {
		result.add(juce::String("[CoreAudio]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::IOSAudio);
	for (auto& s : resultTemp) {
		result.add(juce::String("[iOSAudio]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::WASAPIShared);
	for (auto& s : resultTemp) {
		result.add(juce::String("[WASAPI(shared)]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::WASAPIExclusive);
	for (auto& s : resultTemp) {
		result.add(juce::String("[WASAPI(exclusive)]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::WASAPISharedLowLatency);
	for (auto& s : resultTemp) {
		result.add(juce::String("[WASAPI(sharedLowLatency)]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::DirectSound);
	for (auto& s : resultTemp) {
		result.add(juce::String("[DirectSound]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::ASIO);
	for (auto& s : resultTemp) {
		result.add(juce::String("[ASIO]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::ALSA);
	for (auto& s : resultTemp) {
		result.add(juce::String("[ALSA]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::JACK);
	for (auto& s : resultTemp) {
		result.add(juce::String("[JACK]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::Android);
	for (auto& s : resultTemp) {
		result.add(juce::String("[Android]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::OpenSLES);
	for (auto& s : resultTemp) {
		result.add(juce::String("[OpenSLES]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::Oboe);
	for (auto& s : resultTemp) {
		result.add(juce::String("[Oboe]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::Bela);
	for (auto& s : resultTemp) {
		result.add(juce::String("[Bela]") + s);
	}

	return result;
}

void AudioCore::initAudioDevice() {
	/** Init With Default Device */
	this->audioDeviceManager->initialiseWithDefaultDevices(0, 2);

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
}

AudioCore* AudioCore::getInstance() {
	return AudioCore::instance ? AudioCore::instance : (AudioCore::instance = new AudioCore());
}

AudioCore* AudioCore::instance = nullptr;

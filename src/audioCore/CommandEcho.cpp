#include "CommandUtils.h"

AUDIOCORE_FUNC(echoDeviceAudio) {
	juce::String result;

	auto setup = getAudioDeviceManager(audioCore)->getAudioDeviceSetup();
	auto currentDevice = getAudioDeviceManager(audioCore)->getCurrentAudioDevice();
	auto currentType = getAudioDeviceManager(audioCore)->getCurrentAudioDeviceType();

	result += "========================================================================\n";
	result += "Current Audio Device Information\n";
	result += "========================================================================\n";
	result += "Sample Rate: " + juce::String(currentDevice->getCurrentSampleRate()) + "\n";
	result += "Buffer Size: " + juce::String(currentDevice->getCurrentBufferSizeSamples()) + "\n";
	result += "Bit Depth: " + juce::String(currentDevice->getCurrentBitDepth()) + "\n";
	result += "Device Type: " + currentType + "\n";
	result += "========================================================================\n";
	result += "Input Device: " + setup.inputDeviceName + "\n";
	result += "Input Latency: " + juce::String(currentDevice->getInputLatencyInSamples()) + "\n";
	result += "Input Channels:\n";
	auto inputChannels = currentDevice->getInputChannelNames();
	auto activeInputChannels = currentDevice->getActiveInputChannels();
	for (int i = 0; i < inputChannels.size(); i++) {
		result += "    [" + juce::String(i) + "] " + inputChannels[i] + " - " + (activeInputChannels[i] ? "ON" : "OFF") + "\n";
	}
	result += "========================================================================\n";
	result += "Output Device: " + setup.outputDeviceName + "\n";
	result += "Output Latency: " + juce::String(currentDevice->getOutputLatencyInSamples()) + "\n";
	result += "Output Channels:\n";
	auto outputChannels = currentDevice->getOutputChannelNames();
	auto activeOutputChannels = currentDevice->getActiveOutputChannels();
	for (int i = 0; i < outputChannels.size(); i++) {
		result += "    [" + juce::String(i) + "] " + outputChannels[i] + " - " + (activeOutputChannels[i] ? "ON" : "OFF") + "\n";
	}
	result += "========================================================================\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoDeviceMIDI) {
	juce::String result;

	auto midiInputList = juce::MidiInput::getAvailableDevices();
	auto midiOutputDevice = getAudioDeviceManager(audioCore)->getDefaultMidiOutput();

	result += "========================================================================\n";
	result += "Current MIDI Device Information\n";
	result += "========================================================================\n";
	result += "MIDI Input Devices:\n";
	for (int i = 0; i < midiInputList.size(); i++) {
		auto& device = midiInputList.getReference(i);
		result += "    [" + juce::String(i) + "] " + device.name + " - " + (getAudioDeviceManager(audioCore)->isMidiInputDeviceEnabled(device.identifier) ? "ON" : "OFF") + "\n";
		result += "        " + device.identifier + "\n";
	}
	result += "========================================================================\n";
	result += "MIDI Output Device: " + (midiOutputDevice ? midiOutputDevice->getName() : "") + "\n";
	result += "MIDI Output Device ID: " + (midiOutputDevice ? midiOutputDevice->getIdentifier() : "") + "\n";
	result += "========================================================================\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoMixerInfo) {
	juce::String result;

	auto mixer = audioCore->getMixer();
	if (mixer) {
		result += "========================================================================\n";
		result += "Mixer\n";
		result += "========================================================================\n";

		int icn = mixer->getTotalNumInputChannels();
		int ocn = mixer->getTotalNumOutputChannels();
		int ibsn = mixer->getSequencerBusNum();
		int icsn = mixer->getSequencerChannelNum();
		int icdn = icn - icsn;

		result += "Total Input Channel: " + juce::String(icn) + "\n";
		result += "Total Output Channel: " + juce::String(ocn) + "\n";
		result += "Sequencer Input Bus: " + juce::String(ibsn) + "\n";
		result += "Sequencer Input Channel: " + juce::String(icsn) + "\n";
		result += "Device Input Channel: " + juce::String(icdn) + "\n";
		result += "Total Track: " + juce::String(mixer->getTrackNum()) + "\n";

		result += "========================================================================\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoMixerTrack) {
	juce::String result;

	auto mixer = audioCore->getMixer();
	if (mixer) {
		result += "========================================================================\n";
		result += "Mixer Track List\n";
		result += "========================================================================\n";

		int trackNum = mixer->getTrackNum();
		for (int i = 0; i < trackNum; i++) {
			if (i > 0) {
				result += "------------------------------------------------------------------------\n";
			}

			auto track = mixer->getTrackProcessor(i);
			result += "[" + juce::String(i) + "]\n";
			result += "\tBus Type: " + track->getAudioChannelSet().getDescription() + "\n";
			result += "\tInput Bus: " + juce::String(track->getBusCount(true)) + "\n";
			result += "\tOutput Bus: " + juce::String(track->getBusCount(false)) + "\n";
			result += "\tInput Channel: " + juce::String(track->getTotalNumInputChannels()) + "\n";
			result += "\tOutput Channel: " + juce::String(track->getTotalNumOutputChannels()) + "\n";
			result += "\tMute: " + juce::String(track->getMute() ? "True" : "False") + "\n";
		}

		result += "========================================================================\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoMixerTrackInfo) {
	juce::String result;

	auto mixer = audioCore->getMixer();
	if (mixer) {
		int trackNum = mixer->getTrackNum();
		int trackId = luaL_checkinteger(L, 1);
		if (trackId < 0 || trackId >= trackNum) {
			return CommandFuncResult{ false, "Invalid track ID: " + juce::String(trackId) };
		}

		result += "========================================================================\n";
		result += "Mixer Track " + juce::String(trackId) + "\n";
		result += "========================================================================\n";

		auto track = mixer->getTrackProcessor(trackId);
		if (track) {
			result += "Bus Type: " + track->getAudioChannelSet().getDescription() + "\n";
			result += "\tInput Bus: " + juce::String(track->getBusCount(true)) + "\n";
			result += "\tOutput Bus: " + juce::String(track->getBusCount(false)) + "\n";
			result += "\tInput Channel: " + juce::String(track->getTotalNumInputChannels()) + "\n";
			result += "\tOutput Channel: " + juce::String(track->getTotalNumOutputChannels()) + "\n";
			result += "Mute: " + juce::String(track->getMute() ? "True" : "False") + "\n";
			result += "Gain: " + juce::String(track->getGain()) + "\n";
			result += "Pan: " + juce::String(track->getPan()) + "\n";
			result += "Slider: " + juce::String(track->getSlider()) + "\n";

			result += "------------------------------------------------------------------------\n";

			result += "Input From Track:\n";
			auto inputFromTrackConnectiones = mixer->getTrackInputFromTrackConnections(trackId);
			for (int i = 0; i < inputFromTrackConnectiones.size(); i++) {
				auto connection = inputFromTrackConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<0>(connection)) + ", " + juce::String(std::get<1>(connection))
					+ " - "
					+ juce::String(std::get<2>(connection)) + ", " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "Input From Sequencer:\n";
			auto inputFromSequencerConnectiones = mixer->getTrackInputFromSequencerConnections(trackId);
			for (int i = 0; i < inputFromSequencerConnectiones.size(); i++) {
				auto connection = inputFromSequencerConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<1>(connection)) + " - " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "Input From Device:\n";
			auto inputFromDeviceConnectiones = mixer->getTrackInputFromDeviceConnections(trackId);
			for (int i = 0; i < inputFromDeviceConnectiones.size(); i++) {
				auto connection = inputFromDeviceConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<1>(connection)) + " - " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "------------------------------------------------------------------------\n";

			result += "Output To Track:\n";
			auto outputToTrackConnectiones = mixer->getTrackOutputToTrackConnections(trackId);
			for (int i = 0; i < outputToTrackConnectiones.size(); i++) {
				auto connection = outputToTrackConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<0>(connection)) + ", " + juce::String(std::get<1>(connection))
					+ " - "
					+ juce::String(std::get<2>(connection)) + ", " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "Output To Device:\n";
			auto outputToDeviceConnectiones = mixer->getTrackOutputToDeviceConnections(trackId);
			for (int i = 0; i < outputToDeviceConnectiones.size(); i++) {
				auto connection = outputToDeviceConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<1>(connection)) + " - " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "------------------------------------------------------------------------\n";

			result += "Plugins:\n";
			auto pluginDock = track->getPluginDock();
			if (pluginDock) {
				auto pluginList = pluginDock->getPluginList();
				for (int i = 0; i < pluginList.size(); i++) {
					auto pluginState = pluginList.getUnchecked(i);
					result += "\t[" + juce::String(i) + "] "
						+ std::get<0>(pluginState) + " - "
						+ juce::String(std::get<1>(pluginState) ? "ON" : "OFF") + "\n";
				}
			}
		}

		result += "========================================================================\n";
	}

	return CommandFuncResult{ true, result };
}

void regCommandEcho(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoDeviceAudio);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoDeviceMIDI);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerInfo);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerTrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerTrackInfo);
}

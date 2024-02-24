#include "ActionEcho.h"

#include "../AudioCore.h"
#include "../misc/Device.h"
#include "../Utils.h"

ActionEchoDeviceAudio::ActionEchoDeviceAudio() {}

bool ActionEchoDeviceAudio::doAction() {
	juce::String result;

	auto setup = Device::getInstance()->getAudioDeviceSetup();
	auto currentDevice = Device::getInstance()->getCurrentAudioDevice();
	auto currentType = Device::getInstance()->getCurrentAudioDeviceType();

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

	this->output(result);
	return true;
}

ActionEchoDeviceMidi::ActionEchoDeviceMidi() {}

bool ActionEchoDeviceMidi::doAction() {
	juce::String result;

	auto midiInputList = Device::getAllMIDIInputDevices();
	auto midiOutputDevice = Device::getInstance()->getDefaultMidiOutput();

	result += "========================================================================\n";
	result += "Current MIDI Device Information\n";
	result += "========================================================================\n";
	result += "MIDI Input Devices:\n";
	for (int i = 0; i < midiInputList.size(); i++) {
		auto& device = midiInputList.getReference(i);
		result += "    [" + juce::String(i) + "] " + device.name + " - " + (Device::getInstance()->isMidiInputDeviceEnabled(device.identifier) ? "ON" : "OFF") + "\n";
		result += "        " + device.identifier + "\n";
	}
	result += "========================================================================\n";
	result += "MIDI Output Device: " + (midiOutputDevice ? midiOutputDevice->getName() : "") + "\n";
	result += "MIDI Output Device ID: " + (midiOutputDevice ? midiOutputDevice->getIdentifier() : "") + "\n";
	result += "========================================================================\n";

	this->output(result);
	return true;
}

ActionEchoMixerInfo::ActionEchoMixerInfo() {}

bool ActionEchoMixerInfo::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		juce::String result;

		result += "========================================================================\n";
		result += "Mixer\n";
		result += "========================================================================\n";

		int icn = graph->getTotalNumInputChannels();
		int ocn = graph->getTotalNumOutputChannels();

		result += "Total Input Channel: " + juce::String(icn) + "\n";
		result += "Total Output Channel: " + juce::String(ocn) + "\n";
		result += "Total Track: " + juce::String(graph->getTrackNum()) + "\n";

		result += "========================================================================\n";

		this->output(result);
		return true;
	}

	return false;
}

ActionEchoMixerTrack::ActionEchoMixerTrack() {}

bool ActionEchoMixerTrack::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		juce::String result;

		result += "========================================================================\n";
		result += "Mixer Track List\n";
		result += "========================================================================\n";

		int trackNum = graph->getTrackNum();
		for (int i = 0; i < trackNum; i++) {
			if (i > 0) {
				result += "------------------------------------------------------------------------\n";
			}

			auto track = graph->getTrackProcessor(i);
			result += "[" + juce::String(i) + "]\n";
			result += "\tBus Type: " + track->getAudioChannelSet().getDescription() + "\n";
			result += "\tInput Bus: " + juce::String(track->getBusCount(true)) + "\n";
			result += "\tOutput Bus: " + juce::String(track->getBusCount(false)) + "\n";
			result += "\tInput Channel: " + juce::String(track->getTotalNumInputChannels()) + "\n";
			result += "\tOutput Channel: " + juce::String(track->getTotalNumOutputChannels()) + "\n";
			result += "\tMute: " + juce::String(track->getMute() ? "True" : "False") + "\n";
		}

		result += "========================================================================\n";

		this->output(result);
		return true;
	}

	return false;
}

ActionEchoMixerTrackInfo::ActionEchoMixerTrackInfo(int index)
	: index(index) {}

bool ActionEchoMixerTrackInfo::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		juce::String result;

		int trackNum = graph->getTrackNum();
		int trackId = this->index;
		if (trackId < 0 || trackId >= trackNum) {
			this->output("Invalid track ID: " + juce::String(trackId));
			return false;
		}

		result += "========================================================================\n";
		result += "Mixer Track " + juce::String(trackId) + "\n";
		result += "========================================================================\n";

		if (auto track = graph->getTrackProcessor(trackId)) {
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
			auto inputFromTrackConnectiones = graph->getTrackInputFromTrackConnections(trackId);
			for (int i = 0; i < inputFromTrackConnectiones.size(); i++) {
				auto connection = inputFromTrackConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<0>(connection)) + ", " + juce::String(std::get<1>(connection))
					+ " - "
					+ juce::String(std::get<2>(connection)) + ", " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "Input From Source:\n";
			auto inputFromSequencerConnectiones = graph->getTrackInputFromSrcConnections(trackId);
			for (int i = 0; i < inputFromSequencerConnectiones.size(); i++) {
				auto connection = inputFromSequencerConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<0>(connection)) + ", " + juce::String(std::get<1>(connection))
					+ " - "
					+ juce::String(std::get<2>(connection)) + ", " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "Input From Device:\n";
			auto inputFromDeviceConnectiones = graph->getTrackInputFromDeviceConnections(trackId);
			for (int i = 0; i < inputFromDeviceConnectiones.size(); i++) {
				auto connection = inputFromDeviceConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<0>(connection)) + ", " + juce::String(std::get<1>(connection))
					+ " - "
					+ juce::String(std::get<2>(connection)) + ", " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "------------------------------------------------------------------------\n";

			result += "Output To Track:\n";
			auto outputToTrackConnectiones = graph->getTrackOutputToTrackConnections(trackId);
			for (int i = 0; i < outputToTrackConnectiones.size(); i++) {
				auto connection = outputToTrackConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<0>(connection)) + ", " + juce::String(std::get<1>(connection))
					+ " - "
					+ juce::String(std::get<2>(connection)) + ", " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "Output To Device:\n";
			auto outputToDeviceConnectiones = graph->getTrackOutputToDeviceConnections(trackId);
			for (int i = 0; i < outputToDeviceConnectiones.size(); i++) {
				auto connection = outputToDeviceConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<0>(connection)) + ", " + juce::String(std::get<1>(connection))
					+ " - "
					+ juce::String(std::get<2>(connection)) + ", " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "------------------------------------------------------------------------\n";

			result += "Plugins:\n";
			if (auto pluginDock = track->getPluginDock()) {
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

		this->output(result);
		return true;
	}

	return false;
}

ActionEchoMixerTrackGain::ActionEchoMixerTrackGain(int index)
	: index(index) {}

bool ActionEchoMixerTrackGain::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->index)) {
			this->output("Mixer Track Gain Value: <" + juce::String(this->index) + "> " + juce::String(track->getGain()) + "\n");
			return true;
		}
	}
	return false;
}

ActionEchoMixerTrackPan::ActionEchoMixerTrackPan(int index)
	: index(index) {}

bool ActionEchoMixerTrackPan::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->index)) {
			this->output("Mixer Track Panner Value: <" + juce::String(this->index) + "> " + juce::String(track->getPan()) + "\n");
			return true;
		}
	}
	return false;
}

ActionEchoMixerTrackSlider::ActionEchoMixerTrackSlider(int index)
	: index(index) {}

bool ActionEchoMixerTrackSlider::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->index)) {
			this->output("Mixer Track Slider Value: <" + juce::String(this->index) + "> " + juce::String(track->getSlider()) + "\n");
			return true;
		}
	}
	return false;
}

ActionEchoInstrParamValue::ActionEchoInstrParamValue(
	int instr, int param)
	: instr(instr), param(param) {}

bool ActionEchoInstrParamValue::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(this->instr)) {
			if (auto instr = track->getInstrProcessor()) {
				this->output("Instr Param Value: [" + juce::String(this->param) + "] " + instr->getParamName(this->param) + " - " + juce::String(instr->getParamValue(this->param)) + "\n");
				return true;
			}
		}
	}
	return false;
}

ActionEchoInstrParamDefaultValue::ActionEchoInstrParamDefaultValue(
	int instr, int param)
	: instr(instr), param(param) {}

bool ActionEchoInstrParamDefaultValue::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(this->instr)) {
			if (auto instr = track->getInstrProcessor()) {
				this->output("Instr Param Default Value: [" + juce::String(this->param) + "] " + instr->getParamName(this->param) + " - " + juce::String(instr->getParamDefaultValue(this->param)) + "\n");
				return true;
			}
		}
	}
	return false;
}

ActionEchoEffectParamValue::ActionEchoEffectParamValue(
	int track, int effect, int param)
	: track(track), effect(effect), param(param) {}

bool ActionEchoEffectParamValue::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					this->output("Effect Param Value: [" + juce::String(this->param) + "] " + effect->getParamName(this->param) + " - " + juce::String(effect->getParamValue(this->param)) + "\n");
					return true;
				}
			}
		}
	}
	return false;
}

ActionEchoEffectParamDefaultValue::ActionEchoEffectParamDefaultValue(
	int track, int effect, int param)
	: track(track), effect(effect), param(param) {}

bool ActionEchoEffectParamDefaultValue::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					this->output("Effect Param Default Value: [" + juce::String(this->param) + "] " + effect->getParamName(this->param) + " - " + juce::String(effect->getParamDefaultValue(this->param)) + "\n");
					return true;
				}
			}
		}
	}
	return false;
}

ActionEchoInstrParamCC::ActionEchoInstrParamCC(
	int instr, int param)
	: instr(instr), param(param) {}

bool ActionEchoInstrParamCC::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(this->instr)) {
			if (auto instr = track->getInstrProcessor()) {
				this->output("Instr Param MIDI CC: [" + juce::String(this->param) + "] " + instr->getParamName(this->param) + " - MIDI CC " + juce::String(instr->getParamCCConnection(this->param)) + "\n");
				return true;
			}
		}
	}
	return false;
}

ActionEchoEffectParamCC::ActionEchoEffectParamCC(
	int track, int effect, int param)
	: track(track), effect(effect), param(param) {}

bool ActionEchoEffectParamCC::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					this->output("Effect Param MIDI CC: [" + juce::String(this->param) + "] " + effect->getParamName(this->param) + " - MIDI CC " + juce::String(effect->getParamCCConnection(this->param)) + "\n");
					return true;
				}
			}
		}
	}
	return false;
}

ActionEchoInstrCCParam::ActionEchoInstrCCParam(
	int instr, int cc)
	: instr(instr), cc(cc) {}

bool ActionEchoInstrCCParam::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(this->instr)) {
			if (auto instr = track->getInstrProcessor()) {
				juce::String result;

				int paramIndex = instr->getCCParamConnection(this->cc);
				if (paramIndex > -1) {
					result += "Instr Param MIDI CC: MIDI CC " + juce::String(this->cc) + " - [" + juce::String(paramIndex) + "] " + instr->getParamName(paramIndex) + "\n";
				}
				else {
					result += "Instr Param MIDI CC: MIDI CC " + juce::String(this->cc) + " - Disabled\n";
				}

				this->output(result);
				return true;
			}
		}
	}
	return false;
}

ActionEchoEffectCCParam::ActionEchoEffectCCParam(
	int track, int effect, int cc)
	: track(track), effect(effect), cc(cc) {}

bool ActionEchoEffectCCParam::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					juce::String result;

					int paramIndex = effect->getCCParamConnection(this->cc);
					if (paramIndex > -1) {
						result += "Effect Param MIDI CC: MIDI CC " + juce::String(this->cc) + " - [" + juce::String(paramIndex) + "] " + effect->getParamName(paramIndex) + "\n";
					}
					else {
						result += "Effect Param MIDI CC: MIDI CC " + juce::String(this->cc) + " - Disabled\n";
					}

					this->output(result);
					return true;
				}
			}
		}
	}
	return false;
}

ActionEchoSourceNum::ActionEchoSourceNum() {}

bool ActionEchoSourceNum::doAction() {
	if (auto manager = CloneableSourceManager::getInstance()) {
		this->output("Total Source Num: " + juce::String(manager->getSourceNum()) + "\n");
		return true;
	}
	return false;
}

ActionEchoSource::ActionEchoSource(int index)
	: index(index) {}

bool ActionEchoSource::doAction() {
	if (auto manager = CloneableSourceManager::getInstance()) {
		if (auto source = manager->getSource(this->index)) {
			juce::String result;

			result += "========================================================================\n";
			result += "Source " + juce::String(this->index) + "\n";
			result += "========================================================================\n";
			result += "ID: " + juce::String(source->getId()) + "\n";
			result += "Name: " + source->getName() + "\n";
			result += "Length: " + juce::String(source->getSourceLength()) + "\n";
			result += "SampleRate: " + juce::String(source->getSampleRate()) + "\n";
			result += "Source SampleRate: " + juce::String(source->getSourceSampleRate()) + "\n";
			result += "Channel Num: " + juce::String(source->getChannelNum()) + "\n";
			result += "Track Num: " + juce::String(source->getTrackNum()) + "\n";
			result += "Synthesizer: " + source->getSynthesizerName() + "\n";
			if (auto ptr = dynamic_cast<CloneableAudioSource*>(source.getSource())) {
				result += "Type: Audio\n";
				result += "Channel Num: " + juce::String(ptr->getChannelNum()) + "\n";
			}
			else if (auto ptr = dynamic_cast<CloneableMIDISource*>(source.getSource())) {
				result += "Type: MIDI\n";
				result += "Track Num: " + juce::String(ptr->getTrackNum()) + "\n";
			}
			else {
				result += "Type: Unknown\n";
			}
			result += "========================================================================\n";

			this->output(result);
			return true;
		}
	}

	return false;
}

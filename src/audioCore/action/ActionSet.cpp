#include "ActionSet.h"

#include "../AudioCore.h"
#include "../misc/Device.h"

ActionSetDeviceAudioType::ActionSetDeviceAudioType(const juce::String& type)
	: type(type) {}

bool ActionSetDeviceAudioType::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	Device::getInstance()->setCurrentAudioDeviceType(this->type);

	juce::String result;
	result += "Current Audio Device Type: " + Device::getInstance()->getCurrentAudioDeviceType() + "\n";
	result += "Current Audio Input Device: " + Device::getInstance()->getAudioInputDeviceName() + "\n";
	result += "Current Audio Output Device: " + Device::getInstance()->getAudioOutputDeviceName() + "\n";
	this->output(result);
	return true;
}

ActionSetDeviceAudioInput::ActionSetDeviceAudioInput(const juce::String& name)
	: name(name) {}

bool ActionSetDeviceAudioInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	juce::String result;
	auto err = Device::getInstance()->setAudioInputDevice(this->name);
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Device Type: " + Device::getInstance()->getCurrentAudioDeviceType() + "\n";
	result += "Current Audio Input Device: " + Device::getInstance()->getAudioInputDeviceName() + "\n";
	result += "Current Audio Output Device: " + Device::getInstance()->getAudioOutputDeviceName() + "\n";
	this->output(result);
	return true;
}

ActionSetDeviceAudioOutput::ActionSetDeviceAudioOutput(const juce::String& name)
	: name(name) {}

bool ActionSetDeviceAudioOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	juce::String result;
	auto err = Device::getInstance()->setAudioOutputDevice(this->name);
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Device Type: " + Device::getInstance()->getCurrentAudioDeviceType() + "\n";
	result += "Current Audio Input Device: " + Device::getInstance()->getAudioInputDeviceName() + "\n";
	result += "Current Audio Output Device: " + Device::getInstance()->getAudioOutputDeviceName() + "\n";
	this->output(result);
	return true;
}

ActionSetDeviceAudioSampleRate::ActionSetDeviceAudioSampleRate(double sampleRate)
	: sampleRate(sampleRate) {}

bool ActionSetDeviceAudioSampleRate::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	juce::String result;
	auto err = Device::getInstance()->setAudioSampleRate(this->sampleRate);
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Sample Rate: " + juce::String(Device::getInstance()->getAudioSampleRate()) + "\n";
	this->output(result);
	return true;
}

ActionSetDeviceAudioBufferSize::ActionSetDeviceAudioBufferSize(int bufferSize)
	: bufferSize(bufferSize) {}

bool ActionSetDeviceAudioBufferSize::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	juce::String result;
	auto err = Device::getInstance()->setAudioBufferSize(this->bufferSize);
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Buffer Size: " + juce::String(Device::getInstance()->getAudioBufferSize()) + "\n";
	this->output(result);
	return true;
}

ActionSetDeviceMidiInput::ActionSetDeviceMidiInput(
	const juce::String& name, bool enabled)
	: name(name), enabled(enabled) {}

bool ActionSetDeviceMidiInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	juce::String result;

	Device::getInstance()->setMIDIInputDeviceEnabled(
		this->name,	this->enabled);

	result += "MIDI Input Device: " + this->name + " - " + (Device::getInstance()->getMIDIInputDeviceEnabled(this->name) ? "ON" : "OFF") + "\n";
	this->output(result);
	return true;
}

ActionSetDeviceMidiOutput::ActionSetDeviceMidiOutput(const juce::String& name)
	: name(name) {}

bool ActionSetDeviceMidiOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	juce::String result;

	Device::getInstance()->setMIDIOutputDevice(this->name);
	result += "MIDI Output Device: " + this->name + "\n";
	this->output(result);
	return true;
}

ActionSetMidiDebuggerMaxNum::ActionSetMidiDebuggerMaxNum(int num)
	: num(num) {}

bool ActionSetMidiDebuggerMaxNum::doAction() {
	juce::String result;

	AudioCore::getInstance()->setMIDIDebuggerMaxNum(this->num);
	result += "MIDI Debugger Max Num: " + juce::String(AudioCore::getInstance()->getMIDIDebuggerMaxNum()) + "\n";
	this->output(result);
	return true;
}

ActionSetMixerTrackGain::ActionSetMixerTrackGain(
	int track, float value)
	: ACTION_DB{ track, value } {}

bool ActionSetMixerTrackGain::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetMixerTrackGain);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			ACTION_DATA(oldValue) = track->getGain();

			track->setGain(ACTION_DATA(value));
			ACTION_RESULT(true);
		}
	}

	ACTION_RESULT(false);
}

bool ActionSetMixerTrackGain::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetMixerTrackGain);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			track->setGain(ACTION_DATA(oldValue));

			juce::String result;
			result += "Undo Set Mixer Track Gain Value: <" + juce::String(ACTION_DATA(track)) + "> " + juce::String(track->getGain()) + "\n";
			this->output(result);
			ACTION_RESULT(true);
		}
	}

	ACTION_RESULT(false);
}

juce::UndoableAction* ActionSetMixerTrackGain::createCoalescedAction(
	juce::UndoableAction* nextAction) {
	if (auto action = dynamic_cast<ActionSetMixerTrackGain*>(nextAction)) {
		if (this->_data.track == action->_data.track) {
			auto newAction = std::make_unique<ActionSetMixerTrackGain>(
				action->_data.track, action->_data.value);
			newAction->_data.oldValue = this->_data.oldValue;
			return newAction.release();
		}
	}
	return nullptr;
}

ActionSetMixerTrackPan::ActionSetMixerTrackPan(
	int track, float value)
	: ACTION_DB{ track, value } {}

bool ActionSetMixerTrackPan::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetMixerTrackPan);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			ACTION_DATA(oldValue) = track->getPan();

			track->setPan(ACTION_DATA(value));
			ACTION_RESULT(true);
		}
	}

	ACTION_RESULT(false);
}

bool ActionSetMixerTrackPan::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetMixerTrackPan);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			track->setPan(ACTION_DATA(oldValue));

			juce::String result;
			result += "Undo Set Mixer Track Pan Value: <" + juce::String(ACTION_DATA(track)) + "> " + juce::String(track->getPan()) + "\n";
			this->output(result);
			ACTION_RESULT(true);
		}
	}

	ACTION_RESULT(false);
}

juce::UndoableAction* ActionSetMixerTrackPan::createCoalescedAction(
	juce::UndoableAction* nextAction) {
	if (auto action = dynamic_cast<ActionSetMixerTrackPan*>(nextAction)) {
		if (this->_data.track == action->_data.track) {
			auto newAction = std::make_unique<ActionSetMixerTrackPan>(
				action->_data.track, action->_data.value);
			newAction->_data.oldValue = this->_data.oldValue;
			return newAction.release();
		}
	}
	return nullptr;
}

ActionSetMixerTrackSlider::ActionSetMixerTrackSlider(
	int track, float value)
	: ACTION_DB{ track, value } {}

bool ActionSetMixerTrackSlider::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetMixerTrackSlider);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			ACTION_DATA(oldValue) = track->getSlider();

			track->setSlider(ACTION_DATA(value));
			ACTION_RESULT(true);
		}
	}

	ACTION_RESULT(false);
}

bool ActionSetMixerTrackSlider::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetMixerTrackSlider);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			track->setSlider(ACTION_DATA(oldValue));

			juce::String result;
			result += "Undo Set Mixer Track Slider Value: <" + juce::String(ACTION_DATA(track)) + "> " + juce::String(track->getSlider()) + "\n";
			this->output(result);
			ACTION_RESULT(true);
		}
	}

	ACTION_RESULT(false);
}

juce::UndoableAction* ActionSetMixerTrackSlider::createCoalescedAction(
	juce::UndoableAction* nextAction) {
	if (auto action = dynamic_cast<ActionSetMixerTrackSlider*>(nextAction)) {
		if (this->_data.track == action->_data.track) {
			auto newAction = std::make_unique<ActionSetMixerTrackSlider>(
				action->_data.track, action->_data.value);
			newAction->_data.oldValue = this->_data.oldValue;
			return newAction.release();
		}
	}
	return nullptr;
}

ActionSetEffectBypass::ActionSetEffectBypass(
	int track, int effect, bool bypass)
	: ACTION_DB{ track, effect, bypass } {}

bool ActionSetEffectBypass::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectBypass);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				ACTION_DATA(oldBypass) = pluginDock->getPluginBypass(ACTION_DATA(effect));

				pluginDock->setPluginBypass(ACTION_DATA(effect), ACTION_DATA(bypass));

				this->output("Plugin Bypass: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + juce::String(ACTION_DATA(bypass) ? "ON" : "OFF") + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectBypass::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectBypass);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->setPluginBypass(ACTION_DATA(effect), ACTION_DATA(oldBypass));

				this->output("Undo Plugin Bypass: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + juce::String(ACTION_DATA(bypass) ? "ON" : "OFF") + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetEffectBypassByPtr::ActionSetEffectBypassByPtr(
	quickAPI::PluginHolder effect, bool bypass)
	: ACTION_DB{ effect, bypass } {}

bool ActionSetEffectBypassByPtr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectBypassByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(effect)) {
		ACTION_DATA(oldBypass) = PluginDock::getPluginBypass(ACTION_DATA(effect));

		PluginDock::setPluginBypass(ACTION_DATA(effect), ACTION_DATA(bypass));

		this->output("Plugin Bypass: [" + ACTION_DATA(effect)->getName() + "] " + juce::String(ACTION_DATA(bypass) ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectBypassByPtr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectBypassByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(effect)) {
		PluginDock::setPluginBypass(ACTION_DATA(effect), ACTION_DATA(oldBypass));

		this->output("Undo Plugin Bypass: [" + ACTION_DATA(effect)->getName() + "] " + juce::String(ACTION_DATA(bypass) ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionSetInstrBypass::ActionSetInstrBypass(
	int instr, bool bypass)
	: ACTION_DB{ instr, bypass } {}

bool ActionSetInstrBypass::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrBypass);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(instr))) {
			ACTION_DATA(oldBypass) = track->getInstrumentBypass();

			track->setInstrumentBypass(ACTION_DATA(bypass));

			this->output("Plugin Bypass: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(ACTION_DATA(bypass) ? "ON" : "OFF") + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrBypass::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrBypass);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(instr))) {
			track->setInstrumentBypass(ACTION_DATA(oldBypass));

			this->output("Undo Plugin Bypass: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(ACTION_DATA(bypass) ? "ON" : "OFF") + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

ActionSetInstrBypassByPtr::ActionSetInstrBypassByPtr(
	quickAPI::PluginHolder instr, bool bypass)
	: ACTION_DB{ instr, bypass } {}

bool ActionSetInstrBypassByPtr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrBypassByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(instr)) {
		ACTION_DATA(oldBypass) = SeqSourceProcessor::getInstrumentBypass(ACTION_DATA(instr));

		SeqSourceProcessor::setInstrumentBypass(ACTION_DATA(instr), ACTION_DATA(bypass));

		this->output("Plugin Bypass: [" + ACTION_DATA(instr)->getName() + "] " + juce::String(ACTION_DATA(bypass) ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrBypassByPtr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrBypassByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(instr)) {
		SeqSourceProcessor::setInstrumentBypass(ACTION_DATA(instr), ACTION_DATA(oldBypass));

		this->output("Undo Plugin Bypass: [" + ACTION_DATA(instr)->getName() + "] " + juce::String(ACTION_DATA(bypass) ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionSetInstrMidiChannel::ActionSetInstrMidiChannel(
	int instr, int channel)
	: ACTION_DB{ instr, channel } {}

bool ActionSetInstrMidiChannel::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrMidiChannel);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(instr))) {
			if (auto instr = track->getInstrProcessor()) {
				ACTION_DATA(oldChannel) = instr->getMIDIChannel();

				instr->setMIDIChannel(ACTION_DATA(channel));

				this->output("Plugin MIDI Channel: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(instr->getMIDIChannel()) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrMidiChannel::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrMidiChannel);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(instr))) {
			if (auto instr = track->getInstrProcessor()) {
				instr->setMIDIChannel(ACTION_DATA(oldChannel));

				this->output("Undo Plugin MIDI Channel: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(instr->getMIDIChannel()) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetInstrMidiChannelByPtr::ActionSetInstrMidiChannelByPtr(
	quickAPI::PluginHolder instr, int channel)
	: ACTION_DB{ instr, channel } {}

bool ActionSetInstrMidiChannelByPtr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrMidiChannelByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(instr)) {
		ACTION_DATA(oldChannel) = ACTION_DATA(instr)->getMIDIChannel();

		ACTION_DATA(instr)->setMIDIChannel(ACTION_DATA(channel));

		this->output("Plugin MIDI Channel: [" + ACTION_DATA(instr)->getName() + "] " + juce::String(ACTION_DATA(instr)->getMIDIChannel()) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrMidiChannelByPtr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrMidiChannelByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(instr)) {
		ACTION_DATA(instr)->setMIDIChannel(ACTION_DATA(oldChannel));

		this->output("Undo Plugin MIDI Channel: [" + ACTION_DATA(instr)->getName() + "] " + juce::String(ACTION_DATA(instr)->getMIDIChannel()) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionSetEffectMidiChannel::ActionSetEffectMidiChannel(
	int track, int effect, int channel)
	: ACTION_DB{ track, effect, channel } {}

bool ActionSetEffectMidiChannel::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectMidiChannel);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
					ACTION_DATA(oldChannel) = effect->getMIDIChannel();

					effect->setMIDIChannel(ACTION_DATA(channel));

					this->output("Plugin MIDI Channel: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + juce::String(effect->getMIDIChannel()) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectMidiChannel::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectMidiChannel);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
					effect->setMIDIChannel(ACTION_DATA(oldChannel));

					this->output("Undo Plugin MIDI Channel: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + juce::String(effect->getMIDIChannel()) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetEffectMidiChannelByPtr::ActionSetEffectMidiChannelByPtr(
	quickAPI::PluginHolder effect, int channel)
	: ACTION_DB{ effect, channel } {}

bool ActionSetEffectMidiChannelByPtr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectMidiChannelByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(effect)) {
		ACTION_DATA(oldChannel) = ACTION_DATA(effect)->getMIDIChannel();

		ACTION_DATA(effect)->setMIDIChannel(ACTION_DATA(channel));

		this->output("Plugin MIDI Channel: [" + ACTION_DATA(effect)->getName() + "] " + juce::String(ACTION_DATA(effect)->getMIDIChannel()) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectMidiChannelByPtr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectMidiChannelByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(effect)) {
		ACTION_DATA(effect)->setMIDIChannel(ACTION_DATA(oldChannel));

		this->output("Undo Plugin MIDI Channel: [" + ACTION_DATA(effect)->getName() + "] " + juce::String(ACTION_DATA(effect)->getMIDIChannel()) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionSetInstrParamValue::ActionSetInstrParamValue(
	int instr, int param, float value)
	: ACTION_DB{ instr, param, value } {}

bool ActionSetInstrParamValue::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrParamValue);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(instr))) {
			if (auto instr = track->getInstrProcessor()) {
				ACTION_DATA(oldValue) = instr->getParamValue(ACTION_DATA(param));

				instr->setParamValue(ACTION_DATA(param), ACTION_DATA(value));

				this->output("Set Instr Param Value: [" + juce::String(ACTION_DATA(param)) + "] " + instr->getParamName(ACTION_DATA(param)) + " - " + juce::String(instr->getParamValue(ACTION_DATA(param))) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrParamValue::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrParamValue);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(instr))) {
			if (auto instr = track->getInstrProcessor()) {
				instr->setParamValue(ACTION_DATA(param), ACTION_DATA(oldValue));

				this->output("Undo Set Instr Param Value: [" + juce::String(ACTION_DATA(param)) + "] " + instr->getParamName(ACTION_DATA(param)) + " - " + juce::String(instr->getParamValue(ACTION_DATA(param))) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetEffectParamValue::ActionSetEffectParamValue(
	int track, int effect, int param, float value)
	: ACTION_DB{ track, effect, param, value } {}

bool ActionSetEffectParamValue::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectParamValue);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
					ACTION_DATA(oldValue) = effect->getParamValue(ACTION_DATA(param));

					effect->setParamValue(ACTION_DATA(param), ACTION_DATA(value));

					this->output("Effect Param Value: [" + juce::String(ACTION_DATA(param)) + "] " + effect->getParamName(ACTION_DATA(param)) + " - " + juce::String(effect->getParamValue(ACTION_DATA(param))) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectParamValue::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectParamValue);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
					effect->setParamValue(ACTION_DATA(param), ACTION_DATA(oldValue));

					this->output("Undo Effect Param Value: [" + juce::String(ACTION_DATA(param)) + "] " + effect->getParamName(ACTION_DATA(param)) + " - " + juce::String(effect->getParamValue(ACTION_DATA(param))) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetEffectIndex::ActionSetEffectIndex(
	int track, int oldIndex, int newIndex)
	: ACTION_DB{ track, oldIndex, newIndex } {}

bool ActionSetEffectIndex::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectIndex);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->setPluginIndex(ACTION_DATA(oldIndex), ACTION_DATA(newIndex));

				this->output("Set Effect Index: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(oldIndex)) + "] " + juce::String(ACTION_DATA(newIndex)) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectIndex::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectIndex);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->setPluginIndex(ACTION_DATA(newIndex), ACTION_DATA(oldIndex));

				this->output("Undo Set Effect Index: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(oldIndex)) + "] " + juce::String(ACTION_DATA(newIndex)) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetInstrParamConnectToCC::ActionSetInstrParamConnectToCC(
	int instr, int param, int cc)
	: ACTION_DB{ instr, param, cc } {}

bool ActionSetInstrParamConnectToCC::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrParamConnectToCC);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(instr))) {
			if (auto instr = track->getInstrProcessor()) {
				ACTION_DATA(oldCC) = instr->getParamCCConnection(ACTION_DATA(param));
				ACTION_DATA(oldParam) = instr->getCCParamConnection(ACTION_DATA(cc));

				instr->connectParamCC(ACTION_DATA(param), ACTION_DATA(cc));

				this->output("Connect Instr Param To MIDI CC: [" + juce::String(ACTION_DATA(param)) + "] " + instr->getParamName(ACTION_DATA(param)) + " - MIDI CC " + juce::String(instr->getParamCCConnection(ACTION_DATA(param))) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrParamConnectToCC::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrParamConnectToCC);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(instr))) {
			if (auto instr = track->getInstrProcessor()) {
				instr->connectParamCC(ACTION_DATA(oldParam), ACTION_DATA(cc));
				if (ACTION_DATA(oldCC) > -1) {
					instr->connectParamCC(ACTION_DATA(param), ACTION_DATA(oldCC));
				}

				this->output("Undo Connect Instr Param To MIDI CC: [" + juce::String(ACTION_DATA(param)) + "] " + instr->getParamName(ACTION_DATA(param)) + " - MIDI CC " + juce::String(instr->getParamCCConnection(ACTION_DATA(param))) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetEffectParamConnectToCC::ActionSetEffectParamConnectToCC(
	int track, int effect, int param, int cc)
	: ACTION_DB{ track, effect, param, cc } {}

bool ActionSetEffectParamConnectToCC::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectParamConnectToCC);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
					ACTION_DATA(oldCC) = effect->getParamCCConnection(ACTION_DATA(param));
					ACTION_DATA(oldParam) = effect->getCCParamConnection(ACTION_DATA(cc));

					effect->connectParamCC(ACTION_DATA(param), ACTION_DATA(cc));

					this->output("Connect Effect Param To MIDI CC: [" + juce::String(ACTION_DATA(param)) + "] " + effect->getParamName(ACTION_DATA(param)) + " - MIDI CC " + juce::String(effect->getParamCCConnection(ACTION_DATA(param))) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectParamConnectToCC::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectParamConnectToCC);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
					effect->connectParamCC(ACTION_DATA(oldParam), ACTION_DATA(cc));
					if (ACTION_DATA(oldCC) > -1) {
						effect->connectParamCC(ACTION_DATA(param), ACTION_DATA(oldCC));
					}

					this->output("Undo Connect Effect Param To MIDI CC: [" + juce::String(ACTION_DATA(param)) + "] " + effect->getParamName(ACTION_DATA(param)) + " - MIDI CC " + juce::String(effect->getParamCCConnection(ACTION_DATA(param))) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetInstrParamConnectToCCByPtr::ActionSetInstrParamConnectToCCByPtr(
	quickAPI::PluginHolder instr, int param, int cc)
	: ACTION_DB{ instr, param, cc } {}

bool ActionSetInstrParamConnectToCCByPtr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrParamConnectToCCByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(instr)) {
		ACTION_DATA(oldCC) = ACTION_DATA(instr)->getParamCCConnection(ACTION_DATA(param));
		ACTION_DATA(oldParam) = ACTION_DATA(instr)->getCCParamConnection(ACTION_DATA(cc));

		ACTION_DATA(instr)->connectParamCC(ACTION_DATA(param), ACTION_DATA(cc));

		this->output("Connect Instr Param To MIDI CC: [" + juce::String(ACTION_DATA(param)) + "] " + ACTION_DATA(instr)->getParamName(ACTION_DATA(param)) + " - MIDI CC " + juce::String(ACTION_DATA(instr)->getParamCCConnection(ACTION_DATA(param))) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrParamConnectToCCByPtr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrParamConnectToCCByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(instr)) {
		ACTION_DATA(instr)->connectParamCC(ACTION_DATA(oldParam), ACTION_DATA(cc));
		if (ACTION_DATA(oldCC) > -1) {
			ACTION_DATA(instr)->connectParamCC(ACTION_DATA(param), ACTION_DATA(oldCC));
		}

		this->output("Undo Connect Instr Param To MIDI CC: [" + juce::String(ACTION_DATA(param)) + "] " + ACTION_DATA(instr)->getParamName(ACTION_DATA(param)) + " - MIDI CC " + juce::String(ACTION_DATA(instr)->getParamCCConnection(ACTION_DATA(param))) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionSetEffectParamConnectToCCByPtr::ActionSetEffectParamConnectToCCByPtr(
	quickAPI::PluginHolder effect, int param, int cc)
	: ACTION_DB{ effect, param, cc } {}

bool ActionSetEffectParamConnectToCCByPtr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectParamConnectToCCByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(effect)) {
		ACTION_DATA(oldCC) = ACTION_DATA(effect)->getParamCCConnection(ACTION_DATA(param));
		ACTION_DATA(oldParam) = ACTION_DATA(effect)->getCCParamConnection(ACTION_DATA(cc));

		ACTION_DATA(effect)->connectParamCC(ACTION_DATA(param), ACTION_DATA(cc));

		this->output("Connect Effect Param To MIDI CC: [" + juce::String(ACTION_DATA(param)) + "] " + ACTION_DATA(effect)->getParamName(ACTION_DATA(param)) + " - MIDI CC " + juce::String(ACTION_DATA(effect)->getParamCCConnection(ACTION_DATA(param))) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectParamConnectToCCByPtr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectParamConnectToCCByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(effect)) {
		ACTION_DATA(effect)->connectParamCC(ACTION_DATA(oldParam), ACTION_DATA(cc));
		if (ACTION_DATA(oldCC) > -1) {
			ACTION_DATA(effect)->connectParamCC(ACTION_DATA(param), ACTION_DATA(oldCC));
		}

		this->output("Undo Connect Effect Param To MIDI CC: [" + juce::String(ACTION_DATA(param)) + "] " + ACTION_DATA(effect)->getParamName(ACTION_DATA(param)) + " - MIDI CC " + juce::String(ACTION_DATA(effect)->getParamCCConnection(ACTION_DATA(param))) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionSetInstrMidiCCIntercept::ActionSetInstrMidiCCIntercept(
	int instr, bool intercept)
	: ACTION_DB{ instr, intercept } {}

bool ActionSetInstrMidiCCIntercept::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrMidiCCIntercept);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(instr))) {
			if (auto instr = track->getInstrProcessor()) {
				ACTION_DATA(oldIntercept) = instr->getMIDICCIntercept();

				instr->setMIDICCIntercept(ACTION_DATA(intercept));

				this->output("Set Instr MIDI CC Intercept: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(instr->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrMidiCCIntercept::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrMidiCCIntercept);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(instr))) {
			if (auto instr = track->getInstrProcessor()) {
				instr->setMIDICCIntercept(ACTION_DATA(oldIntercept));

				this->output("Undo Set Instr MIDI CC Intercept: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(instr->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetInstrMidiCCInterceptByPtr::ActionSetInstrMidiCCInterceptByPtr(
	quickAPI::PluginHolder instr, bool intercept)
	: ACTION_DB{ instr, intercept } {}

bool ActionSetInstrMidiCCInterceptByPtr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrMidiCCInterceptByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(instr)) {
		ACTION_DATA(oldIntercept) = ACTION_DATA(instr)->getMIDICCIntercept();

		ACTION_DATA(instr)->setMIDICCIntercept(ACTION_DATA(intercept));

		this->output("Set Instr MIDI CC Intercept: [" + ACTION_DATA(instr)->getName() + "] " + juce::String(ACTION_DATA(instr)->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrMidiCCInterceptByPtr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrMidiCCInterceptByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(instr)) {
		ACTION_DATA(instr)->setMIDICCIntercept(ACTION_DATA(oldIntercept));

		this->output("Undo Set Instr MIDI CC Intercept: [" + ACTION_DATA(instr)->getName() + "] " + juce::String(ACTION_DATA(instr)->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionSetEffectMidiCCIntercept::ActionSetEffectMidiCCIntercept(
	int track, int effect, bool intercept)
	: ACTION_DB{ track, effect, intercept } {}

bool ActionSetEffectMidiCCIntercept::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectMidiCCIntercept);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
					ACTION_DATA(oldIntercept) = effect->getMIDICCIntercept();

					effect->setMIDICCIntercept(ACTION_DATA(intercept));

					this->output("Set Effect MIDI CC Intercept: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + juce::String(effect->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectMidiCCIntercept::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectMidiCCIntercept);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
					effect->setMIDICCIntercept(ACTION_DATA(oldIntercept));

					this->output("Undo Set Effect MIDI CC Intercept: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + juce::String(effect->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetEffectMidiCCInterceptByPtr::ActionSetEffectMidiCCInterceptByPtr(
	quickAPI::PluginHolder effect, bool intercept)
	: ACTION_DB{ effect, intercept } {}

bool ActionSetEffectMidiCCInterceptByPtr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectMidiCCInterceptByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(effect)) {
		ACTION_DATA(oldIntercept) = ACTION_DATA(effect)->getMIDICCIntercept();

		ACTION_DATA(effect)->setMIDICCIntercept(ACTION_DATA(intercept));

		this->output("Set Effect MIDI CC Intercept: [" + ACTION_DATA(effect)->getName() + "] " + juce::String(ACTION_DATA(effect)->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectMidiCCInterceptByPtr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectMidiCCInterceptByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(effect)) {
		ACTION_DATA(effect)->setMIDICCIntercept(ACTION_DATA(oldIntercept));

		this->output("Undo Set Effect MIDI CC Intercept: [" + ACTION_DATA(effect)->getName() + "] " + juce::String(ACTION_DATA(effect)->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionSetInstrMidiOutputByPtr::ActionSetInstrMidiOutputByPtr(
	quickAPI::PluginHolder instr, bool output)
	: ACTION_DB{ instr, output} {}

bool ActionSetInstrMidiOutputByPtr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrMidiOutputByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(instr)) {
		ACTION_DATA(oldOutput) = ACTION_DATA(instr)->getMIDIOutput();

		ACTION_DATA(instr)->setMIDIOutput(ACTION_DATA(output));

		this->output("Set Instr MIDI Output: [" + ACTION_DATA(instr)->getName() + "] " + juce::String(ACTION_DATA(instr)->getMIDIOutput() ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrMidiOutputByPtr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrMidiOutputByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(instr)) {
		ACTION_DATA(instr)->setMIDIOutput(ACTION_DATA(oldOutput));

		this->output("Undo Set Instr MIDI Output: [" + ACTION_DATA(instr)->getName() + "] " + juce::String(ACTION_DATA(instr)->getMIDIOutput() ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionSetEffectMidiOutputByPtr::ActionSetEffectMidiOutputByPtr(
	quickAPI::PluginHolder effect, bool output)
	: ACTION_DB{ effect, output } {}

bool ActionSetEffectMidiOutputByPtr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectMidiOutputByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(effect)) {
		ACTION_DATA(oldOutput) = ACTION_DATA(effect)->getMIDIOutput();

		ACTION_DATA(effect)->setMIDIOutput(ACTION_DATA(output));

		this->output("Set Effect MIDI Output: [" + ACTION_DATA(effect)->getName() + "] " + juce::String(ACTION_DATA(effect)->getMIDIOutput() ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectMidiOutputByPtr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectMidiOutputByPtr);
	ACTION_WRITE_DB();

	if (ACTION_DATA(effect)) {
		ACTION_DATA(effect)->setMIDIOutput(ACTION_DATA(oldOutput));

		this->output("Set Effect MIDI Output: [" + ACTION_DATA(effect)->getName() + "] " + juce::String(ACTION_DATA(effect)->getMIDIOutput() ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionSetSequencerTrackBypass::ActionSetSequencerTrackBypass(
	int track, bool bypass)
	: ACTION_DB{ track, bypass } {}

bool ActionSetSequencerTrackBypass::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetSequencerTrackBypass);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		ACTION_DATA(oldBypass) = graph->getSourceBypass(ACTION_DATA(track));

		graph->setSourceBypass(ACTION_DATA(track), ACTION_DATA(bypass));

		this->output("Sequencer Track Bypass: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String(graph->getSourceBypass(ACTION_DATA(track)) ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetSequencerTrackBypass::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetSequencerTrackBypass);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setSourceBypass(ACTION_DATA(track), ACTION_DATA(oldBypass));

		this->output("Undo Sequencer Track Bypass: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String(graph->getSourceBypass(ACTION_DATA(track)) ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionSetMixerTrackMute::ActionSetMixerTrackMute(
	int track, bool mute)
	: ACTION_DB{ track, mute } {}

bool ActionSetMixerTrackMute::doAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetMixerTrackMute);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			ACTION_DATA(oldMute) = track->getMute();
			track->setMute(ACTION_DATA(mute));

			this->output("Set track mute: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String{ ACTION_DATA(mute) ? "ON" : "OFF" } + "\n");;
			ACTION_RESULT(true);
		}
	}
	this->output("Can't set track mute: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String{ ACTION_DATA(mute) ? "ON" : "OFF" } + "\n");
	ACTION_RESULT(false);
}

bool ActionSetMixerTrackMute::undo() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetMixerTrackMute);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			track->setMute(ACTION_DATA(oldMute));

			this->output("Undo set track mute: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String{ ACTION_DATA(mute) ? "ON" : "OFF" } + "\n");;
			ACTION_RESULT(true);
		}
	}
	this->output("Can't undo set track mute: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String{ ACTION_DATA(mute) ? "ON" : "OFF" } + "\n");
	ACTION_RESULT(false);
}

ActionSetMixerTrackName::ActionSetMixerTrackName(
	int track, const juce::String& name)
	: ACTION_DB{ track, name } {}

bool ActionSetMixerTrackName::doAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetMixerTrackName);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(name);
	ACTION_WRITE_STRING(oldName);

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			ACTION_DATA(oldName) = track->getTrackName();
			track->setTrackName(ACTION_DATA(name));

			this->output("Set track name: [" + juce::String(ACTION_DATA(track)) + "] " + ACTION_DATA(name) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't set track name: [" + juce::String(ACTION_DATA(track)) + "] " + ACTION_DATA(name) + "\n");
	ACTION_RESULT(false);
}

bool ActionSetMixerTrackName::undo() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetMixerTrackName);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(name);
	ACTION_WRITE_STRING(oldName);

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			track->setTrackName(ACTION_DATA(oldName));

			this->output("Undo set track name: [" + juce::String(ACTION_DATA(track)) + "] " + ACTION_DATA(name) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't undo set track name: [" + juce::String(ACTION_DATA(track)) + "] " + ACTION_DATA(name) + "\n");
	ACTION_RESULT(false);
}

ActionSetMixerTrackColor::ActionSetMixerTrackColor(
	int track, const juce::Colour& color)
	: ACTION_DB{ track, color } {}

bool ActionSetMixerTrackColor::doAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetMixerTrackColor);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			ACTION_DATA(oldColor) = track->getTrackColor();
			track->setTrackColor(ACTION_DATA(color));

			this->output("Set track color: [" + juce::String(ACTION_DATA(track)) + "] " + ACTION_DATA(color).toDisplayString(false) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't set track color: [" + juce::String(ACTION_DATA(track)) + "] " + ACTION_DATA(color).toDisplayString(false) + "\n");
	ACTION_RESULT(false);
}

bool ActionSetMixerTrackColor::undo() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetMixerTrackColor);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			track->setTrackColor(ACTION_DATA(oldColor));

			this->output("Undo set track color: [" + juce::String(ACTION_DATA(track)) + "] " + ACTION_DATA(color).toDisplayString(false) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't undo set track color: [" + juce::String(ACTION_DATA(track)) + "] " + ACTION_DATA(color).toDisplayString(false) + "\n");
	ACTION_RESULT(false);
}

ActionSetEffectWindow::ActionSetEffectWindow(
	int track, int effect, bool visible)
	: track(track), effect(effect), visible(visible) {}

bool ActionSetEffectWindow::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto plugin = pluginDock->getPluginProcessor(this->effect)) {
					if (auto editor = plugin->createEditorIfNeeded()) {
						if (this->visible) {
							editor->setName(plugin->getName());
							editor->addToDesktop(
								juce::ComponentPeer::windowAppearsOnTaskbar |
								juce::ComponentPeer::windowHasTitleBar |
								juce::ComponentPeer::windowHasDropShadow);
						}
						editor->setVisible(this->visible);

						if (this->visible) {
							editor->centreWithSize(editor->getWidth(), editor->getHeight());
						}

						this->output("Plugin Window: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + juce::String(this->visible ? "ON" : "OFF") + "\n");
						return true;
					}
				}
			}
		}
	}
	return false;
}

ActionSetPlayPosition::ActionSetPlayPosition(double pos)
	: pos(pos) {}

bool ActionSetPlayPosition::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	AudioCore::getInstance()->setPositon(this->pos);

	auto pos = AudioCore::getInstance()->getPosition();
	this->output("Set play position at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n");
	return true;
}

ActionSetReturnToStart::ActionSetReturnToStart(bool returnToStart)
	: returnToStart(returnToStart) {}

bool ActionSetReturnToStart::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	AudioCore::getInstance()->setReturnToPlayStartPosition(this->returnToStart);

	this->output("Set return to start position on play stop: " + juce::String(AudioCore::getInstance()->getReturnToPlayStartPosition() ? "ON" : "OFF") + "\n");
	return true;
}

ActionSetAudioSaveBitsPerSample::ActionSetAudioSaveBitsPerSample(
	const juce::String& format, int bitPerSample)
	: format(format), bitPerSample(bitPerSample) {}

bool ActionSetAudioSaveBitsPerSample::doAction() {
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	AudioSaveConfig::getInstance()->setBitsPerSample(
		this->format, this->bitPerSample);

	this->output("Set audio save bits per sample: [" + this->format + "] "
		+ juce::String(AudioSaveConfig::getInstance()->getBitsPerSample(this->format)) + "\n");
	return true;
}

ActionSetAudioSaveMetaData::ActionSetAudioSaveMetaData(
	const juce::String& format, const juce::StringPairArray& metaData)
	: format(format), metaData(metaData) {}

bool ActionSetAudioSaveMetaData::doAction() {
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	AudioSaveConfig::getInstance()->setMetaData(
		this->format, this->metaData);

	auto metaData = AudioSaveConfig::getInstance()->getMetaData(this->format);
	auto& metaKeys = metaData.getAllKeys();

	juce::String result;
	result += "Set audio save meta data: [" + this->format + "]\n";
	for (auto& i : metaKeys) {
		result += "    " + i + " : " + metaData.getValue(i, "") + "\n";
	}
	this->output(result);
	return true;
}

ActionSetAudioSaveQualityOptionIndex::ActionSetAudioSaveQualityOptionIndex(
	const juce::String& format, int quality)
	: format(format), quality(quality) {}

bool ActionSetAudioSaveQualityOptionIndex::doAction() {
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	AudioSaveConfig::getInstance()
		->setQualityOptionIndex(this->format, this->quality);

	this->output("Set audio save quality option index: [" + this->format + "] "
		+ juce::String(AudioSaveConfig::getInstance()->getQualityOptionIndex(this->format)) + "\n");
	return true;
}

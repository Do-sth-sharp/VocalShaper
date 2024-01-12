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

			juce::String result;
			result += "Set Mixer Track Gain Value: <" + juce::String(ACTION_DATA(track)) + "> " + juce::String(track->getGain()) + "\n";
			this->output(result);
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

			juce::String result;
			result += "Set Mixer Track Pan Value: <" + juce::String(ACTION_DATA(track)) + "> " + juce::String(track->getPan()) + "\n";
			this->output(result);
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

			juce::String result;
			result += "Set Mixer Track Slider Value: <" + juce::String(ACTION_DATA(track)) + "> " + juce::String(track->getSlider()) + "\n";
			this->output(result);
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
		ACTION_DATA(oldBypass) = graph->getInstrumentBypass(ACTION_DATA(instr));

		graph->setInstrumentBypass(ACTION_DATA(instr), ACTION_DATA(bypass));

		this->output("Plugin Bypass: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(ACTION_DATA(bypass) ? "ON" : "OFF") + "\n");
		ACTION_RESULT(true);
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
		graph->setInstrumentBypass(ACTION_DATA(instr), ACTION_DATA(oldBypass));

		this->output("Undo Plugin Bypass: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(ACTION_DATA(bypass) ? "ON" : "OFF") + "\n");
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
		if (auto instr = graph->getInstrumentProcessor(ACTION_DATA(instr))) {
			ACTION_DATA(oldChannel) = instr->getMIDIChannel();

			instr->setMIDIChannel(ACTION_DATA(channel));

			this->output("Plugin MIDI Channel: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(instr->getMIDIChannel()) + "\n");
			ACTION_RESULT(true);
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
		if (auto instr = graph->getInstrumentProcessor(ACTION_DATA(instr))) {
			instr->setMIDIChannel(ACTION_DATA(oldChannel));

			this->output("Undo Plugin MIDI Channel: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(instr->getMIDIChannel()) + "\n");
			ACTION_RESULT(true);
		}
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
		if (auto instr = graph->getInstrumentProcessor(ACTION_DATA(instr))) {
			ACTION_DATA(oldValue) = instr->getParamValue(ACTION_DATA(param));

			instr->setParamValue(ACTION_DATA(param), ACTION_DATA(value));

			this->output("Set Instr Param Value: [" + juce::String(ACTION_DATA(param)) + "] " + instr->getParamName(ACTION_DATA(param)) + " - " + juce::String(instr->getParamValue(ACTION_DATA(param))) + "\n");
			ACTION_RESULT(true);
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
		if (auto instr = graph->getInstrumentProcessor(ACTION_DATA(instr))) {
			instr->setParamValue(ACTION_DATA(param), ACTION_DATA(oldValue));

			this->output("Undo Set Instr Param Value: [" + juce::String(ACTION_DATA(param)) + "] " + instr->getParamName(ACTION_DATA(param)) + " - " + juce::String(instr->getParamValue(ACTION_DATA(param))) + "\n");
			ACTION_RESULT(true);
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
		if (auto instr = graph->getInstrumentProcessor(ACTION_DATA(instr))) {
			ACTION_DATA(oldCC) = instr->getParamCCConnection(ACTION_DATA(param));
			ACTION_DATA(oldParam) = instr->getCCParamConnection(ACTION_DATA(cc));

			instr->connectParamCC(ACTION_DATA(param), ACTION_DATA(cc));

			this->output("Connect Instr Param To MIDI CC: [" + juce::String(ACTION_DATA(param)) + "] " + instr->getParamName(ACTION_DATA(param)) + " - MIDI CC " + juce::String(instr->getParamCCConnection(ACTION_DATA(param))) + "\n");
			ACTION_RESULT(true);
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
		if (auto instr = graph->getInstrumentProcessor(ACTION_DATA(instr))) {
			instr->connectParamCC(ACTION_DATA(oldParam), ACTION_DATA(cc));
			if (ACTION_DATA(oldCC) > -1) {
				instr->connectParamCC(ACTION_DATA(param), ACTION_DATA(oldCC));
			}

			this->output("Undo Connect Instr Param To MIDI CC: [" + juce::String(ACTION_DATA(param)) + "] " + instr->getParamName(ACTION_DATA(param)) + " - MIDI CC " + juce::String(instr->getParamCCConnection(ACTION_DATA(param))) + "\n");
			ACTION_RESULT(true);
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
		if (auto instr = graph->getInstrumentProcessor(ACTION_DATA(instr))) {
			ACTION_DATA(oldIntercept) = instr->getMIDICCIntercept();

			instr->setMIDICCIntercept(ACTION_DATA(intercept));

			this->output("Set Instr MIDI CC Intercept: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(instr->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
			ACTION_RESULT(true);
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
		if (auto instr = graph->getInstrumentProcessor(ACTION_DATA(instr))) {
			instr->setMIDICCIntercept(ACTION_DATA(oldIntercept));

			this->output("Undo Set Instr MIDI CC Intercept: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(instr->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
			ACTION_RESULT(true);
		}
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

ActionSetSourceSynthesizer::ActionSetSourceSynthesizer(
	int index, const juce::String& pid)
	: ACTION_DB{ index, pid } {}

bool ActionSetSourceSynthesizer::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change synthesizer while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetSourceSynthesizer);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(pid);

	if (CloneableSourceManager::getInstance()->setSourceSynthesizer(ACTION_DATA(index), ACTION_DATA(pid))) {
		this->output("Set synthesizer: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + "\n");
		ACTION_RESULT(true);
	}
	this->error("Can't set synthesizer: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + "\n");
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

ActionSetInstrWindow::ActionSetInstrWindow(
	int instr, bool visible)
	: instr(instr), visible(visible) {}

bool ActionSetInstrWindow::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto plugin = graph->getInstrumentProcessor(this->instr)) {
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

				this->output("Plugin Window: [" + juce::String(this->instr) + "] " + juce::String(this->visible ? "ON" : "OFF") + "\n");
				return true;
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

#include "ActionSet.h"
#include "ActionUtils.h"

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
	: track(track), value(value) {}

bool ActionSetMixerTrackGain::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			this->oldValue = track->getGain();

			track->setGain(this->value);

			juce::String result;
			result += "Set Mixer Track Gain Value: <" + juce::String(this->track) + "> " + juce::String(track->getGain()) + "\n";
			this->output(result);
			return true;
		}
	}

	return false;
}

bool ActionSetMixerTrackGain::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			track->setGain(this->oldValue);

			juce::String result;
			result += "Undo Set Mixer Track Gain Value: <" + juce::String(this->track) + "> " + juce::String(track->getGain()) + "\n";
			this->output(result);
			return true;
		}
	}

	return false;
}

ActionSetMixerTrackPan::ActionSetMixerTrackPan(
	int track, float value)
	: track(track), value(value) {}

bool ActionSetMixerTrackPan::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			this->oldValue = track->getPan();

			track->setPan(this->value);

			juce::String result;
			result += "Set Mixer Track Pan Value: <" + juce::String(this->track) + "> " + juce::String(track->getPan()) + "\n";
			this->output(result);
			return true;
		}
	}

	return false;
}

bool ActionSetMixerTrackPan::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			track->setPan(this->oldValue);

			juce::String result;
			result += "Undo Set Mixer Track Pan Value: <" + juce::String(this->track) + "> " + juce::String(track->getPan()) + "\n";
			this->output(result);
			return true;
		}
	}

	return false;
}

ActionSetMixerTrackSlider::ActionSetMixerTrackSlider(
	int track, float value)
	: track(track), value(value) {}

bool ActionSetMixerTrackSlider::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			this->oldValue = track->getSlider();

			track->setSlider(this->value);

			juce::String result;
			result += "Set Mixer Track Slider Value: <" + juce::String(this->track) + "> " + juce::String(track->getSlider()) + "\n";
			this->output(result);
			return true;
		}
	}

	return false;
}

bool ActionSetMixerTrackSlider::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			track->setSlider(this->oldValue);

			juce::String result;
			result += "Undo Set Mixer Track Slider Value: <" + juce::String(this->track) + "> " + juce::String(track->getSlider()) + "\n";
			this->output(result);
			return true;
		}
	}

	return false;
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

ActionSetEffectBypass::ActionSetEffectBypass(
	int track, int effect, bool bypass)
	: track(track), effect(effect), bypass(bypass) {}

bool ActionSetEffectBypass::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				this->oldBypass = pluginDock->getPluginBypass(this->effect);

				pluginDock->setPluginBypass(this->effect, this->bypass);

				this->output("Plugin Bypass: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + juce::String(this->bypass ? "ON" : "OFF") + "\n");
				return true;
			}
		}
	}
	return false;
}

bool ActionSetEffectBypass::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->setPluginBypass(this->effect, this->oldBypass);

				this->output("Undo Plugin Bypass: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + juce::String(this->bypass ? "ON" : "OFF") + "\n");
				return true;
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

ActionSetInstrBypass::ActionSetInstrBypass(
	int instr, bool bypass)
	: instr(instr), bypass(bypass) {}

bool ActionSetInstrBypass::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		this->oldBypass = graph->getInstrumentBypass(this->instr);

		graph->setInstrumentBypass(this->instr, this->bypass);

		this->output("Plugin Bypass: [" + juce::String(this->instr) + "] " + juce::String(this->bypass ? "ON" : "OFF") + "\n");
		return true;
	}
	return false;
}

bool ActionSetInstrBypass::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setInstrumentBypass(this->instr, this->oldBypass);

		this->output("Undo Plugin Bypass: [" + juce::String(this->instr) + "] " + juce::String(this->bypass ? "ON" : "OFF") + "\n");
		return true;
	}
	return false;
}

ActionSetInstrMidiChannel::ActionSetInstrMidiChannel(
	int instr, int channel)
	: instr(instr), channel(channel) {}

bool ActionSetInstrMidiChannel::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto instr = graph->getInstrumentProcessor(this->instr)) {
			this->oldChannel = instr->getMIDIChannel();

			instr->setMIDIChannel(this->channel);

			this->output("Plugin MIDI Channel: [" + juce::String(this->instr) + "] " + juce::String(instr->getMIDIChannel()) + "\n");
			return true;
		}
	}
	return false;
}

bool ActionSetInstrMidiChannel::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto instr = graph->getInstrumentProcessor(this->instr)) {
			instr->setMIDIChannel(this->oldChannel);

			this->output("Undo Plugin MIDI Channel: [" + juce::String(this->instr) + "] " + juce::String(instr->getMIDIChannel()) + "\n");
			return true;
		}
	}
	return false;
}

ActionSetEffectMidiChannel::ActionSetEffectMidiChannel(
	int track, int effect, int channel)
	: track(track), effect(effect), channel(channel) {}

bool ActionSetEffectMidiChannel::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					this->oldChannel = effect->getMIDIChannel();

					effect->setMIDIChannel(this->channel);

					this->output("Plugin MIDI Channel: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + juce::String(effect->getMIDIChannel()) + "\n");
					return true;
				}
			}
		}
	}
	return false;
}

bool ActionSetEffectMidiChannel::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					effect->setMIDIChannel(this->oldChannel);

					this->output("Undo Plugin MIDI Channel: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + juce::String(effect->getMIDIChannel()) + "\n");
					return true;
				}
			}
		}
	}
	return false;
}

ActionSetInstrParamValue::ActionSetInstrParamValue(
	int instr, int param, float value)
	: instr(instr), param(param), value(value) {}

bool ActionSetInstrParamValue::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto instr = graph->getInstrumentProcessor(this->instr)) {
			this->oldValue = instr->getParamValue(this->param);

			instr->setParamValue(this->param, this->value);

			this->output("Set Instr Param Value: [" + juce::String(this->param) + "] " + instr->getParamName(this->param) + " - " + juce::String(instr->getParamValue(this->param)) + "\n");
			return true;
		}
	}
	return false;
}

bool ActionSetInstrParamValue::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto instr = graph->getInstrumentProcessor(this->instr)) {
			instr->setParamValue(this->param, this->oldValue);

			this->output("Undo Set Instr Param Value: [" + juce::String(this->param) + "] " + instr->getParamName(this->param) + " - " + juce::String(instr->getParamValue(this->param)) + "\n");
			return true;
		}
	}
	return false;
}

ActionSetEffectParamValue::ActionSetEffectParamValue(
	int track, int effect, int param, float value)
	: track(track), effect(effect), param(param), value(value) {}

bool ActionSetEffectParamValue::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					this->oldValue = effect->getParamValue(this->param);

					effect->setParamValue(this->param, this->value);

					this->output("Effect Param Value: [" + juce::String(this->param) + "] " + effect->getParamName(this->param) + " - " + juce::String(effect->getParamValue(this->param)) + "\n");
					return true;
				}
			}
		}
	}
	return false;
}

bool ActionSetEffectParamValue::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					effect->setParamValue(this->param, this->oldValue);

					this->output("Undo Effect Param Value: [" + juce::String(this->param) + "] " + effect->getParamName(this->param) + " - " + juce::String(effect->getParamValue(this->param)) + "\n");
					return true;
				}
			}
		}
	}
	return false;
}

ActionSetInstrParamConnectToCC::ActionSetInstrParamConnectToCC(
	int instr, int param, int cc)
	: instr(instr), param(param), cc(cc) {}

bool ActionSetInstrParamConnectToCC::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto instr = graph->getInstrumentProcessor(this->instr)) {
			this->oldCC = instr->getParamCCConnection(this->param);
			this->oldParam = instr->getCCParamConnection(this->cc);

			instr->connectParamCC(this->param, this->cc);

			this->output("Connect Instr Param To MIDI CC: [" + juce::String(this->param) + "] " + instr->getParamName(this->param) + " - MIDI CC " + juce::String(instr->getParamCCConnection(this->param)) + "\n");
			return true;
		}
	}
	return false;
}

bool ActionSetInstrParamConnectToCC::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto instr = graph->getInstrumentProcessor(this->instr)) {
			instr->connectParamCC(this->oldParam, this->cc);
			if (this->oldCC > -1) {
				instr->connectParamCC(this->param, this->oldCC);
			}

			this->output("Undo Connect Instr Param To MIDI CC: [" + juce::String(this->param) + "] " + instr->getParamName(this->param) + " - MIDI CC " + juce::String(instr->getParamCCConnection(this->param)) + "\n");
			return true;
		}
	}
	return false;
}

ActionSetEffectParamConnectToCC::ActionSetEffectParamConnectToCC(
	int track, int effect, int param, int cc)
	: track(track), effect(effect), param(param), cc(cc) {}

bool ActionSetEffectParamConnectToCC::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					this->oldCC = effect->getParamCCConnection(this->param);
					this->oldParam = effect->getCCParamConnection(this->cc);

					effect->connectParamCC(this->param, this->cc);

					this->output("Connect Effect Param To MIDI CC: [" + juce::String(this->param) + "] " + effect->getParamName(this->param) + " - MIDI CC " + juce::String(effect->getParamCCConnection(this->param)) + "\n");
					return true;
				}
			}
		}
	}
	return false;
}

bool ActionSetEffectParamConnectToCC::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					effect->connectParamCC(this->oldParam, this->cc);
					if (this->oldCC > -1) {
						effect->connectParamCC(this->param, this->oldCC);
					}

					this->output("Undo Connect Effect Param To MIDI CC: [" + juce::String(this->param) + "] " + effect->getParamName(this->param) + " - MIDI CC " + juce::String(effect->getParamCCConnection(this->param)) + "\n");
					return true;
				}
			}
		}
	}
	return false;
}

ActionSetInstrMidiCCIntercept::ActionSetInstrMidiCCIntercept(
	int instr, bool intercept)
	: instr(instr), intercept(intercept) {}

bool ActionSetInstrMidiCCIntercept::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto instr = graph->getInstrumentProcessor(this->instr)) {
			this->oldIntercept = instr->getMIDICCIntercept();

			instr->setMIDICCIntercept(this->intercept);

			this->output("Set Instr MIDI CC Intercept: [" + juce::String(this->instr) + "] " + juce::String(instr->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
			return true;
		}
	}
	return false;
}

bool ActionSetInstrMidiCCIntercept::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto instr = graph->getInstrumentProcessor(this->instr)) {
			instr->setMIDICCIntercept(this->oldIntercept);

			this->output("Undo Set Instr MIDI CC Intercept: [" + juce::String(this->instr) + "] " + juce::String(instr->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
			return true;
		}
	}
	return false;
}

ActionSetEffectMidiCCIntercept::ActionSetEffectMidiCCIntercept(
	int track, int effect, bool intercept)
	: track(track), effect(effect), intercept(intercept) {}

bool ActionSetEffectMidiCCIntercept::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					this->oldIntercept = effect->getMIDICCIntercept();

					effect->setMIDICCIntercept(this->intercept);

					this->output("Set Effect MIDI CC Intercept: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + juce::String(effect->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
					return true;
				}
			}
		}
	}
	return false;
}

bool ActionSetEffectMidiCCIntercept::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					effect->setMIDICCIntercept(this->oldIntercept);

					this->output("Undo Set Effect MIDI CC Intercept: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + juce::String(effect->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
					return true;
				}
			}
		}
	}
	return false;
}

ActionSetSequencerTrackBypass::ActionSetSequencerTrackBypass(
	int track, bool bypass)
	: track(track), bypass(bypass) {}

bool ActionSetSequencerTrackBypass::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		this->oldBypass = graph->getSourceBypass(this->track);

		graph->setSourceBypass(this->track, this->bypass);

		this->output("Sequencer Track Bypass: [" + juce::String(this->track) + "] " + juce::String(graph->getSourceBypass(this->track) ? "ON" : "OFF") + "\n");
		return true;
	}
	return false;
}

bool ActionSetSequencerTrackBypass::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setSourceBypass(this->track, this->oldBypass);

		this->output("Undo Sequencer Track Bypass: [" + juce::String(this->track) + "] " + juce::String(graph->getSourceBypass(this->track) ? "ON" : "OFF") + "\n");
		return true;
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

ActionSetSourceSynthesizer::ActionSetSourceSynthesizer(
	int index, const juce::String& pid)
	: index(index), pid(pid) {}

bool ActionSetSourceSynthesizer::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (CloneableSourceManager::getInstance()->setSourceSynthesizer(this->index, this->pid)) {
		this->output("Set synthesizer: [" + juce::String(this->index) + "] " + this->pid + "\n");
		return true;
	}
	this->output("Can't set synthesizer: [" + juce::String(this->index) + "] " + this->pid + "\n");
	return false;
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

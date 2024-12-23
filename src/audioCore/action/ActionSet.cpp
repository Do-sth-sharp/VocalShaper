#include "ActionSet.h"

#include "../AudioCore.h"
#include "../misc/Device.h"
#include "../misc/PlayPosition.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

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

ActionSetTrackGain::ActionSetTrackGain(
	quickAPI::TrackIndex track, float value)
	: ACTION_DB{ track, value } {}

bool ActionSetTrackGain::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetTrackGain);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				ACTION_DATA(oldValue) = mixer->getGain();

				mixer->setGain(ACTION_DATA(value));
				ACTION_RESULT(true);
			}
		}
	}

	ACTION_RESULT(false);
}

bool ActionSetTrackGain::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTrackGain);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				mixer->setGain(ACTION_DATA(oldValue));
				ACTION_RESULT(true);
			}
			
		}
	}

	ACTION_RESULT(false);
}

juce::UndoableAction* ActionSetTrackGain::createCoalescedAction(
	juce::UndoableAction* nextAction) {
	if (auto action = dynamic_cast<ActionSetTrackGain*>(nextAction)) {
		if (this->_data.track == action->_data.track) {
			auto newAction = std::make_unique<ActionSetTrackGain>(
				action->_data.track, action->_data.value);
			newAction->_data.oldValue = this->_data.oldValue;
			return newAction.release();
		}
	}
	return nullptr;
}

ActionSetTrackPan::ActionSetTrackPan(
	quickAPI::TrackIndex track, float value)
	: ACTION_DB{ track, value } {}

bool ActionSetTrackPan::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetTrackPan);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				ACTION_DATA(oldValue) = mixer->getPan();

				mixer->setPan(ACTION_DATA(value));
				ACTION_RESULT(true);
			}
		}
	}

	ACTION_RESULT(false);
}

bool ActionSetTrackPan::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTrackPan);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				mixer->setPan(ACTION_DATA(oldValue));
				ACTION_RESULT(true);
			}
		}
	}

	ACTION_RESULT(false);
}

juce::UndoableAction* ActionSetTrackPan::createCoalescedAction(
	juce::UndoableAction* nextAction) {
	if (auto action = dynamic_cast<ActionSetTrackPan*>(nextAction)) {
		if (this->_data.track == action->_data.track) {
			auto newAction = std::make_unique<ActionSetTrackPan>(
				action->_data.track, action->_data.value);
			newAction->_data.oldValue = this->_data.oldValue;
			return newAction.release();
		}
	}
	return nullptr;
}

ActionSetTrackFader::ActionSetTrackFader(
	quickAPI::TrackIndex track, float value)
	: ACTION_DB{ track, value } {}

bool ActionSetTrackFader::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetTrackFader);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				ACTION_DATA(oldValue) = mixer->getFader();

				mixer->setFader(ACTION_DATA(value));
				ACTION_RESULT(true);
			}
		}
	}

	ACTION_RESULT(false);
}

bool ActionSetTrackFader::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTrackFader);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				mixer->setFader(ACTION_DATA(oldValue));
				ACTION_RESULT(true);
			}
		}
	}

	ACTION_RESULT(false);
}

juce::UndoableAction* ActionSetTrackFader::createCoalescedAction(
	juce::UndoableAction* nextAction) {
	if (auto action = dynamic_cast<ActionSetTrackFader*>(nextAction)) {
		if (this->_data.track == action->_data.track) {
			auto newAction = std::make_unique<ActionSetTrackFader>(
				action->_data.track, action->_data.value);
			newAction->_data.oldValue = this->_data.oldValue;
			return newAction.release();
		}
	}
	return nullptr;
}

ActionSetEffectBypass::ActionSetEffectBypass(
	quickAPI::TrackIndex track, int effect, bool bypass)
	: ACTION_DB{ track, effect, bypass } {}

bool ActionSetEffectBypass::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectBypass);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					ACTION_DATA(oldBypass) = pluginDock->getPluginBypass(ACTION_DATA(effect));

					pluginDock->setPluginBypass(ACTION_DATA(effect), ACTION_DATA(bypass));

					this->output("Plugin Bypass: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + ", " + juce::String{ ACTION_DATA(effect) } + "] " + juce::String{ ACTION_DATA(bypass) ? "ON" : "OFF" } + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectBypass::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectBypass);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					pluginDock->setPluginBypass(ACTION_DATA(effect), ACTION_DATA(oldBypass));

					this->output("Undo Plugin Bypass: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + ", " + juce::String{ ACTION_DATA(effect) } + "] " + juce::String{ ACTION_DATA(bypass) ? "ON" : "OFF" } + "\n");
					ACTION_RESULT(true);
				}
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
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				ACTION_DATA(oldBypass) = seq->getInstrumentBypass();

				seq->setInstrumentBypass(ACTION_DATA(bypass));

				this->output("Plugin Bypass: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(ACTION_DATA(bypass) ? "ON" : "OFF") + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrBypass::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrBypass);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				seq->setInstrumentBypass(ACTION_DATA(oldBypass));

				this->output("Undo Plugin Bypass: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(ACTION_DATA(bypass) ? "ON" : "OFF") + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetInstrMidiChannel::ActionSetInstrMidiChannel(
	int instr, int channel)
	: ACTION_DB{ instr, channel } {
}

bool ActionSetInstrMidiChannel::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrMidiChannel);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					ACTION_DATA(oldChannel) = instr->getMIDIChannel();

					instr->setMIDIChannel(ACTION_DATA(channel));

					this->output("Plugin MIDI Channel: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(instr->getMIDIChannel()) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrMidiChannel::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrMidiChannel);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					instr->setMIDIChannel(ACTION_DATA(oldChannel));

					this->output("Undo Plugin MIDI Channel: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(instr->getMIDIChannel()) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetEffectMidiChannel::ActionSetEffectMidiChannel(
	quickAPI::TrackIndex track, int effect, int channel)
	: ACTION_DB{ track, effect, channel } {
}

bool ActionSetEffectMidiChannel::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectMidiChannel);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
						ACTION_DATA(oldChannel) = effect->getMIDIChannel();

						effect->setMIDIChannel(ACTION_DATA(channel));

						this->output("Plugin MIDI Channel: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + ", " + juce::String{ ACTION_DATA(effect) } + "] " + juce::String{ effect->getMIDIChannel() } + "\n");
						ACTION_RESULT(true);
					}
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectMidiChannel::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectMidiChannel);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
						effect->setMIDIChannel(ACTION_DATA(oldChannel));

						this->output("Undo Plugin MIDI Channel: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + ", " + juce::String{ ACTION_DATA(effect) } + "] " + juce::String{ effect->getMIDIChannel() } + "\n");
						ACTION_RESULT(true);
					}
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetInstrParamValue::ActionSetInstrParamValue(
	int instr, int param, float value)
	: ACTION_DB{ instr, param, value } {
}

bool ActionSetInstrParamValue::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrParamValue);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					ACTION_DATA(oldValue) = instr->getParamValue(ACTION_DATA(param));

					instr->setParamValue(ACTION_DATA(param), ACTION_DATA(value));

					this->output("Set Instr Param Value: [" + juce::String(ACTION_DATA(param)) + "] " + instr->getParamName(ACTION_DATA(param)) + " - " + juce::String(instr->getParamValue(ACTION_DATA(param))) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrParamValue::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrParamValue);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					instr->setParamValue(ACTION_DATA(param), ACTION_DATA(oldValue));

					this->output("Undo Set Instr Param Value: [" + juce::String(ACTION_DATA(param)) + "] " + instr->getParamName(ACTION_DATA(param)) + " - " + juce::String(instr->getParamValue(ACTION_DATA(param))) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetEffectParamValue::ActionSetEffectParamValue(
	quickAPI::TrackIndex track, int effect, int param, float value)
	: ACTION_DB{ track, effect, param, value } {
}

bool ActionSetEffectParamValue::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectParamValue);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
						ACTION_DATA(oldValue) = effect->getParamValue(ACTION_DATA(param));

						effect->setParamValue(ACTION_DATA(param), ACTION_DATA(value));

						this->output("Effect Param Value: [" + juce::String(ACTION_DATA(param)) + "] " + effect->getParamName(ACTION_DATA(param)) + " - " + juce::String(effect->getParamValue(ACTION_DATA(param))) + "\n");
						ACTION_RESULT(true);
					}
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectParamValue::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectParamValue);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
						effect->setParamValue(ACTION_DATA(param), ACTION_DATA(oldValue));

						this->output("Undo Effect Param Value: [" + juce::String(ACTION_DATA(param)) + "] " + effect->getParamName(ACTION_DATA(param)) + " - " + juce::String(effect->getParamValue(ACTION_DATA(param))) + "\n");
						ACTION_RESULT(true);
					}
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetInstrParamConnectToCC::ActionSetInstrParamConnectToCC(
	int instr, int param, int cc)
	: ACTION_DB{ instr, param, cc } {
}

bool ActionSetInstrParamConnectToCC::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrParamConnectToCC);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					ACTION_DATA(oldCC) = instr->getParamCCConnection(ACTION_DATA(param));
					ACTION_DATA(oldParam) = instr->getCCParamConnection(ACTION_DATA(cc));

					instr->connectParamCC(ACTION_DATA(param), ACTION_DATA(cc));

					this->output("Connect Instr Param To MIDI CC: [" + juce::String(ACTION_DATA(param)) + "] " + instr->getParamName(ACTION_DATA(param)) + " - MIDI CC " + juce::String(instr->getParamCCConnection(ACTION_DATA(param))) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrParamConnectToCC::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrParamConnectToCC);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					instr->connectParamCC(ACTION_DATA(oldParam), ACTION_DATA(cc));
					if (ACTION_DATA(oldCC) > -1) {
						instr->connectParamCC(ACTION_DATA(param), ACTION_DATA(oldCC));
					}

					this->output("Undo Connect Instr Param To MIDI CC: [" + juce::String(ACTION_DATA(param)) + "] " + instr->getParamName(ACTION_DATA(param)) + " - MIDI CC " + juce::String(instr->getParamCCConnection(ACTION_DATA(param))) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetEffectParamConnectToCC::ActionSetEffectParamConnectToCC(
	quickAPI::TrackIndex track, int effect, int param, int cc)
	: ACTION_DB{ track, effect, param, cc } {
}

bool ActionSetEffectParamConnectToCC::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectParamConnectToCC);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
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
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectParamConnectToCC::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectParamConnectToCC);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
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
	}
	ACTION_RESULT(false);
}

ActionSetInstrMidiCCIntercept::ActionSetInstrMidiCCIntercept(
	int instr, bool intercept)
	: ACTION_DB{ instr, intercept } {
}

bool ActionSetInstrMidiCCIntercept::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrMidiCCIntercept);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					ACTION_DATA(oldIntercept) = instr->getMIDICCIntercept();

					instr->setMIDICCIntercept(ACTION_DATA(intercept));

					this->output("Set Instr MIDI CC Intercept: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(instr->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrMidiCCIntercept::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrMidiCCIntercept);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					instr->setMIDICCIntercept(ACTION_DATA(oldIntercept));

					this->output("Undo Set Instr MIDI CC Intercept: [" + juce::String(ACTION_DATA(instr)) + "] " + juce::String(instr->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetEffectMidiCCIntercept::ActionSetEffectMidiCCIntercept(
	quickAPI::TrackIndex track, int effect, bool intercept)
	: ACTION_DB{ track, effect, intercept } {
}

bool ActionSetEffectMidiCCIntercept::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectMidiCCIntercept);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
						ACTION_DATA(oldIntercept) = effect->getMIDICCIntercept();

						effect->setMIDICCIntercept(ACTION_DATA(intercept));

						this->output("Set Effect MIDI CC Intercept: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + ", " + juce::String{ ACTION_DATA(effect) } + "] " + juce::String{ effect->getMIDICCIntercept() ? "ON" : "OFF" } + "\n");
						ACTION_RESULT(true);
					}
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectMidiCCIntercept::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectMidiCCIntercept);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
						effect->setMIDICCIntercept(ACTION_DATA(oldIntercept));

						this->output("Undo Set Effect MIDI CC Intercept: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + ", " + juce::String{ ACTION_DATA(effect) } + "] " + juce::String{ effect->getMIDICCIntercept() ? "ON" : "OFF" } + "\n");
						ACTION_RESULT(true);
					}
				}
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

bool ActionSetEffectBypassByPtr::undoAction() {
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

bool ActionSetInstrBypassByPtr::undoAction() {
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

bool ActionSetInstrMidiChannelByPtr::undoAction() {
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

bool ActionSetEffectMidiChannelByPtr::undoAction() {
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

ActionSetInstrMidiCCInterceptByPtr::ActionSetInstrMidiCCInterceptByPtr(
	quickAPI::PluginHolder instr, bool intercept)
	: ACTION_DB{ instr, intercept } {
}

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

bool ActionSetInstrMidiCCInterceptByPtr::undoAction() {
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

ActionSetEffectMidiCCInterceptByPtr::ActionSetEffectMidiCCInterceptByPtr(
	quickAPI::PluginHolder effect, bool intercept)
	: ACTION_DB{ effect, intercept } {
}

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

bool ActionSetEffectMidiCCInterceptByPtr::undoAction() {
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
	: ACTION_DB{ instr, output } {
}

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

bool ActionSetInstrMidiOutputByPtr::undoAction() {
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
	: ACTION_DB{ effect, output } {
}

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

bool ActionSetEffectMidiOutputByPtr::undoAction() {
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

ActionSetInstrParamConnectToCCByPtr::ActionSetInstrParamConnectToCCByPtr(
	quickAPI::PluginHolder instr, int param, int cc)
	: ACTION_DB{ instr, param, cc } {
}

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

bool ActionSetInstrParamConnectToCCByPtr::undoAction() {
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
	: ACTION_DB{ effect, param, cc } {
}

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

bool ActionSetEffectParamConnectToCCByPtr::undoAction() {
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

ActionSetTrackName::ActionSetTrackName(
	quickAPI::TrackIndex track, const juce::String& name)
	: ACTION_DB{ track, name } {
}

bool ActionSetTrackName::doAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetTrackName);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(name);
	ACTION_WRITE_STRING(oldName);

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {

			ACTION_DATA(oldName) = track->getTrackName();
			track->setTrackName(ACTION_DATA(name));

			this->output("Set track name: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + ACTION_DATA(name) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't set track name: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + ACTION_DATA(name) + "\n");
	ACTION_RESULT(false);
}

bool ActionSetTrackName::undoAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTrackName);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(name);
	ACTION_WRITE_STRING(oldName);

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			track->setTrackName(ACTION_DATA(oldName));

			this->output("Undo set track name: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + ACTION_DATA(name) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't undo set track name: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + ACTION_DATA(name) + "\n");
	ACTION_RESULT(false);
}

ActionSetTrackColor::ActionSetTrackColor(
	quickAPI::TrackIndex track, const juce::Colour& color)
	: ACTION_DB{ track, color } {
}

bool ActionSetTrackColor::doAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetTrackColor);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			ACTION_DATA(oldColor) = track->getTrackColor();
			track->setTrackColor(ACTION_DATA(color));

			this->output("Set track color: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + ACTION_DATA(color).toDisplayString(false) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't set track color: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + ACTION_DATA(color).toDisplayString(false) + "\n");
	ACTION_RESULT(false);
}

bool ActionSetTrackColor::undoAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTrackColor);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			track->setTrackColor(ACTION_DATA(oldColor));

			this->output("Undo set track color: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + ACTION_DATA(color).toDisplayString(false) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't undo set track color: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + ACTION_DATA(color).toDisplayString(false) + "\n");
	ACTION_RESULT(false);
}

ActionSetEffectIndex::ActionSetEffectIndex(
	quickAPI::TrackIndex track, int oldIndex, int newIndex)
	: ACTION_DB{ track, oldIndex, newIndex } {}

bool ActionSetEffectIndex::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffectIndex);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (pluginDock->setPluginIndex(ACTION_DATA(oldIndex), ACTION_DATA(newIndex))) {
						this->output("Set Effect Index: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + ", " + juce::String{ ACTION_DATA(oldIndex) } + "] " + juce::String{ ACTION_DATA(newIndex) } + "\n");
						ACTION_RESULT(true);
					}
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetEffectIndex::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffectIndex);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (pluginDock->setPluginIndex(ACTION_DATA(newIndex), ACTION_DATA(oldIndex))) {
						this->output("Undo Set Effect Index: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + ", " + juce::String{ ACTION_DATA(oldIndex) } + "] " + juce::String{ ACTION_DATA(newIndex) } + "\n");
						ACTION_RESULT(true);
					}
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetInstrOffline::ActionSetInstrOffline(
	int instr, bool offline)
	: ACTION_DB{ instr, offline } {
}

bool ActionSetInstrOffline::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetInstrOffline);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				ACTION_DATA(oldOffline) = seq->getInstrOffline();

				seq->setInstrOffline(ACTION_DATA(offline));

				this->output("Instr Offline: [" + juce::String{ ACTION_DATA(instr) } + "] " + juce::String{ seq->getInstrOffline() ? "ON" : "OFF" } + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetInstrOffline::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetInstrOffline);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				seq->setInstrOffline(ACTION_DATA(oldOffline));

				this->output("Undo Instr Offline: [" + juce::String{ ACTION_DATA(instr) } + "] " + juce::String{ seq->getInstrOffline() ? "ON" : "OFF" } + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetTempoTime::ActionSetTempoTime(
	int index, double time)
	: ACTION_DB{ index, time } {
}

bool ActionSetTempoTime::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetTempoTime);
	ACTION_WRITE_DB();

	if (ACTION_DATA(index) >= 0 && ACTION_DATA(index) < PlayPosition::getInstance()->getTempoLabelNum()) {
		ACTION_DATA(oldTime) = PlayPosition::getInstance()->getTempoLabelTime(ACTION_DATA(index));

		ACTION_DATA(newIndex) = PlayPosition::getInstance()->setTempoLabelTime(
			ACTION_DATA(index), ACTION_DATA(time));

		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetTempoTime::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTempoTime);
	ACTION_WRITE_DB();

	PlayPosition::getInstance()->setTempoLabelTime(
		ACTION_DATA(newIndex), ACTION_DATA(oldTime), ACTION_DATA(index));

	ACTION_RESULT(true);
}

ActionSetTempoTempo::ActionSetTempoTempo(
	int index, double tempo)
	: ACTION_DB{ index, tempo } {
}

bool ActionSetTempoTempo::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetTempoTempo);
	ACTION_WRITE_DB();

	if (ACTION_DATA(index) >= 0 && ACTION_DATA(index) < PlayPosition::getInstance()->getTempoLabelNum()) {
		ACTION_DATA(oldTempo) = PlayPosition::getInstance()->getTempoLabelTempo(ACTION_DATA(index));

		PlayPosition::getInstance()->setTempoLabelTempo(ACTION_DATA(index), ACTION_DATA(tempo));

		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetTempoTempo::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTempoTempo);
	ACTION_WRITE_DB();

	PlayPosition::getInstance()->setTempoLabelTempo(ACTION_DATA(index), ACTION_DATA(oldTempo));

	ACTION_RESULT(true);
}

ActionSetTempoBeat::ActionSetTempoBeat(
	int index, int numerator, int denominator)
	: ACTION_DB{ index, numerator, denominator } {
}

bool ActionSetTempoBeat::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetTempoBeat);
	ACTION_WRITE_DB();

	if (ACTION_DATA(index) >= 0 && ACTION_DATA(index) < PlayPosition::getInstance()->getTempoLabelNum()) {
		std::tie(ACTION_DATA(oldNumerator), ACTION_DATA(oldDenominator)) = PlayPosition::getInstance()->getTempoLabelBeat(ACTION_DATA(index));

		PlayPosition::getInstance()->setTempoLabelBeat(ACTION_DATA(index), ACTION_DATA(numerator), ACTION_DATA(denominator));

		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionSetTempoBeat::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTempoBeat);
	ACTION_WRITE_DB();

	PlayPosition::getInstance()->setTempoLabelBeat(ACTION_DATA(index), ACTION_DATA(numerator), ACTION_DATA(denominator));

	ACTION_RESULT(true);
}

ActionSetTrackMute::ActionSetTrackMute(
	quickAPI::TrackIndex track, bool mute)
	: ACTION_DB{ track, mute } {}

bool ActionSetTrackMute::doAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetTrackMute);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			ACTION_DATA(oldMute) = track->getMute();
			track->setMute(ACTION_DATA(mute));

			this->output("Set track mute: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + juce::String{ ACTION_DATA(mute) ? "ON" : "OFF" } + "\n");;
			ACTION_RESULT(true);
		}
	}
	this->output("Can't set track mute: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + juce::String{ ACTION_DATA(mute) ? "ON" : "OFF" } + "\n");
	ACTION_RESULT(false);
}

bool ActionSetTrackMute::undoAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTrackMute);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			track->setMute(ACTION_DATA(oldMute));

			this->output("Undo set track mute: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + juce::String{ ACTION_DATA(mute) ? "ON" : "OFF" } + "\n");;
			ACTION_RESULT(true);
		}
	}
	this->output("Can't undo set track mute: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + juce::String{ ACTION_DATA(mute) ? "ON" : "OFF" } + "\n");
	ACTION_RESULT(false);
}

ActionSetTrackSolo::ActionSetTrackSolo(
	quickAPI::TrackIndex track, bool solo)
	: ACTION_DB{ track, solo } {
}

bool ActionSetTrackSolo::doAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetTrackSolo);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			ACTION_DATA(oldSolo) = track->getSolo();
			track->setSolo(ACTION_DATA(solo));

			this->output("Set track solo: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + juce::String{ ACTION_DATA(solo) ? "ON" : "OFF" } + "\n");;
			ACTION_RESULT(true);
		}
	}
	this->output("Can't set track solo: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + juce::String{ ACTION_DATA(solo) ? "ON" : "OFF" } + "\n");
	ACTION_RESULT(false);
}

bool ActionSetTrackSolo::undoAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTrackSolo);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			track->setSolo(ACTION_DATA(oldSolo));

			this->output("Undo set track solo: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + juce::String{ ACTION_DATA(solo) ? "ON" : "OFF" } + "\n");;
			ACTION_RESULT(true);
		}
	}
	this->output("Can't undo set track solo: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + "] " + juce::String{ ACTION_DATA(solo) ? "ON" : "OFF" } + "\n");
	ACTION_RESULT(false);
}

ActionSetTrackRecording::ActionSetTrackRecording(
	int track, quickAPI::RecordState recordState)
	: ACTION_DB{ track, recordState } {
}

bool ActionSetTrackRecording::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetTrackRecording);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(track))) {
			if (auto seq = track->getSequencer()) {
				ACTION_DATA(oldRecordState) = seq->getRecording();

				seq->setRecording(ACTION_DATA(recordState));

				this->output("Set Track Recording: [" + juce::String{ ACTION_DATA(track) } + "] " + juce::String{ seq->getRecording() } + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetTrackRecording::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTrackRecording);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(track))) {
			if (auto seq = track->getSequencer()) {
				seq->setRecording(ACTION_DATA(oldRecordState));

				this->output("Undo Set Track Recording: [" + juce::String{ ACTION_DATA(track) } + "] " + juce::String{ seq->getRecording() } + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetTrackInputMonitoring::ActionSetTrackInputMonitoring(
	int track, bool inputMonitoring)
	: ACTION_DB{ track, inputMonitoring } {
}

bool ActionSetTrackInputMonitoring::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetTrackInputMonitoring);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(track))) {
			if (auto seq = track->getSequencer()) {
				ACTION_DATA(oldInputMonitoring) = seq->getInputMonitoring();

				seq->setInputMonitoring(ACTION_DATA(inputMonitoring));

				this->output("Set Track Input Monitoring: [" + juce::String{ ACTION_DATA(track) } + "] " + juce::String{ seq->getInputMonitoring() ? "ON" : "OFF" } + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionSetTrackInputMonitoring::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTrackInputMonitoring);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(track))) {
			if (auto seq = track->getSequencer()) {
				seq->setInputMonitoring(ACTION_DATA(oldInputMonitoring));

				this->output("Undo Set Track Input Monitoring: [" + juce::String{ ACTION_DATA(track) } + "] " + juce::String{ seq->getInputMonitoring() ? "ON" : "OFF" } + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetEffect::ActionSetEffect(
	quickAPI::TrackIndex track, int effect, const juce::String& pid)
	: ACTION_DB{ track, effect, pid } {
}

bool ActionSetEffect::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change effect while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetEffect);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(pid);

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto des = Plugin::getInstance()->findPlugin(ACTION_DATA(pid), false)) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
				if (auto mixer = track->getMixer()) {
					if (auto pluginDock = mixer->getPluginDock()) {
						/** Check Effect */
						if (ACTION_DATA(effect) < 0 || ACTION_DATA(effect) >= pluginDock->getSlotNum()) { ACTION_RESULT(false); }

						/** Save Effect State */
						auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect));
						if (!effect) { ACTION_RESULT(false); }
						auto state = effect->serialize(Serializable::createSerializeConfigQuickly());

						auto statePtr = dynamic_cast<vsp4::Plugin*>(state.get());
						if (!statePtr) { ACTION_RESULT(false); }
						statePtr->set_bypassed(pluginDock->getPluginBypass(ACTION_DATA(effect)));

						ACTION_DATA(data).setSize(state->ByteSizeLong());
						state->SerializeToArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize());

						/** Remove Effect */
						pluginDock->removePlugin(ACTION_DATA(effect));

						/** Add */
						if (auto ptr = pluginDock->insertPlugin(ACTION_DATA(effect))) {
							PluginLoader::getInstance()->loadPlugin(*(des.get()), false, ptr);

							this->output("Set Plugin: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + ", " + juce::String{ ACTION_DATA(effect) } + "] " + ACTION_DATA(pid) + "\n");
							ACTION_RESULT(true);
						}
					}
				}
			}
		}
	}

	this->error("Can't Set Plugin: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + ", " + juce::String{ ACTION_DATA(effect) } + "] " + ACTION_DATA(pid) + "\n");
	ACTION_RESULT(false);
}

bool ActionSetEffect::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change effect while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetEffect);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(pid);

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					/** Check Effect */
					if (ACTION_DATA(effect) < 0 || ACTION_DATA(effect) >= pluginDock->getSlotNum()) { ACTION_RESULT(false); }

					/** Remove Effect */
					pluginDock->removePlugin(ACTION_DATA(effect));

					/** Prepare Effect State */
					auto state = std::make_unique<vsp4::Plugin>();
					if (!state->ParseFromArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize())) {
						ACTION_RESULT(false);
					}

					/** Add Effect */
					pluginDock->insertPlugin(ACTION_DATA(effect));

					/** Recover Effect State */
					auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect));
					pluginDock->setPluginBypass(ACTION_DATA(effect), state->bypassed());
					effect->parse(state.get(), Serializable::createParseConfigQuickly());

					this->output("Undo Set Plugin: [" + juce::String{ (int)(ACTION_DATA(track).first) } + ", " + juce::String{ ACTION_DATA(track).second } + ", " + juce::String{ ACTION_DATA(effect) } + "]" + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionSetCurrentMIDITrack::ActionSetCurrentMIDITrack(
	int track, int midiTrack)
	: ACTION_DB{ track, midiTrack } {
}

bool ActionSetCurrentMIDITrack::doAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetCurrentMIDITrack);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(track))) {
			if (auto seq = track->getSequencer()) {
				ACTION_DATA(oldMIDITrack) = seq->getCurrentMIDITrack();
				seq->setCurrentMIDITrack(ACTION_DATA(midiTrack));

				this->output("Set current MIDI track: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String{ ACTION_DATA(midiTrack) } + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	this->output("Can't set current MIDI track: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String{ ACTION_DATA(midiTrack) } + "\n");
	ACTION_RESULT(false);
}

bool ActionSetCurrentMIDITrack::undoAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetCurrentMIDITrack);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(track))) {
			if (auto seq = track->getSequencer()) {
				seq->setCurrentMIDITrack(ACTION_DATA(oldMIDITrack));

				this->output("Undo set current MIDI track: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String{ ACTION_DATA(midiTrack) } + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	this->output("Can't undo set current MIDI track: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String{ ACTION_DATA(midiTrack) } + "\n");
	ACTION_RESULT(false);
}

ActionSetSequencerBlockTime::ActionSetSequencerBlockTime(
	int track, int index, const BlockTime& time)
	: ACTION_DB{ track, index, time } {
}

bool ActionSetSequencerBlockTime::doAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSetSequencerBlockTime);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(track))) {
			if (auto seq = track->getSequencer()) {
				ACTION_DATA(oldTime) = seq->getSeq(ACTION_DATA(index));
				ACTION_DATA(newIndex) = seq->resetSeqTime(ACTION_DATA(index), ACTION_DATA(time));
				if (ACTION_DATA(newIndex) >= 0) {
					this->output("Set seq block time: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String{ ACTION_DATA(index) } + "\n");
					ACTION_RESULT(true);
				}
			}
			
		}
	}
	this->output("Can't set seq block time: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String{ ACTION_DATA(index) } + "\n");
	ACTION_RESULT(false);
}

bool ActionSetSequencerBlockTime::undoAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetSequencerBlockTime);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(track))) {
			if (auto seq = track->getSequencer()) {
				seq->resetSeqTime(ACTION_DATA(newIndex), ACTION_DATA(oldTime));

				this->output("Undo set seq block time: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String{ ACTION_DATA(index) } + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	this->output("Can't undo set seq block time: [" + juce::String(ACTION_DATA(track)) + "] " + juce::String{ ACTION_DATA(index) } + "\n");
	ACTION_RESULT(false);
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

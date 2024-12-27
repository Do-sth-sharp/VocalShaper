#include "ActionSet.h"

#include "../AudioCore.h"
#include "../misc/Device.h"
#include "../misc/PlayPosition.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

ActionSetDeviceAudioType::ActionSetDeviceAudioType(const juce::String& type)
	: type(type) {}

bool ActionSetDeviceAudioType::doAction() {
	Device::getInstance()->setCurrentAudioDeviceType(this->type);
	return true;
}

const juce::String ActionSetDeviceAudioType::getStatusStr() const {
	return this->type;
}

ActionSetDeviceAudioInput::ActionSetDeviceAudioInput(const juce::String& name)
	: name(name) {}

bool ActionSetDeviceAudioInput::doAction() {
	auto err = Device::getInstance()->setAudioInputDevice(this->name);
	if (err.isNotEmpty()) {
		this->output(err);
	}
	return true;
}

const juce::String ActionSetDeviceAudioInput::getStatusStr() const {
	return this->name;
}

ActionSetDeviceAudioOutput::ActionSetDeviceAudioOutput(const juce::String& name)
	: name(name) {}

bool ActionSetDeviceAudioOutput::doAction() {
	auto err = Device::getInstance()->setAudioOutputDevice(this->name);
	if (err.isNotEmpty()) {
		this->output(err);
	}
	return true;
}

const juce::String ActionSetDeviceAudioOutput::getStatusStr() const {
	return this->name;
}

ActionSetDeviceAudioSampleRate::ActionSetDeviceAudioSampleRate(double sampleRate)
	: sampleRate(sampleRate) {}

bool ActionSetDeviceAudioSampleRate::doAction() {
	auto err = Device::getInstance()->setAudioSampleRate(this->sampleRate);
	if (err.isNotEmpty()) {
		this->output(err);
	}
	return true;
}

const juce::String ActionSetDeviceAudioSampleRate::getStatusStr() const {
	return juce::String{ this->sampleRate, 2 };
}

ActionSetDeviceAudioBufferSize::ActionSetDeviceAudioBufferSize(int bufferSize)
	: bufferSize(bufferSize) {}

bool ActionSetDeviceAudioBufferSize::doAction() {
	auto err = Device::getInstance()->setAudioBufferSize(this->bufferSize);
	if (err.isNotEmpty()) {
		this->output(err);
	}
	return true;
}

const juce::String ActionSetDeviceAudioBufferSize::getStatusStr() const {
	return juce::String{ this->bufferSize };
}

ActionSetDeviceMidiInput::ActionSetDeviceMidiInput(
	const juce::String& name, bool enabled)
	: name(name), enabled(enabled) {}

bool ActionSetDeviceMidiInput::doAction() {
	Device::getInstance()->setMIDIInputDeviceEnabled(
		this->name,	this->enabled);
	return true;
}

const juce::String ActionSetDeviceMidiInput::getStatusStr() const {
	return this->name + " - " + juce::String{ this->enabled ? "ON" : "OFF" };
}

ActionSetDeviceMidiOutput::ActionSetDeviceMidiOutput(const juce::String& name)
	: name(name) {}

bool ActionSetDeviceMidiOutput::doAction() {
	Device::getInstance()->setMIDIOutputDevice(this->name);
	return true;
}

const juce::String ActionSetDeviceMidiOutput::getStatusStr() const {
	return this->name;
}

ActionSetMidiDebuggerMaxNum::ActionSetMidiDebuggerMaxNum(int num)
	: num(num) {}

bool ActionSetMidiDebuggerMaxNum::doAction() {
	AudioCore::getInstance()->setMIDIDebuggerMaxNum(this->num);
	return true;
}

const juce::String ActionSetMidiDebuggerMaxNum::getStatusStr() const {
	return juce::String{ this->num };
}

ActionSetTrackGain::ActionSetTrackGain(
	quickAPI::TrackIndex track, float value)
	: track(track), value(value) {}

bool ActionSetTrackGain::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				this->oldValue = mixer->getGain();

				mixer->setGain(this->value);
				return true;
			}
		}
	}
	return false;
}

bool ActionSetTrackGain::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				mixer->setGain(this->oldValue);
				return true;
			}
			
		}
	}
	return false;
}

const juce::String ActionSetTrackGain::getStatusStr() const {
	return juce::String{ this->value, 2 };
}

void ActionSetTrackGain::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeFloat(this->value);
	stream.writeFloat(this->oldValue);
}

juce::UndoableAction* ActionSetTrackGain::createCoalescedAction(
	juce::UndoableAction* nextAction) {
	if (auto action = dynamic_cast<ActionSetTrackGain*>(nextAction)) {
		if (this->track == action->track) {
			auto newAction = std::make_unique<ActionSetTrackGain>(
				action->track, action->value);
			newAction->oldValue = this->oldValue;
			return newAction.release();
		}
	}
	return nullptr;
}

ActionSetTrackPan::ActionSetTrackPan(
	quickAPI::TrackIndex track, float value)
	: track(track), value(value) {}

bool ActionSetTrackPan::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				this->oldValue = mixer->getPan();

				mixer->setPan(this->value);
				return true;
			}
		}
	}
	return false;
}

bool ActionSetTrackPan::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				mixer->setPan(this->oldValue);
				return true;
			}
		}
	}
	return false;
}

const juce::String ActionSetTrackPan::getStatusStr() const {
	return juce::String{ this->value, 2 };
}

void ActionSetTrackPan::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeFloat(this->value);
	stream.writeFloat(this->oldValue);
}

juce::UndoableAction* ActionSetTrackPan::createCoalescedAction(
	juce::UndoableAction* nextAction) {
	if (auto action = dynamic_cast<ActionSetTrackPan*>(nextAction)) {
		if (this->track == action->track) {
			auto newAction = std::make_unique<ActionSetTrackPan>(
				action->track, action->value);
			newAction->oldValue = this->oldValue;
			return newAction.release();
		}
	}
	return nullptr;
}

ActionSetTrackFader::ActionSetTrackFader(
	quickAPI::TrackIndex track, float value)
	: track(track), value(value) {}

bool ActionSetTrackFader::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				this->oldValue = mixer->getFader();

				mixer->setFader(this->value);
				return true;
			}
		}
	}
	return false;
}

bool ActionSetTrackFader::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				mixer->setFader(this->oldValue);
				return true;
			}
		}
	}
	return false;
}

const juce::String ActionSetTrackFader::getStatusStr() const {
	return juce::String{ this->value, 2 };
}

void ActionSetTrackFader::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeFloat(this->value);
	stream.writeFloat(this->oldValue);
}

juce::UndoableAction* ActionSetTrackFader::createCoalescedAction(
	juce::UndoableAction* nextAction) {
	if (auto action = dynamic_cast<ActionSetTrackFader*>(nextAction)) {
		if (this->track == action->track) {
			auto newAction = std::make_unique<ActionSetTrackFader>(
				action->track, action->value);
			newAction->oldValue = this->oldValue;
			return newAction.release();
		}
	}
	return nullptr;
}

ActionSetEffectBypass::ActionSetEffectBypass(
	quickAPI::TrackIndex track, int effect, bool bypass)
	: track(track), effect(effect), bypass(bypass) {}

bool ActionSetEffectBypass::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					this->oldBypass = pluginDock->getPluginBypass(this->effect);

					pluginDock->setPluginBypass(this->effect, this->bypass);

					return true;
				}
			}
		}
	}
	return false;
}

bool ActionSetEffectBypass::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					pluginDock->setPluginBypass(this->effect, this->oldBypass);

					return true;
				}
			}
		}
	}
	return false;
}

const juce::String ActionSetEffectBypass::getStatusStr() const {
	return "[" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + ", " + juce::String{ this->effect } + "] " + juce::String{ this->bypass ? "ON" : "OFF" };
}

void ActionSetEffectBypass::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeInt(this->effect);
	stream.writeBool(this->bypass);
	stream.writeBool(this->oldBypass);
}

ActionSetInstrBypass::ActionSetInstrBypass(
	int instr, bool bypass)
	: instr(instr), bypass(bypass) {}

bool ActionSetInstrBypass::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				this->oldBypass = seq->getInstrumentBypass();

				seq->setInstrumentBypass(this->bypass);

				return true;
			}
		}
	}
	return false;
}

bool ActionSetInstrBypass::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				seq->setInstrumentBypass(this->oldBypass);

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionSetInstrBypass::getStatusStr() const {
	return "[" + juce::String{ this->instr } + "] " + juce::String{ this->bypass ? "ON" : "OFF" };
}

void ActionSetInstrBypass::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->instr);
	stream.writeBool(this->bypass);
	stream.writeBool(this->oldBypass);
}

ActionSetInstrMidiChannel::ActionSetInstrMidiChannel(
	int instr, int channel)
	: instr(instr), channel(channel) {
}

bool ActionSetInstrMidiChannel::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					this->oldChannel = instr->getMIDIChannel();

					instr->setMIDIChannel(this->channel);

					return true;
				}
			}
		}
	}
	return false;
}

bool ActionSetInstrMidiChannel::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					instr->setMIDIChannel(this->oldChannel);

					return true;
				}
			}
		}
	}
	return false;
}

const juce::String ActionSetInstrMidiChannel::getStatusStr() const {
	return "[" + juce::String{ this->instr } + "] " + juce::String{ this->channel };
}

void ActionSetInstrMidiChannel::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->instr);
	stream.writeInt(this->channel);
	stream.writeInt(this->oldChannel);
}

ActionSetEffectMidiChannel::ActionSetEffectMidiChannel(
	quickAPI::TrackIndex track, int effect, int channel)
	: track(track), effect(effect), channel(channel) {
}

bool ActionSetEffectMidiChannel::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
						this->oldChannel = effect->getMIDIChannel();

						effect->setMIDIChannel(this->channel);

						return true;
					}
				}
			}
		}
	}
	return false;
}

bool ActionSetEffectMidiChannel::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
						effect->setMIDIChannel(this->oldChannel);

						return true;
					}
				}
			}
		}
	}
	return false;
}

const juce::String ActionSetEffectMidiChannel::getStatusStr() const {
	return "[" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + ", " + juce::String{ this->effect } + "] " + juce::String{ this->channel };
}

void ActionSetEffectMidiChannel::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeInt(this->effect);
	stream.writeInt(this->channel);
	stream.writeInt(this->oldChannel);
}

ActionSetInstrParamValue::ActionSetInstrParamValue(
	int instr, int param, float value)
	: instr(instr), param(param), value(value) {
}

bool ActionSetInstrParamValue::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					this->oldValue = instr->getParamValue(this->param);

					instr->setParamValue(this->param, this->value);

					return true;
				}
			}
		}
	}
	return false;
}

bool ActionSetInstrParamValue::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					instr->setParamValue(this->param, this->oldValue);

					return true;
				}
			}
		}
	}
	return false;
}

const juce::String ActionSetInstrParamValue::getStatusStr() const {
	return "Undo Set Instr Param Value: [" + juce::String{ this->instr } + "] " + juce::String{ this->param } + " - " + juce::String{ this->value, 2 };
}

void ActionSetInstrParamValue::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->instr);
	stream.writeInt(this->param);
	stream.writeFloat(this->value);
	stream.writeFloat(this->oldValue);
}

ActionSetEffectParamValue::ActionSetEffectParamValue(
	quickAPI::TrackIndex track, int effect, int param, float value)
	: track(track), effect(effect), param(param), value(value) {}

bool ActionSetEffectParamValue::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
						this->oldValue = effect->getParamValue(this->param);

						effect->setParamValue(this->param, this->value);

						return true;
					}
				}
			}
		}
	}
	return false;
}

bool ActionSetEffectParamValue::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
						effect->setParamValue(this->param, this->oldValue);

						return true;
					}
				}
			}
		}
	}
	return false;
}

const juce::String ActionSetEffectParamValue::getStatusStr() const {
	return "[" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + ", " + juce::String{ this->effect } + "] " + juce::String{this->param} + " - " + juce::String{this->value, 2};
}

void ActionSetEffectParamValue::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeInt(this->effect);
	stream.writeInt(this->param);
	stream.writeFloat(this->value);
	stream.writeFloat(this->oldValue);
}

ActionSetInstrParamConnectToCC::ActionSetInstrParamConnectToCC(
	int instr, int param, int cc)
	: instr(instr), param(param), cc(cc) {}

bool ActionSetInstrParamConnectToCC::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					this->oldCC = instr->getParamCCConnection(this->param);
					this->oldParam = instr->getCCParamConnection(this->cc);

					instr->connectParamCC(this->param, this->cc);

					return true;
				}
			}
		}
	}
	return false;
}

bool ActionSetInstrParamConnectToCC::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					instr->connectParamCC(this->oldParam, this->cc);
					if (this->oldCC > -1) {
						instr->connectParamCC(this->param, this->oldCC);
					}

					return true;
				}
			}
		}
	}
	return false;
}

const juce::String ActionSetInstrParamConnectToCC::getStatusStr() const {
	return "[" + juce::String{ this->instr } + "] " + juce::String{ this->param } + " - MIDI CC " + juce::String{ this->cc };
}

void ActionSetInstrParamConnectToCC::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->instr);
	stream.writeInt(this->param);
	stream.writeInt(this->cc);
	stream.writeInt(this->oldParam);
	stream.writeInt(this->oldCC);
}

ActionSetEffectParamConnectToCC::ActionSetEffectParamConnectToCC(
	quickAPI::TrackIndex track, int effect, int param, int cc)
	: track(track), effect(effect), param(param), cc(cc) {}

bool ActionSetEffectParamConnectToCC::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
						this->oldCC = effect->getParamCCConnection(this->param);
						this->oldParam = effect->getCCParamConnection(this->cc);

						effect->connectParamCC(this->param, this->cc);

						return true;
					}
				}
			}
		}
	}
	return false;
}

bool ActionSetEffectParamConnectToCC::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
						effect->connectParamCC(this->oldParam, this->cc);
						if (this->oldCC > -1) {
							effect->connectParamCC(this->param, this->oldCC);
						}

						return true;
					}
				}
			}
		}
	}
	return false;
}

const juce::String ActionSetEffectParamConnectToCC::getStatusStr() const {
	return "[" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + ", " + juce::String{ this->effect } + "] " + juce::String{ this->param } + " - MIDI CC " + juce::String{ this->cc };
}

void ActionSetEffectParamConnectToCC::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeInt(this->effect);
	stream.writeInt(this->param);
	stream.writeInt(this->cc);
	stream.writeInt(this->oldParam);
	stream.writeInt(this->oldCC);
}

ActionSetInstrMidiCCIntercept::ActionSetInstrMidiCCIntercept(
	int instr, bool intercept)
	: instr(instr), intercept(intercept) {}

bool ActionSetInstrMidiCCIntercept::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					this->oldIntercept = instr->getMIDICCIntercept();

					instr->setMIDICCIntercept(this->intercept);

					return true;
				}
			}
		}
	}
	return false;
}

bool ActionSetInstrMidiCCIntercept::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					instr->setMIDICCIntercept(this->oldIntercept);

					return true;
				}
			}
		}
	}
	return false;
}

const juce::String ActionSetInstrMidiCCIntercept::getStatusStr() const {
	return "[" + juce::String{ this->instr } + "] " + juce::String{ this->intercept ? "ON" : "OFF" };
}

void ActionSetInstrMidiCCIntercept::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->instr);
	stream.writeBool(this->intercept);
	stream.writeBool(this->oldIntercept);
}

ActionSetEffectMidiCCIntercept::ActionSetEffectMidiCCIntercept(
	quickAPI::TrackIndex track, int effect, bool intercept)
	: track(track), effect(effect), intercept(intercept) {}

bool ActionSetEffectMidiCCIntercept::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
						this->oldIntercept = effect->getMIDICCIntercept();

						effect->setMIDICCIntercept(this->intercept);

						return true;
					}
				}
			}
		}
	}
	return false;
}

bool ActionSetEffectMidiCCIntercept::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
						effect->setMIDICCIntercept(this->oldIntercept);

						return true;
					}
				}
			}
		}
	}
	return false;
}

const juce::String ActionSetEffectMidiCCIntercept::getStatusStr() const {
	return "[" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + ", " + juce::String{ this->effect } + "] " + juce::String{ this->intercept ? "ON" : "OFF" };
}

void ActionSetEffectMidiCCIntercept::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeInt(this->effect);
	stream.writeBool(this->intercept);
	stream.writeBool(this->oldIntercept);
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

	if (this->effect) {
		this->oldBypass = PluginDock::getPluginBypass(this->effect);

		PluginDock::setPluginBypass(this->effect, this->bypass);

		this->output("Plugin Bypass: [" + this->effect->getName() + "] " + juce::String(this->bypass ? "ON" : "OFF") + "\n");
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

	if (this->effect) {
		PluginDock::setPluginBypass(this->effect, this->oldBypass);

		this->output("Undo Plugin Bypass: [" + this->effect->getName() + "] " + juce::String(this->bypass ? "ON" : "OFF") + "\n");
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

	if (this->instr) {
		this->oldBypass = SeqSourceProcessor::getInstrumentBypass(this->instr);

		SeqSourceProcessor::setInstrumentBypass(this->instr, this->bypass);

		this->output("Plugin Bypass: [" + this->instr->getName() + "] " + juce::String(this->bypass ? "ON" : "OFF") + "\n");
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

	if (this->instr) {
		SeqSourceProcessor::setInstrumentBypass(this->instr, this->oldBypass);

		this->output("Undo Plugin Bypass: [" + this->instr->getName() + "] " + juce::String(this->bypass ? "ON" : "OFF") + "\n");
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

	if (this->instr) {
		this->oldChannel = this->instr->getMIDIChannel();

		this->instr->setMIDIChannel(this->channel);

		this->output("Plugin MIDI Channel: [" + this->instr->getName() + "] " + juce::String(this->instr->getMIDIChannel()) + "\n");
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

	if (this->instr) {
		this->instr->setMIDIChannel(this->oldChannel);

		this->output("Undo Plugin MIDI Channel: [" + this->instr->getName() + "] " + juce::String(this->instr->getMIDIChannel()) + "\n");
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

	if (this->effect) {
		this->oldChannel = this->effect->getMIDIChannel();

		this->effect->setMIDIChannel(this->channel);

		this->output("Plugin MIDI Channel: [" + this->effect->getName() + "] " + juce::String(this->effect->getMIDIChannel()) + "\n");
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

	if (this->effect) {
		this->effect->setMIDIChannel(this->oldChannel);

		this->output("Undo Plugin MIDI Channel: [" + this->effect->getName() + "] " + juce::String(this->effect->getMIDIChannel()) + "\n");
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

	if (this->instr) {
		this->oldIntercept = this->instr->getMIDICCIntercept();

		this->instr->setMIDICCIntercept(this->intercept);

		this->output("Set Instr MIDI CC Intercept: [" + this->instr->getName() + "] " + juce::String(this->instr->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
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

	if (this->instr) {
		this->instr->setMIDICCIntercept(this->oldIntercept);

		this->output("Undo Set Instr MIDI CC Intercept: [" + this->instr->getName() + "] " + juce::String(this->instr->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
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

	if (this->effect) {
		this->oldIntercept = this->effect->getMIDICCIntercept();

		this->effect->setMIDICCIntercept(this->intercept);

		this->output("Set Effect MIDI CC Intercept: [" + this->effect->getName() + "] " + juce::String(this->effect->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
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

	if (this->effect) {
		this->effect->setMIDICCIntercept(this->oldIntercept);

		this->output("Undo Set Effect MIDI CC Intercept: [" + this->effect->getName() + "] " + juce::String(this->effect->getMIDICCIntercept() ? "ON" : "OFF") + "\n");
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

	if (this->instr) {
		this->oldOutput = this->instr->getMIDIOutput();

		this->instr->setMIDIOutput(this->output);

		this->output("Set Instr MIDI Output: [" + this->instr->getName() + "] " + juce::String(this->instr->getMIDIOutput() ? "ON" : "OFF") + "\n");
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

	if (this->instr) {
		this->instr->setMIDIOutput(this->oldOutput);

		this->output("Undo Set Instr MIDI Output: [" + this->instr->getName() + "] " + juce::String(this->instr->getMIDIOutput() ? "ON" : "OFF") + "\n");
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

	if (this->effect) {
		this->oldOutput = this->effect->getMIDIOutput();

		this->effect->setMIDIOutput(this->output);

		this->output("Set Effect MIDI Output: [" + this->effect->getName() + "] " + juce::String(this->effect->getMIDIOutput() ? "ON" : "OFF") + "\n");
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

	if (this->effect) {
		this->effect->setMIDIOutput(this->oldOutput);

		this->output("Set Effect MIDI Output: [" + this->effect->getName() + "] " + juce::String(this->effect->getMIDIOutput() ? "ON" : "OFF") + "\n");
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

	if (this->instr) {
		this->oldCC = this->instr->getParamCCConnection(this->param);
		this->oldParam = this->instr->getCCParamConnection(this->cc);

		this->instr->connectParamCC(this->param, this->cc);

		this->output("Connect Instr Param To MIDI CC: [" + juce::String(this->param) + "] " + this->instr->getParamName(this->param) + " - MIDI CC " + juce::String(this->instr->getParamCCConnection(this->param)) + "\n");
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

	if (this->instr) {
		this->instr->connectParamCC(this->oldParam, this->cc);
		if (this->oldCC > -1) {
			this->instr->connectParamCC(this->param, this->oldCC);
		}

		this->output("Undo Connect Instr Param To MIDI CC: [" + juce::String(this->param) + "] " + this->instr->getParamName(this->param) + " - MIDI CC " + juce::String(this->instr->getParamCCConnection(this->param)) + "\n");
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

	if (this->effect) {
		this->oldCC = this->effect->getParamCCConnection(this->param);
		this->oldParam = this->effect->getCCParamConnection(this->cc);

		this->effect->connectParamCC(this->param, this->cc);

		this->output("Connect Effect Param To MIDI CC: [" + juce::String(this->param) + "] " + this->effect->getParamName(this->param) + " - MIDI CC " + juce::String(this->effect->getParamCCConnection(this->param)) + "\n");
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

	if (this->effect) {
		this->effect->connectParamCC(this->oldParam, this->cc);
		if (this->oldCC > -1) {
			this->effect->connectParamCC(this->param, this->oldCC);
		}

		this->output("Undo Connect Effect Param To MIDI CC: [" + juce::String(this->param) + "] " + this->effect->getParamName(this->param) + " - MIDI CC " + juce::String(this->effect->getParamCCConnection(this->param)) + "\n");
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
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {

			this->oldName = track->getTrackName();
			track->setTrackName(this->name);

			this->output("Set track name: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + this->name + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't set track name: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + this->name + "\n");
	ACTION_RESULT(false);
}

bool ActionSetTrackName::undoAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTrackName);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(name);
	ACTION_WRITE_STRING(oldName);

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			track->setTrackName(this->oldName);

			this->output("Undo set track name: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + this->name + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't undo set track name: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + this->name + "\n");
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
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			this->oldColor = track->getTrackColor();
			track->setTrackColor(this->color);

			this->output("Set track color: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + this->color.toDisplayString(false) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't set track color: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + this->color.toDisplayString(false) + "\n");
	ACTION_RESULT(false);
}

bool ActionSetTrackColor::undoAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTrackColor);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			track->setTrackColor(this->oldColor);

			this->output("Undo set track color: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + this->color.toDisplayString(false) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't undo set track color: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + this->color.toDisplayString(false) + "\n");
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
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (pluginDock->setPluginIndex(this->oldIndex, this->newIndex)) {
						this->output("Set Effect Index: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + ", " + juce::String{ this->oldIndex } + "] " + juce::String{ this->newIndex } + "\n");
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
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (pluginDock->setPluginIndex(this->newIndex, this->oldIndex)) {
						this->output("Undo Set Effect Index: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + ", " + juce::String{ this->oldIndex } + "] " + juce::String{ this->newIndex } + "\n");
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
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				this->oldOffline = seq->getInstrOffline();

				seq->setInstrOffline(this->offline);

				this->output("Instr Offline: [" + juce::String{ this->instr } + "] " + juce::String{ seq->getInstrOffline() ? "ON" : "OFF" } + "\n");
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
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				seq->setInstrOffline(this->oldOffline);

				this->output("Undo Instr Offline: [" + juce::String{ this->instr } + "] " + juce::String{ seq->getInstrOffline() ? "ON" : "OFF" } + "\n");
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

	if (this->index >= 0 && this->index < PlayPosition::getInstance()->getTempoLabelNum()) {
		this->oldTime = PlayPosition::getInstance()->getTempoLabelTime(this->index);

		this->newIndex = PlayPosition::getInstance()->setTempoLabelTime(
			this->index, this->time);

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
		this->newIndex, this->oldTime, this->index);

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

	if (this->index >= 0 && this->index < PlayPosition::getInstance()->getTempoLabelNum()) {
		this->oldTempo = PlayPosition::getInstance()->getTempoLabelTempo(this->index);

		PlayPosition::getInstance()->setTempoLabelTempo(this->index, this->tempo);

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

	PlayPosition::getInstance()->setTempoLabelTempo(this->index, this->oldTempo);

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

	if (this->index >= 0 && this->index < PlayPosition::getInstance()->getTempoLabelNum()) {
		std::tie(this->oldNumerator, this->oldDenominator) = PlayPosition::getInstance()->getTempoLabelBeat(this->index);

		PlayPosition::getInstance()->setTempoLabelBeat(this->index, this->numerator, this->denominator);

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

	PlayPosition::getInstance()->setTempoLabelBeat(this->index, this->numerator, this->denominator);

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
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			this->oldMute = track->getMute();
			track->setMute(this->mute);

			this->output("Set track mute: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + juce::String{ this->mute ? "ON" : "OFF" } + "\n");;
			ACTION_RESULT(true);
		}
	}
	this->output("Can't set track mute: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + juce::String{ this->mute ? "ON" : "OFF" } + "\n");
	ACTION_RESULT(false);
}

bool ActionSetTrackMute::undoAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTrackMute);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			track->setMute(this->oldMute);

			this->output("Undo set track mute: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + juce::String{ this->mute ? "ON" : "OFF" } + "\n");;
			ACTION_RESULT(true);
		}
	}
	this->output("Can't undo set track mute: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + juce::String{ this->mute ? "ON" : "OFF" } + "\n");
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
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			this->oldSolo = track->getSolo();
			track->setSolo(this->solo);

			this->output("Set track solo: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + juce::String{ this->solo ? "ON" : "OFF" } + "\n");;
			ACTION_RESULT(true);
		}
	}
	this->output("Can't set track solo: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + juce::String{ this->solo ? "ON" : "OFF" } + "\n");
	ACTION_RESULT(false);
}

bool ActionSetTrackSolo::undoAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetTrackSolo);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			track->setSolo(this->oldSolo);

			this->output("Undo set track solo: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + juce::String{ this->solo ? "ON" : "OFF" } + "\n");;
			ACTION_RESULT(true);
		}
	}
	this->output("Can't undo set track solo: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + juce::String{ this->solo ? "ON" : "OFF" } + "\n");
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
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				this->oldRecordState = seq->getRecording();

				seq->setRecording(this->recordState);

				this->output("Set Track Recording: [" + juce::String{ this->track } + "] " + juce::String{ seq->getRecording() } + "\n");
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
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				seq->setRecording(this->oldRecordState);

				this->output("Undo Set Track Recording: [" + juce::String{ this->track } + "] " + juce::String{ seq->getRecording() } + "\n");
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
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				this->oldInputMonitoring = seq->getInputMonitoring();

				seq->setInputMonitoring(this->inputMonitoring);

				this->output("Set Track Input Monitoring: [" + juce::String{ this->track } + "] " + juce::String{ seq->getInputMonitoring() ? "ON" : "OFF" } + "\n");
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
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				seq->setInputMonitoring(this->oldInputMonitoring);

				this->output("Undo Set Track Input Monitoring: [" + juce::String{ this->track } + "] " + juce::String{ seq->getInputMonitoring() ? "ON" : "OFF" } + "\n");
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

	writeRecoverySizeValue(this->data.getSize());
	writeRecoveryDataBlockValue((const char*)(this->data.getData()), this->data.getSize());

	if (auto des = Plugin::getInstance()->findPlugin(this->pid, false)) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
				if (auto mixer = track->getMixer()) {
					if (auto pluginDock = mixer->getPluginDock()) {
						/** Check Effect */
						if (this->effect < 0 || this->effect >= pluginDock->getSlotNum()) { ACTION_RESULT(false); }

						/** Save Effect State */
						auto effect = pluginDock->getPluginProcessor(this->effect);
						if (!effect) { ACTION_RESULT(false); }
						auto state = effect->serialize(Serializable::createSerializeConfigQuickly());

						auto statePtr = dynamic_cast<vsp4::Plugin*>(state.get());
						if (!statePtr) { ACTION_RESULT(false); }
						statePtr->set_bypassed(pluginDock->getPluginBypass(this->effect));

						this->data.setSize(state->ByteSizeLong());
						state->SerializeToArray(this->data.getData(), this->data.getSize());

						/** Remove Effect */
						pluginDock->removePlugin(this->effect);

						/** Add */
						if (auto ptr = pluginDock->insertPlugin(this->effect)) {
							PluginLoader::getInstance()->loadPlugin(*(des.get()), false, ptr);

							this->output("Set Plugin: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + ", " + juce::String{ this->effect } + "] " + this->pid + "\n");
							ACTION_RESULT(true);
						}
					}
				}
			}
		}
	}

	this->error("Can't Set Plugin: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + ", " + juce::String{ this->effect } + "] " + this->pid + "\n");
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

	writeRecoverySizeValue(this->data.getSize());
	writeRecoveryDataBlockValue((const char*)(this->data.getData()), this->data.getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					/** Check Effect */
					if (this->effect < 0 || this->effect >= pluginDock->getSlotNum()) { ACTION_RESULT(false); }

					/** Remove Effect */
					pluginDock->removePlugin(this->effect);

					/** Prepare Effect State */
					auto state = std::make_unique<vsp4::Plugin>();
					if (!state->ParseFromArray(this->data.getData(), this->data.getSize())) {
						ACTION_RESULT(false);
					}

					/** Add Effect */
					pluginDock->insertPlugin(this->effect);

					/** Recover Effect State */
					auto effect = pluginDock->getPluginProcessor(this->effect);
					pluginDock->setPluginBypass(this->effect, state->bypassed());
					effect->parse(state.get(), Serializable::createParseConfigQuickly());

					this->output("Undo Set Plugin: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + ", " + juce::String{ this->effect } + "]" + "\n");
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
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				this->oldMIDITrack = seq->getCurrentMIDITrack();
				seq->setCurrentMIDITrack(this->midiTrack);

				this->output("Set current MIDI track: [" + juce::String(this->track) + "] " + juce::String{ this->midiTrack } + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	this->output("Can't set current MIDI track: [" + juce::String(this->track) + "] " + juce::String{ this->midiTrack } + "\n");
	ACTION_RESULT(false);
}

bool ActionSetCurrentMIDITrack::undoAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetCurrentMIDITrack);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				seq->setCurrentMIDITrack(this->oldMIDITrack);

				this->output("Undo set current MIDI track: [" + juce::String(this->track) + "] " + juce::String{ this->midiTrack } + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	this->output("Can't undo set current MIDI track: [" + juce::String(this->track) + "] " + juce::String{ this->midiTrack } + "\n");
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
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				this->oldTime = seq->getSeq(this->index);
				this->newIndex = seq->resetSeqTime(this->index, this->time);
				if (this->newIndex >= 0) {
					this->output("Set seq block time: [" + juce::String(this->track) + "] " + juce::String{ this->index } + "\n");
					ACTION_RESULT(true);
				}
			}
			
		}
	}
	this->output("Can't set seq block time: [" + juce::String(this->track) + "] " + juce::String{ this->index } + "\n");
	ACTION_RESULT(false);
}

bool ActionSetSequencerBlockTime::undoAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSetSequencerBlockTime);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				seq->resetSeqTime(this->newIndex, this->oldTime);

				this->output("Undo set seq block time: [" + juce::String(this->track) + "] " + juce::String{ this->index } + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	this->output("Can't undo set seq block time: [" + juce::String(this->track) + "] " + juce::String{ this->index } + "\n");
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

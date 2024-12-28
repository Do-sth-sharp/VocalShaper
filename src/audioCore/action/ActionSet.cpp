#include "ActionSet.h"

#include "../AudioCore.h"
#include "../plugin/Plugin.h"
#include "../plugin/PluginLoader.h"
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
	: effect(effect), bypass(bypass) {}

bool ActionSetEffectBypassByPtr::doAction() {
	if (this->effect) {
		this->oldBypass = PluginDock::getPluginBypass(this->effect);

		PluginDock::setPluginBypass(this->effect, this->bypass);
		return true;
	}
	return false;
}

bool ActionSetEffectBypassByPtr::undoAction() {
	if (this->effect) {
		PluginDock::setPluginBypass(this->effect, this->oldBypass);
		return true;
	}
	return false;
}

const juce::String ActionSetEffectBypassByPtr::getStatusStr() const {
	return "[" + juce::String{ this->effect ? this->effect->getName() : "" } + "] " + juce::String{ this->bypass ? "ON" : "OFF" };
}

void ActionSetEffectBypassByPtr::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeString(this->effect ? this->effect->getName() : "");
	stream.writeBool(this->bypass);
	stream.writeBool(this->oldBypass);
}

ActionSetInstrBypassByPtr::ActionSetInstrBypassByPtr(
	quickAPI::PluginHolder instr, bool bypass)
	: instr(instr), bypass(bypass) {}

bool ActionSetInstrBypassByPtr::doAction() {
	if (this->instr) {
		this->oldBypass = SeqSourceProcessor::getInstrumentBypass(this->instr);

		SeqSourceProcessor::setInstrumentBypass(this->instr, this->bypass);
		return true;
	}
	return false;
}

bool ActionSetInstrBypassByPtr::undoAction() {
	if (this->instr) {
		SeqSourceProcessor::setInstrumentBypass(this->instr, this->oldBypass);
		return true;
	}
	return false;
}

const juce::String ActionSetInstrBypassByPtr::getStatusStr() const {
	return "[" + juce::String{ this->instr ? this->instr->getName() : "" } + "] " + juce::String{ this->bypass ? "ON" : "OFF" };
}

void ActionSetInstrBypassByPtr::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeString(this->instr ? this->instr->getName() : "");
	stream.writeBool(this->bypass);
	stream.writeBool(this->oldBypass);
}

ActionSetInstrMidiChannelByPtr::ActionSetInstrMidiChannelByPtr(
	quickAPI::PluginHolder instr, int channel)
	: instr(instr), channel(channel) {}

bool ActionSetInstrMidiChannelByPtr::doAction() {
	if (this->instr) {
		this->oldChannel = this->instr->getMIDIChannel();

		this->instr->setMIDIChannel(this->channel);
		return true;
	}
	return false;
}

bool ActionSetInstrMidiChannelByPtr::undoAction() {
	if (this->instr) {
		this->instr->setMIDIChannel(this->oldChannel);
		return true;
	}
	return false;
}

const juce::String ActionSetInstrMidiChannelByPtr::getStatusStr() const {
	return "[" + juce::String{ this->instr ? this->instr->getName() : "" } + "] " + juce::String{ this->channel };
}

void ActionSetInstrMidiChannelByPtr::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeString(this->instr ? this->instr->getName() : "");
	stream.writeInt(this->channel);
	stream.writeInt(this->oldChannel);
}

ActionSetEffectMidiChannelByPtr::ActionSetEffectMidiChannelByPtr(
	quickAPI::PluginHolder effect, int channel)
	: effect(effect), channel(channel) {}

bool ActionSetEffectMidiChannelByPtr::doAction() {
	if (this->effect) {
		this->oldChannel = this->effect->getMIDIChannel();

		this->effect->setMIDIChannel(this->channel);
		return true;
	}
	return false;
}

bool ActionSetEffectMidiChannelByPtr::undoAction() {
	if (this->effect) {
		this->effect->setMIDIChannel(this->oldChannel);
		return true;
	}
	return false;
}

const juce::String ActionSetEffectMidiChannelByPtr::getStatusStr() const {
	return "[" + juce::String{ this->effect ? this->effect->getName() : "" } + "] " + juce::String{ this->channel };
}

void ActionSetEffectMidiChannelByPtr::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeString(this->effect ? this->effect->getName() : "");
	stream.writeInt(this->channel);
	stream.writeInt(this->oldChannel);
}

ActionSetInstrMidiCCInterceptByPtr::ActionSetInstrMidiCCInterceptByPtr(
	quickAPI::PluginHolder instr, bool intercept)
	: instr(instr), intercept(intercept) {
}

bool ActionSetInstrMidiCCInterceptByPtr::doAction() {
	if (this->instr) {
		this->oldIntercept = this->instr->getMIDICCIntercept();

		this->instr->setMIDICCIntercept(this->intercept);
		return true;
	}
	return false;
}

bool ActionSetInstrMidiCCInterceptByPtr::undoAction() {
	if (this->instr) {
		this->instr->setMIDICCIntercept(this->oldIntercept);
		return true;
	}
	return false;
}

const juce::String ActionSetInstrMidiCCInterceptByPtr::getStatusStr() const {
	return "[" + juce::String{ this->instr ? this->instr->getName() : "" } + "] " + juce::String{ this->intercept ? "ON" : "OFF" };
}

void ActionSetInstrMidiCCInterceptByPtr::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeString(this->instr ? this->instr->getName() : "");
	stream.writeBool(this->intercept);
	stream.writeBool(this->oldIntercept);
}

ActionSetEffectMidiCCInterceptByPtr::ActionSetEffectMidiCCInterceptByPtr(
	quickAPI::PluginHolder effect, bool intercept)
	: effect(effect), intercept(intercept) {}

bool ActionSetEffectMidiCCInterceptByPtr::doAction() {
	if (this->effect) {
		this->oldIntercept = this->effect->getMIDICCIntercept();

		this->effect->setMIDICCIntercept(this->intercept);
		return true;
	}
	return false;
}

bool ActionSetEffectMidiCCInterceptByPtr::undoAction() {
	if (this->effect) {
		this->effect->setMIDICCIntercept(this->oldIntercept);
		return true;
	}
	return false;
}

const juce::String ActionSetEffectMidiCCInterceptByPtr::getStatusStr() const {
	return "[" + juce::String{ this->effect ? this->effect->getName() : "" } + "] " + juce::String{ this->intercept ? "ON" : "OFF" };
}

void ActionSetEffectMidiCCInterceptByPtr::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeString(this->effect ? this->effect->getName() : "");
	stream.writeBool(this->intercept);
	stream.writeBool(this->oldIntercept);
}

ActionSetInstrMidiOutputByPtr::ActionSetInstrMidiOutputByPtr(
	quickAPI::PluginHolder instr, bool output)
	: instr(instr), output(output) {
}

bool ActionSetInstrMidiOutputByPtr::doAction() {
	if (this->instr) {
		this->oldOutput = this->instr->getMIDIOutput();

		this->instr->setMIDIOutput(this->output);
		return true;
	}
	return false;
}

bool ActionSetInstrMidiOutputByPtr::undoAction() {
	if (this->instr) {
		this->instr->setMIDIOutput(this->oldOutput);
		return true;
	}
	return false;
}

const juce::String ActionSetInstrMidiOutputByPtr::getStatusStr() const {
	return "[" + juce::String{ this->instr ? this->instr->getName() : "" } + "] " + juce::String{ this->output ? "ON" : "OFF" };
}

void ActionSetInstrMidiOutputByPtr::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeString(this->instr ? this->instr->getName() : "");
	stream.writeBool(this->output);
	stream.writeBool(this->oldOutput);
}

ActionSetEffectMidiOutputByPtr::ActionSetEffectMidiOutputByPtr(
	quickAPI::PluginHolder effect, bool output)
	: effect(effect), output(output) {}

bool ActionSetEffectMidiOutputByPtr::doAction() {
	if (this->effect) {
		this->oldOutput = this->effect->getMIDIOutput();

		this->effect->setMIDIOutput(this->output);
		return true;
	}
	return false;
}

bool ActionSetEffectMidiOutputByPtr::undoAction() {
	if (this->effect) {
		this->effect->setMIDIOutput(this->oldOutput);
		return true;
	}
	return false;
}

const juce::String ActionSetEffectMidiOutputByPtr::getStatusStr() const {
	return "[" + juce::String{ this->effect ? this->effect->getName() : "" } + "] " + juce::String{ this->output ? "ON" : "OFF" };
}

void ActionSetEffectMidiOutputByPtr::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeString(this->effect ? this->effect->getName() : "");
	stream.writeBool(this->output);
	stream.writeBool(this->oldOutput);
}

ActionSetInstrParamConnectToCCByPtr::ActionSetInstrParamConnectToCCByPtr(
	quickAPI::PluginHolder instr, int param, int cc)
	: instr(instr), param(param), cc(cc) {
}

bool ActionSetInstrParamConnectToCCByPtr::doAction() {
	if (this->instr) {
		this->oldCC = this->instr->getParamCCConnection(this->param);
		this->oldParam = this->instr->getCCParamConnection(this->cc);

		this->instr->connectParamCC(this->param, this->cc);
		return true;
	}
	return false;
}

bool ActionSetInstrParamConnectToCCByPtr::undoAction() {
	if (this->instr) {
		this->instr->connectParamCC(this->oldParam, this->cc);
		if (this->oldCC > -1) {
			this->instr->connectParamCC(this->param, this->oldCC);
		}
		return true;
	}
	return false;
}

const juce::String ActionSetInstrParamConnectToCCByPtr::getStatusStr() const {
	return "[" + juce::String{ this->instr ? this->instr->getName() : "" } + "] " + juce::String{ this->param } + " - MIDI CC " + juce::String{ this->cc };
}

void ActionSetInstrParamConnectToCCByPtr::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeString(this->instr ? this->instr->getName() : "");
	stream.writeInt(this->param);
	stream.writeInt(this->cc);
	stream.writeInt(this->oldParam);
	stream.writeInt(this->oldCC);
}

ActionSetEffectParamConnectToCCByPtr::ActionSetEffectParamConnectToCCByPtr(
	quickAPI::PluginHolder effect, int param, int cc)
	: effect(effect), param(param), cc(cc) {
}

bool ActionSetEffectParamConnectToCCByPtr::doAction() {
	if (this->effect) {
		this->oldCC = this->effect->getParamCCConnection(this->param);
		this->oldParam = this->effect->getCCParamConnection(this->cc);

		this->effect->connectParamCC(this->param, this->cc);
		return true;
	}
	return false;
}

bool ActionSetEffectParamConnectToCCByPtr::undoAction() {
	if (this->effect) {
		this->effect->connectParamCC(this->oldParam, this->cc);
		if (this->oldCC > -1) {
			this->effect->connectParamCC(this->param, this->oldCC);
		}
		return true;
	}
	return false;
}

const juce::String ActionSetEffectParamConnectToCCByPtr::getStatusStr() const {
	return "[" + juce::String{ this->effect ? this->effect->getName() : "" } + "] " + juce::String{ this->param } + " - MIDI CC " + juce::String{ this->cc };

}

void ActionSetEffectParamConnectToCCByPtr::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeString(this->effect ? this->effect->getName() : "");
	stream.writeInt(this->param);
	stream.writeInt(this->cc);
	stream.writeInt(this->oldParam);
	stream.writeInt(this->oldCC);
}

ActionSetTrackName::ActionSetTrackName(
	quickAPI::TrackIndex track, const juce::String& name)
	: track(track), name(name) {
}

bool ActionSetTrackName::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {

			this->oldName = track->getTrackName();
			track->setTrackName(this->name);

			return true;
		}
	}
	return false;
}

bool ActionSetTrackName::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			track->setTrackName(this->oldName);

			return true;
		}
	}
	return false;
}

const juce::String ActionSetTrackName::getStatusStr() const {
	return "[" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + this->name;
}

void ActionSetTrackName::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeString(this->name);
}

ActionSetTrackColor::ActionSetTrackColor(
	quickAPI::TrackIndex track, const juce::Colour& color)
	: track(track), color(color) {
}

bool ActionSetTrackColor::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			this->oldColor = track->getTrackColor();
			track->setTrackColor(this->color);

			return true;
		}
	}
	return false;
}

bool ActionSetTrackColor::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			track->setTrackColor(this->oldColor);

			return true;
		}
	}
	return false;
}

const juce::String ActionSetTrackColor::getStatusStr() const {
	return "[" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + this->color.toDisplayString(false);
}

void ActionSetTrackColor::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeString(this->color.toDisplayString(false));
}

ActionSetEffectIndex::ActionSetEffectIndex(
	quickAPI::TrackIndex track, int oldIndex, int newIndex)
	: track(track), oldIndex(oldIndex), newIndex(newIndex) {}

bool ActionSetEffectIndex::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (pluginDock->setPluginIndex(this->oldIndex, this->newIndex)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool ActionSetEffectIndex::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (pluginDock->setPluginIndex(this->newIndex, this->oldIndex)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

const juce::String ActionSetEffectIndex::getStatusStr() const {
	return "[" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + ", " + juce::String{ this->oldIndex } + "] " + juce::String{ this->newIndex };
}

void ActionSetEffectIndex::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeInt(this->oldIndex);
	stream.writeInt(this->newIndex);
}

ActionSetInstrOffline::ActionSetInstrOffline(
	int instr, bool offline)
	: instr(instr), offline(offline) {
}

bool ActionSetInstrOffline::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				this->oldOffline = seq->getInstrOffline();

				seq->setInstrOffline(this->offline);

				return true;
			}
		}
	}
	return false;
}

bool ActionSetInstrOffline::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				seq->setInstrOffline(this->oldOffline);

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionSetInstrOffline::getStatusStr() const {
	return "[" + juce::String{ this->instr } + "] " + juce::String{ this->offline ? "ON" : "OFF" };
}

void ActionSetInstrOffline::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->instr);
	stream.writeBool(this->offline);
	stream.writeBool(this->oldOffline);
}

ActionSetTempoTime::ActionSetTempoTime(
	int index, double time)
	: index(index), time(time) {
}

bool ActionSetTempoTime::doAction() {
	if (this->index >= 0 && this->index < PlayPosition::getInstance()->getTempoLabelNum()) {
		this->oldTime = PlayPosition::getInstance()->getTempoLabelTime(this->index);

		this->newIndex = PlayPosition::getInstance()->setTempoLabelTime(
			this->index, this->time);

		return true;
	}
	return false;
}

bool ActionSetTempoTime::undoAction() {
	PlayPosition::getInstance()->setTempoLabelTime(
		this->newIndex, this->oldTime, this->index);
	return true;
}

const juce::String ActionSetTempoTime::getStatusStr() const {
	return "[" + juce::String{ this->index } + "] " + juce::String{ this->time, 2 };
}

void ActionSetTempoTime::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->index);
	stream.writeDouble(this->time);
	stream.writeDouble(this->oldTime);
	stream.writeInt(this->newIndex);
}

ActionSetTempoTempo::ActionSetTempoTempo(
	int index, double tempo)
	: index(index), tempo(tempo) {
}

bool ActionSetTempoTempo::doAction() {
	if (this->index >= 0 && this->index < PlayPosition::getInstance()->getTempoLabelNum()) {
		this->oldTempo = PlayPosition::getInstance()->getTempoLabelTempo(this->index);

		PlayPosition::getInstance()->setTempoLabelTempo(this->index, this->tempo);

		return true;
	}
	return false;
}

bool ActionSetTempoTempo::undoAction() {
	PlayPosition::getInstance()->setTempoLabelTempo(this->index, this->oldTempo);
	return true;
}

const juce::String ActionSetTempoTempo::getStatusStr() const {
	return "[" + juce::String{ this->index } + "] " + juce::String{ this->tempo, 2 };
}

void ActionSetTempoTempo::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->index);
	stream.writeDouble(this->tempo);
	stream.writeDouble(this->oldTempo);
}

ActionSetTempoBeat::ActionSetTempoBeat(
	int index, int numerator, int denominator)
	: index(index), numerator(numerator), denominator(denominator) {}

bool ActionSetTempoBeat::doAction() {
	if (this->index >= 0 && this->index < PlayPosition::getInstance()->getTempoLabelNum()) {
		std::tie(this->oldNumerator, this->oldDenominator) = PlayPosition::getInstance()->getTempoLabelBeat(this->index);

		PlayPosition::getInstance()->setTempoLabelBeat(this->index, this->numerator, this->denominator);

		return true;
	}
	return false;
}

bool ActionSetTempoBeat::undoAction() {
	PlayPosition::getInstance()->setTempoLabelBeat(this->index, this->numerator, this->denominator);
	return true;
}

const juce::String ActionSetTempoBeat::getStatusStr() const {
	return "[" + juce::String{ this->index } + "] " + juce::String{ this->numerator } + " / " + juce::String{ this->denominator };
}

void ActionSetTempoBeat::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->index);
	stream.writeInt(this->numerator);
	stream.writeInt(this->denominator);
	stream.writeInt(this->oldNumerator);
	stream.writeInt(this->oldDenominator);
}

ActionSetTrackMute::ActionSetTrackMute(
	quickAPI::TrackIndex track, bool mute)
	: track(track), mute(mute) {}

bool ActionSetTrackMute::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			this->oldMute = track->getMute();
			track->setMute(this->mute);

			return true;
		}
	}
	return false;
}

bool ActionSetTrackMute::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			track->setMute(this->oldMute);

			return true;
		}
	}
	return false;
}

const juce::String ActionSetTrackMute::getStatusStr() const {
	return "[" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + juce::String{ this->mute ? "ON" : "OFF" };
}

void ActionSetTrackMute::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeBool(this->mute);
	stream.writeBool(this->oldMute);
}

ActionSetTrackSolo::ActionSetTrackSolo(
	quickAPI::TrackIndex track, bool solo)
	: track(track), solo(solo) {
}

bool ActionSetTrackSolo::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			this->oldSolo = track->getSolo();
			track->setSolo(this->solo);

			return true;
		}
	}
	return false;
}

bool ActionSetTrackSolo::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			track->setSolo(this->oldSolo);

			return true;
		}
	}
	return false;
}

const juce::String ActionSetTrackSolo::getStatusStr() const {
	return "[" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + "] " + juce::String{ this->solo ? "ON" : "OFF" };
}

void ActionSetTrackSolo::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeBool(this->solo);
	stream.writeBool(this->oldSolo);
}

ActionSetTrackRecording::ActionSetTrackRecording(
	int track, quickAPI::RecordState recordState)
	: track(track), recordState(recordState) {
}

bool ActionSetTrackRecording::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				this->oldRecordState = seq->getRecording();

				seq->setRecording(this->recordState);

				return true;
			}
		}
	}
	return false;
}

bool ActionSetTrackRecording::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				seq->setRecording(this->oldRecordState);

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionSetTrackRecording::getStatusStr() const {
	return "Undo Set Track Recording: [" + juce::String{ this->track } + "] " + juce::String{ (int)this->recordState };
}

void ActionSetTrackRecording::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->track);
	stream.writeInt((int)this->recordState);
	stream.writeInt((int)this->oldRecordState);
}

ActionSetTrackInputMonitoring::ActionSetTrackInputMonitoring(
	int track, bool inputMonitoring)
	: track(track), inputMonitoring(inputMonitoring) {}

bool ActionSetTrackInputMonitoring::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				this->oldInputMonitoring = seq->getInputMonitoring();

				seq->setInputMonitoring(this->inputMonitoring);

				return true;
			}
		}
	}
	return false;
}

bool ActionSetTrackInputMonitoring::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				seq->setInputMonitoring(this->oldInputMonitoring);

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionSetTrackInputMonitoring::getStatusStr() const {
	return "[" + juce::String{ this->track } + "] " + juce::String{ this->inputMonitoring ? "ON" : "OFF" };
}

void ActionSetTrackInputMonitoring::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->track);
	stream.writeBool(this->inputMonitoring);
	stream.writeBool(this->oldInputMonitoring);
}

ActionSetEffect::ActionSetEffect(
	quickAPI::TrackIndex track, int effect, const juce::String& pid)
	: track(track), effect(effect), pid(pid) {
}

bool ActionSetEffect::doAction() {
	if (auto des = Plugin::getInstance()->findPlugin(this->pid, false)) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
				if (auto mixer = track->getMixer()) {
					if (auto pluginDock = mixer->getPluginDock()) {
						/** Check Effect */
						if (this->effect < 0 || this->effect >= pluginDock->getSlotNum()) { return false; }

						/** Save Effect State */
						auto effect = pluginDock->getPluginProcessor(this->effect);
						if (!effect) { return false; }
						auto state = effect->serialize(Serializable::createSerializeConfigQuickly());

						auto statePtr = dynamic_cast<vsp4::Plugin*>(state.get());
						if (!statePtr) { return false; }
						statePtr->set_bypassed(pluginDock->getPluginBypass(this->effect));

						this->data.setSize(state->ByteSizeLong());
						state->SerializeToArray(this->data.getData(), this->data.getSize());

						/** Remove Effect */
						pluginDock->removePlugin(this->effect);

						/** Add */
						if (auto ptr = pluginDock->insertPlugin(this->effect)) {
							PluginLoader::getInstance()->loadPlugin(*(des.get()), false, ptr);

							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool ActionSetEffect::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					/** Check Effect */
					if (this->effect < 0 || this->effect >= pluginDock->getSlotNum()) { return false; }

					/** Remove Effect */
					pluginDock->removePlugin(this->effect);

					/** Prepare Effect State */
					auto state = std::make_unique<vsp4::Plugin>();
					if (!state->ParseFromArray(this->data.getData(), this->data.getSize())) {
						return false;
					}

					/** Add Effect */
					pluginDock->insertPlugin(this->effect);

					/** Recover Effect State */
					auto effect = pluginDock->getPluginProcessor(this->effect);
					pluginDock->setPluginBypass(this->effect, state->bypassed());
					effect->parse(state.get(), Serializable::createParseConfigQuickly());

					return true;
				}
			}
		}
	}
	return false;
}

const juce::String ActionSetEffect::getStatusStr() const {
	return "Undo Set Plugin: [" + juce::String{ (int)(this->track.first) } + ", " + juce::String{ this->track.second } + ", " + juce::String{ this->effect } + "] " + this->pid;
}

void ActionSetEffect::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeInt(this->effect);
	stream.writeString(this->pid);

	stream.writeInt64(this->data.getSize());
	stream.write(this->data.getData(), this->data.getSize());
}

ActionSetCurrentMIDITrack::ActionSetCurrentMIDITrack(
	int track, int midiTrack)
	: track(track), midiTrack(midiTrack) {
}

bool ActionSetCurrentMIDITrack::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				this->oldMIDITrack = seq->getCurrentMIDITrack();
				seq->setCurrentMIDITrack(this->midiTrack);

				return true;
			}
		}
	}
	return false;
}

bool ActionSetCurrentMIDITrack::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				seq->setCurrentMIDITrack(this->oldMIDITrack);

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionSetCurrentMIDITrack::getStatusStr() const {
	return "[" + juce::String(this->track) + "] " + juce::String{ this->midiTrack };
}

void ActionSetCurrentMIDITrack::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->track);
	stream.writeInt(this->midiTrack);
	stream.writeInt(this->oldMIDITrack);
}

ActionSetSequencerBlockTime::ActionSetSequencerBlockTime(
	int track, int index, const BlockTime& time)
	: track(track), index(index), time(time) {}

bool ActionSetSequencerBlockTime::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				this->oldTime = seq->getSeq(this->index);
				this->newIndex = seq->resetSeqTime(this->index, this->time);
				if (this->newIndex >= 0) {
					return true;
				}
			}
			
		}
	}
	return false;
}

bool ActionSetSequencerBlockTime::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				seq->resetSeqTime(this->newIndex, this->oldTime);

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionSetSequencerBlockTime::getStatusStr() const {
	return "[" + juce::String{ this->track } + ", " + juce::String{ this->index } + "] " + juce::String{ std::get<0>(this->time), 2 } + ", " + juce::String{ std::get<1>(this->time), 2 } + ", " + juce::String{ std::get<2>(this->time), 2 };
}

void ActionSetSequencerBlockTime::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->track);
	stream.writeInt(this->index);
	stream.writeDouble(std::get<0>(this->time));
	stream.writeDouble(std::get<1>(this->time));
	stream.writeDouble(std::get<2>(this->time));

	stream.writeInt(this->newIndex);
	stream.writeDouble(std::get<0>(this->oldTime));
	stream.writeDouble(std::get<1>(this->oldTime));
	stream.writeDouble(std::get<2>(this->oldTime));
}

ActionSetPlayPosition::ActionSetPlayPosition(double pos)
	: pos(pos) {}

bool ActionSetPlayPosition::doAction() {
	AudioCore::getInstance()->setPositon(this->pos);
	return true;
}

const juce::String ActionSetPlayPosition::getStatusStr() const {
	return juce::String{ this->pos } + "s";
}

ActionSetReturnToStart::ActionSetReturnToStart(bool returnToStart)
	: returnToStart(returnToStart) {}

bool ActionSetReturnToStart::doAction() {
	AudioCore::getInstance()->setReturnToPlayStartPosition(this->returnToStart);
	return true;
}

const juce::String ActionSetReturnToStart::getStatusStr() const {
	return juce::String{ this->returnToStart ? "ON" : "OFF" };
}

ActionSetAudioSaveBitsPerSample::ActionSetAudioSaveBitsPerSample(
	const juce::String& format, int bitPerSample)
	: format(format), bitPerSample(bitPerSample) {}

bool ActionSetAudioSaveBitsPerSample::doAction() {
	AudioSaveConfig::getInstance()->setBitsPerSample(
		this->format, this->bitPerSample);
	return true;
}

const juce::String ActionSetAudioSaveBitsPerSample::getStatusStr() const {
	return "[" + this->format + "] " + juce::String{ this->bitPerSample };
}

ActionSetAudioSaveMetaData::ActionSetAudioSaveMetaData(
	const juce::String& format, const juce::StringPairArray& metaData)
	: format(format), metaData(metaData) {}

bool ActionSetAudioSaveMetaData::doAction() {
	AudioSaveConfig::getInstance()->setMetaData(
		this->format, this->metaData);
	return true;
}

const juce::String ActionSetAudioSaveMetaData::getStatusStr() const {
	juce::String result;
	result += "[" + this->format + "]\n";

	int size = this->metaData.size();
	auto& keys = this->metaData.getAllKeys();
	auto& values = this->metaData.getAllValues();
	for (int i = 0; i < size; i++) {
		result += "    " + keys[i] + " : " + values[i] + "\n";
	}

	return result;
}

ActionSetAudioSaveQualityOptionIndex::ActionSetAudioSaveQualityOptionIndex(
	const juce::String& format, int quality)
	: format(format), quality(quality) {}

bool ActionSetAudioSaveQualityOptionIndex::doAction() {
	AudioSaveConfig::getInstance()
		->setQualityOptionIndex(this->format, this->quality);
	return true;
}

const juce::String ActionSetAudioSaveQualityOptionIndex::getStatusStr() const {
	return "[" + this->format + "] " + juce::String{ this->quality };
}

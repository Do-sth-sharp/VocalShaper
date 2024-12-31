#include "ActionAdd.h"

#include "../AudioCore.h"
#include "../plugin/Plugin.h"
#include "../plugin/PluginLoader.h"
#include "../misc/PlayPosition.h"
#include "../Utils.h"

ActionAddPluginBlackList::ActionAddPluginBlackList(const juce::String& plugin)
	: plugin(plugin) {}

bool ActionAddPluginBlackList::doAction() {
	Plugin::getInstance()->addToPluginBlackList(this->plugin);
	return true;
}

const juce::String ActionAddPluginBlackList::getStatusStr() const {
	return this->plugin;
}

ActionAddPluginSearchPath::ActionAddPluginSearchPath(const juce::String& path)
	: path(path) {}

bool ActionAddPluginSearchPath::doAction() {
	Plugin::getInstance()->addToPluginSearchPath(this->path);
	return true;
}

const juce::String ActionAddPluginSearchPath::getStatusStr() const {
	return this->path;
}

ActionAddTrack::ActionAddTrack(
	quickAPI::TrackIndex index, int bus)
	: index(index), bus(bus) {}

bool ActionAddTrack::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		juce::AudioChannelSet busType = utils::getChannelSet(
			static_cast<utils::BusType>(this->bus));

		this->newIndex = graph->insertTrack(
			this->index.first, this->index.second, busType);

		if (this->newIndex >= 0) {
			return true;
		}
	}
	return false;
}

bool ActionAddTrack::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeTrack(this->index.first, this->newIndex);

		return true;
	}
	return false;
}

const juce::String ActionAddTrack::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + "]" + juce::String{ this->bus };
}

void ActionAddTrack::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
	stream.writeInt(this->bus);
	stream.writeInt(this->newIndex);
}

ActionAddTrackAudioInput::ActionAddTrackAudioInput(
	quickAPI::TrackIndex index,
	int srcc, int dstc)
	: index(index), srcc(srcc), dstc(dstc) {}

bool ActionAddTrackAudioInput::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->connectTrackAudioInput(
			this->index.first, this->index.second, this->srcc, this->dstc)) {
			return true;
		}
	}
	return false;
}

bool ActionAddTrackAudioInput::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->disconnectTrackAudioInput(
			this->index.first, this->index.second, this->srcc, this->dstc);
		return true;
	}
	return false;
}

const juce::String ActionAddTrackAudioInput::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + "] " + juce::String{ this->srcc } + " - " + juce::String{ this->dstc };
}

void ActionAddTrackAudioInput::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
	stream.writeInt(this->srcc);
	stream.writeInt(this->dstc);
}

ActionAddTrackMIDIInput::ActionAddTrackMIDIInput(
	quickAPI::TrackIndex index)
	: index(index) {}

bool ActionAddTrackMIDIInput::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->connectTrackMIDIInput(
			this->index.first, this->index.second)) {
			return true;
		}
	}
	return false;
}

bool ActionAddTrackMIDIInput::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->disconnectTrackMIDIInput(
			this->index.first, this->index.second);
		return true;
	}
	return false;
}

const juce::String ActionAddTrackMIDIInput::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + "]";
}

void ActionAddTrackMIDIInput::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
}

ActionAddTrackAudioSend::ActionAddTrackAudioSend(
	quickAPI::TrackIndex index, int slot,
	quickAPI::SendDst dst, int srcc, int dstc)
	: index(index), slot(slot), dst(dst), srcc(srcc), dstc(dstc) {}

bool ActionAddTrackAudioSend::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->connectTrackAudioSend(
			this->index.first, this->index.second, this->slot,
			this->dst.first, this->dst.second, this->srcc, this->dstc)) {
			return true;
		}
	}
	return false;
}

bool ActionAddTrackAudioSend::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->disconnectTrackAudioSend(
			this->index.first, this->index.second, this->slot,
			this->dst.first, this->dst.second, this->srcc, this->dstc);
		return true;
	}
	return false;
}

const juce::String ActionAddTrackAudioSend::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + " : " + juce::String{ this->slot } + "] " + juce::String{ this->srcc } + " - " + juce::String{ this->dstc };
}

void ActionAddTrackAudioSend::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
	stream.writeInt(this->slot);
	stream.writeInt((int)this->dst.first);
	stream.writeInt(this->dst.second);
	stream.writeInt(this->srcc);
	stream.writeInt(this->dstc);
}

ActionAddTrackMIDISend::ActionAddTrackMIDISend(
	quickAPI::TrackIndex index, int slot,
	quickAPI::SendDst dst)
	: index(index), slot(slot), dst(dst) {}

bool ActionAddTrackMIDISend::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->connectTrackMIDISend(
			this->index.first, this->index.second, this->slot,
			this->dst.first, this->dst.second)) {
			return true;
		}
	}
	return false;
}

bool ActionAddTrackMIDISend::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->disconnectTrackMIDISend(
			this->index.first, this->index.second, this->slot,
			this->dst.first, this->dst.second);
		return true;
	}
	return false;
}

const juce::String ActionAddTrackMIDISend::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + " : " + juce::String{ this->slot } + "]";
}

void ActionAddTrackMIDISend::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
	stream.writeInt(this->slot);
	stream.writeInt((int)this->dst.first);
	stream.writeInt(this->dst.second);
}

ActionAddEffect::ActionAddEffect(
	quickAPI::TrackIndex trackIndex,
	int effect, const juce::String& pid)
	: trackIndex(trackIndex), effect(effect), pid(pid) {}

bool ActionAddEffect::doAction() {
	if (auto des = Plugin::getInstance()->findPlugin(this->pid, false)) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(this->trackIndex.first, this->trackIndex.second)) {
				if (auto mixer = track->getMixer()) {
					if (auto pluginDock = mixer->getPluginDock()) {
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

bool ActionAddEffect::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->trackIndex.first, this->trackIndex.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					pluginDock->removePlugin(this->effect);
					return true;
				}
			}
		}
	}
	return false;
}

const juce::String ActionAddEffect::getStatusStr() const {
	return "[" + juce::String{ (int)(this->trackIndex.first) } + ", " + juce::String{ this->trackIndex.second } + ", " + juce::String(this->effect) + "] " + this->pid;
}

void ActionAddEffect::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->trackIndex.first);
	stream.writeInt(this->trackIndex.second);
	stream.writeInt(this->effect);
	stream.writeString(this->pid);
}

ActionAddInstr::ActionAddInstr(
	int index, const juce::String& pid, bool addARA)
	: index(index), pid(pid), addARA(addARA) {}

bool ActionAddInstr::doAction() {
	if (auto des = Plugin::getInstance()->findPlugin(this->pid, true, this->addARA)) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->index)) {
				if (auto seq = track->getSequencer()) {
					if (auto ptr = seq->prepareInstr()) {
						PluginLoader::getInstance()->loadPlugin(*(des.get()), this->addARA, ptr);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool ActionAddInstr::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->index)) {
			if (auto seq = track->getSequencer()) {
				seq->removeInstr();
				return true;
			}
		}
	}
	return false;
}

const juce::String ActionAddInstr::getStatusStr() const {
	return "[" + juce::String(this->index) + "] " + this->pid;
}

void ActionAddInstr::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->index);
	stream.writeString(this->pid);
	stream.writeBool(this->addARA);
}

ActionAddTrackSideChainBus::ActionAddTrackSideChainBus(
	quickAPI::TrackIndex index) : index(index) {}

bool ActionAddTrackSideChainBus::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->index.first, this->index.second)) {
			if (track->addAdditionalAudioBus()) {
				return true;
			}
		}
	}
	return false;
}

bool ActionAddTrackSideChainBus::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->index.first, this->index.second)) {
			if (track->removeAdditionalAudioBus()) {
				return true;
			}
		}
	}
	return false;
}

const juce::String ActionAddTrackSideChainBus::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + "]";
}

void ActionAddTrackSideChainBus::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
}

ActionAddSequencerBlock::ActionAddSequencerBlock(
	int trackIndex, double startTime, double endTime, double offset)
	: trackIndex(trackIndex), startTime(startTime), endTime(endTime), offset(offset) {}

bool ActionAddSequencerBlock::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->trackIndex)) {
			if (auto seq = track->getSequencer()) {
				this->index = seq->addSeq(
					{ this->startTime, this->endTime, this->offset });
				return true;
			}
		}
	}
	return false;
}

bool ActionAddSequencerBlock::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->trackIndex)) {
			if (auto seq = track->getSequencer()) {
				seq->removeSeq(this->index);
				return true;
			}
		}
	}
	return false;
}

const juce::String ActionAddSequencerBlock::getStatusStr() const {
	return "[" + juce::String(this->trackIndex) + "]";
}

void ActionAddSequencerBlock::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->trackIndex);
	stream.writeDouble(this->startTime);
	stream.writeDouble(this->endTime);
	stream.writeDouble(this->offset);
	stream.writeInt(this->index);
}

ActionAddLabelTempo::ActionAddLabelTempo(
	double time, double tempo)
	: time(time), tempo(tempo) {}

bool ActionAddLabelTempo::doAction() {
	this->index = PlayPosition::getInstance()
		->addTempoLabelTempo(this->time, this->tempo, this->index);
	return true;
}

bool ActionAddLabelTempo::undoAction() {
	PlayPosition::getInstance()->removeTempoLabel(this->index);
	return true;
}

const juce::String  ActionAddLabelTempo::getStatusStr() const {
	return "[" + juce::String{ this->time } + "] " + juce::String{ this->tempo, 2 };
}

void ActionAddLabelTempo::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeDouble(this->time);
	stream.writeDouble(this->tempo);
	stream.writeInt(this->index);
}

ActionAddLabelBeat::ActionAddLabelBeat(
	double time, int numerator, int denominator)
	: time(time), numerator(numerator), denominator(denominator) {}

bool ActionAddLabelBeat::doAction() {
	this->index = PlayPosition::getInstance()
		->addTempoLabelBeat(this->time,
			this->numerator, this->denominator, this->index);
	return true;
}

bool ActionAddLabelBeat::undoAction() {
	PlayPosition::getInstance()->removeTempoLabel(this->index);
	return true;
}

const juce::String ActionAddLabelBeat::getStatusStr() const {
	return "[" + juce::String{ this->time } + "] " + juce::String{ this->numerator } + " / " + juce::String{ this->denominator };
}

void ActionAddLabelBeat::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeDouble(this->time);
	stream.writeInt(this->numerator);
	stream.writeInt(this->denominator);
	stream.writeInt(this->index);
}

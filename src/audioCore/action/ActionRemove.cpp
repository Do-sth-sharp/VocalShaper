#include "ActionRemove.h"

#include "../AudioCore.h"
#include "../misc/PlayPosition.h"
#include "../plugin/Plugin.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

ActionRemovePluginBlackList::ActionRemovePluginBlackList(
	const juce::String& path)
	: path(path) {}

bool ActionRemovePluginBlackList::doAction() {
	Plugin::getInstance()->removeFromPluginBlackList(this->path);
	return true;
}

const juce::String ActionRemovePluginBlackList::getStatusStr() const {
	return this->path;
}

ActionRemovePluginSearchPath::ActionRemovePluginSearchPath(
	const juce::String& path)
	: path(path) {}

bool ActionRemovePluginSearchPath::doAction() {
	Plugin::getInstance()->removeFromPluginSearchPath(this->path);
	return true;
}

const juce::String ActionRemovePluginSearchPath::getStatusStr() const {
	return this->path;
}

ActionRemoveTrack::ActionRemoveTrack(quickAPI::TrackIndex index)
	: index(index) {}

bool ActionRemoveTrack::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Check Track */
		if (this->index.second < 0 || this->index.second >= graph->getTrackNum(this->index.first)) { return false; }

		/** Save Connections */
		this->midiInput = graph->isTrackMIDIInputConnected(this->index.first, this->index.second);
		this->audioInput = graph->getTrackAudioInputChannels(this->index.first, this->index.second);

		const int midiSendSlots = MainGraph::getMIDISendSlotNum();
		const int audioSendSlots = MainGraph::getAudioSendSlotNum();

		this->midiSend.clearQuick();
		for (int i = 0; i < midiSendSlots; i++) {
			this->midiSend.add(graph->getTrackMIDISendDst(this->index.first, this->index.second, i));
		}

		this->audioSend.clearQuick();
		for (int i = 0; i < audioSendSlots; i++) {
			this->audioSend.add({
				graph->getTrackAudioSendDst(this->index.first, this->index.second, i),
				graph->getTrackAudioSendChannels(this->index.first, this->index.second, i)
				});
		}

		/** Save Track State */
		auto track = graph->getTrackProcessor(this->index.first, this->index.second);
		if (!track) { return false; }
		auto state = track->serialize(Serializable::createSerializeConfigQuickly());

		auto statePtr = dynamic_cast<vsp4::Track*>(state.get());
		if (!statePtr) { return false; }

		this->data.setSize(state->ByteSizeLong());
		state->SerializeToArray(this->data.getData(), this->data.getSize());

		/** Remove Track */
		graph->removeTrack(this->index.first, this->index.second);

		return true;
	}
	return false;
}

bool ActionRemoveTrack::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Prepare Track State */
		auto state = std::make_unique<vsp4::Track>();
		if (!state->ParseFromArray(this->data.getData(), this->data.getSize())) {
			return false;
		}

		/** Add Track */
		graph->insertTrack(this->index.first, this->index.second,
			utils::getChannelSet(static_cast<utils::BusType>(state->bus())));
		
		/** Recover Track State */
		auto track = graph->getTrackProcessor(this->index.first, this->index.second);
		track->parse(state.get(), Serializable::createParseConfigQuickly());

		/** Recover Connections */
		if (this->midiInput) {
			graph->connectTrackMIDIInput(this->index.first, this->index.second);
		}
		for (auto& i : this->audioInput) {
			graph->connectTrackAudioInput(
				this->index.first, this->index.second, i.first, i.second);
		}
		for (int i = 0; i < this->midiSend.size(); i++) {
			auto& dst = this->midiSend.getReference(i);
			if (dst.second >= 0) {
				graph->connectTrackMIDISend(
					this->index.first, this->index.second, i, dst.first, dst.second);
			}
		}
		for (int i = 0; i < this->audioSend.size(); i++) {
			auto& dst = this->audioSend.getReference(i);
			if (dst.first.second >= 0) {
				for (auto& j : dst.second) {
					graph->connectTrackAudioSend(
						this->index.first, this->index.second, i,
						dst.first.first, dst.first.second, j.first, j.second);
				}
			}
		}

		return true;
	}
	return false;
}

const juce::String ActionRemoveTrack::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + "]";
}

void ActionRemoveTrack::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);

	stream.writeBool(this->midiInput);
	
	stream.writeInt(this->audioInput.size());
	for (auto& i : this->audioInput) {
		stream.writeInt(i.first);
		stream.writeInt(i.second);
	}

	stream.writeInt(this->midiSend.size());
	for (auto i : this->midiSend) {
		stream.writeInt((int)(i.first));
		stream.writeInt(i.second);
	}

	stream.writeInt(this->audioSend.size());
	for (auto& i : this->audioSend) {
		stream.writeInt((int)(i.first.first));
		stream.writeInt(i.first.second);

		stream.writeInt(i.second.size());
		for (auto& j : i.second) {
			stream.writeInt(j.first);
			stream.writeInt(j.second);
		}
	}

	stream.writeInt64(this->data.getSize());
	stream.write(this->data.getData(), this->data.getSize());
}

ActionRemoveTrackAudioInput::ActionRemoveTrackAudioInput(
	quickAPI::TrackIndex index, int srcc, int dstc)
	: index(index), srcc(srcc), dstc(dstc) {}

bool ActionRemoveTrackAudioInput::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->disconnectTrackAudioInput(
			this->index.first, this->index.second, this->srcc, this->dstc)) {
			return true;
		}
	}
	return false;
}

bool ActionRemoveTrackAudioInput::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->connectTrackAudioInput(
			this->index.first, this->index.second, this->srcc, this->dstc);

		return true;
	}
	return false;
}

const juce::String ActionRemoveTrackAudioInput::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + "] " + juce::String{ this->srcc } + " - " + juce::String{ this->dstc };
}

void ActionRemoveTrackAudioInput::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
	stream.writeInt(this->srcc);
	stream.writeInt(this->dstc);
}

ActionRemoveTrackMIDIInput::ActionRemoveTrackMIDIInput(
	quickAPI::TrackIndex index)
	: index(index) {
}

bool ActionRemoveTrackMIDIInput::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->disconnectTrackMIDIInput(
			this->index.first, this->index.second)) {
			return true;
		}
	}
	return false;
}

bool ActionRemoveTrackMIDIInput::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->connectTrackMIDIInput(
			this->index.first, this->index.second);
		return true;
	}
	return false;
}

const juce::String ActionRemoveTrackMIDIInput::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + "]";
}

void ActionRemoveTrackMIDIInput::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
}

ActionRemoveTrackAudioSend::ActionRemoveTrackAudioSend(
	quickAPI::TrackIndex index, int slot,
	quickAPI::SendDst dst, int srcc, int dstc)
	: index(index), slot(slot), dst(dst), srcc(srcc), dstc(dstc) {
}

bool ActionRemoveTrackAudioSend::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->disconnectTrackAudioSend(
			this->index.first, this->index.second, this->slot,
			this->dst.first, this->dst.second, this->srcc, this->dstc)) {
			return true;
		}
	}
	return false;
}

bool ActionRemoveTrackAudioSend::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->connectTrackAudioSend(
			this->index.first, this->index.second, this->slot,
			this->dst.first, this->dst.second, this->srcc, this->dstc);
		return true;
	}
	return false;
}

const juce::String ActionRemoveTrackAudioSend::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + " : " + juce::String{ this->slot } + "] " + juce::String{ this->srcc } + " - " + juce::String{ this->dstc };
}

void ActionRemoveTrackAudioSend::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
	stream.writeInt(this->slot);
	stream.writeInt((int)this->dst.first);
	stream.writeInt(this->dst.second);
	stream.writeInt(this->srcc);
	stream.writeInt(this->dstc);
}

ActionRemoveTrackMIDISend::ActionRemoveTrackMIDISend(
	quickAPI::TrackIndex index, int slot,
	quickAPI::SendDst dst)
	: index(index), slot(slot), dst(dst) {
}

bool ActionRemoveTrackMIDISend::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->disconnectTrackMIDISend(
			this->index.first, this->index.second, this->slot,
			this->dst.first, this->dst.second)) {
			return true;
		}
	}
	return false;
}

bool ActionRemoveTrackMIDISend::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->connectTrackMIDISend(
			this->index.first, this->index.second, this->slot,
			this->dst.first, this->dst.second);
		return true;
	}
	return false;
}

const juce::String ActionRemoveTrackMIDISend::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + " : " + juce::String{ this->slot } + "]";
}

void ActionRemoveTrackMIDISend::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
	stream.writeInt(this->slot);
	stream.writeInt((int)this->dst.first);
	stream.writeInt(this->dst.second);
}

ActionRemoveTrackAudioSendAllChannel::ActionRemoveTrackAudioSendAllChannel(
	quickAPI::TrackIndex index, int slot, quickAPI::SendDst dst)
	: index(index), slot(slot), dst(dst) {}

bool ActionRemoveTrackAudioSendAllChannel::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		this->channels = graph->getTrackAudioSendChannels(
			this->index.first, this->index.second, this->slot);

		if (graph->disconnectTrackAudioSend(
			this->index.first, this->index.second, this->slot,
			this->dst.first, this->dst.second)) {
			return true;
		}
	}
	return false;
}

bool ActionRemoveTrackAudioSendAllChannel::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		for (auto& i : this->channels) {
			graph->connectTrackAudioSend(
				this->index.first, this->index.second, this->slot,
				this->dst.first, this->dst.second, i.first, i.second);
		}
		return true;
	}
	return false;
}

const juce::String ActionRemoveTrackAudioSendAllChannel::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + " : " + juce::String{ this->slot } + "]";
}

void ActionRemoveTrackAudioSendAllChannel::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
	stream.writeInt(this->slot);
	stream.writeInt((int)this->dst.first);
	stream.writeInt(this->dst.second);

	stream.writeInt(this->channels.size());
	for (auto& i : this->channels) {
		stream.writeInt(i.first);
		stream.writeInt(i.second);
	}
}

ActionRemoveTrackAudioSendOnSlot::ActionRemoveTrackAudioSendOnSlot(
	quickAPI::TrackIndex index, int slot)
	: index(index), slot(slot) {}

bool ActionRemoveTrackAudioSendOnSlot::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		this->channels = graph->getTrackAudioSendChannels(
			this->index.first, this->index.second, this->slot);
		this->dst = graph->getTrackAudioSendDst(
			this->index.first, this->index.second, this->slot);

		if (graph->disconnectTrackAudioSend(
			this->index.first, this->index.second, this->slot)) {
			return true;
		}
	}
	return false;
}

bool ActionRemoveTrackAudioSendOnSlot::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		for (auto& i : this->channels) {
			graph->connectTrackAudioSend(
				this->index.first, this->index.second, this->slot,
				this->dst.first, this->dst.second, i.first, i.second);
		}
		return true;
	}
	return false;
}

const juce::String ActionRemoveTrackAudioSendOnSlot::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + " : " + juce::String{ this->slot } + "]";
}

void ActionRemoveTrackAudioSendOnSlot::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
	stream.writeInt(this->slot);

	stream.writeInt((int)this->dst.first);
	stream.writeInt(this->dst.second);

	stream.writeInt(this->channels.size());
	for (auto& i : this->channels) {
		stream.writeInt(i.first);
		stream.writeInt(i.second);
	}
}

ActionRemoveTrackMIDISendOnSlot::ActionRemoveTrackMIDISendOnSlot(
	quickAPI::TrackIndex index, int slot)
	: index(index), slot(slot) {}

bool ActionRemoveTrackMIDISendOnSlot::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		this->dst = graph->getTrackMIDISendDst(
			this->index.first, this->index.second, this->slot);

		if (graph->disconnectTrackMIDISend(
			this->index.first, this->index.second, this->slot)) {
			return true;
		}
	}
	return false;
}

bool ActionRemoveTrackMIDISendOnSlot::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->connectTrackMIDISend(
			this->index.first, this->index.second, this->slot,
			this->dst.first, this->dst.second);
		return true;
	}
	return false;
}

const juce::String ActionRemoveTrackMIDISendOnSlot::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + " : " + juce::String{ this->slot } + "]";
}

void ActionRemoveTrackMIDISendOnSlot::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
	stream.writeInt(this->slot);

	stream.writeInt((int)this->dst.first);
	stream.writeInt(this->dst.second);
}

ActionRemoveEffect::ActionRemoveEffect(
	quickAPI::TrackIndex trackIndex, int effect)
	: trackIndex(trackIndex), effect(effect) {}

bool ActionRemoveEffect::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->trackIndex.first, this->trackIndex.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto dock = mixer->getPluginDock()) {
					/** Check Effect */
					if (this->effect < 0 || this->effect >= dock->getSlotNum()) { return false; }

					/** Save Effect State */
					auto effect = dock->getPluginProcessor(this->effect);
					if (!effect) { return false; }
					auto state = effect->serialize(Serializable::createSerializeConfigQuickly());

					auto statePtr = dynamic_cast<vsp4::Plugin*>(state.get());
					if (!statePtr) { return false; }
					statePtr->set_bypassed(dock->getPluginBypass(this->effect));

					this->data.setSize(state->ByteSizeLong());
					state->SerializeToArray(this->data.getData(), this->data.getSize());

					/** Remove Effect */
					dock->removePlugin(this->effect);

					return true;
				}
			}
		}
	}
	return false;
}

bool ActionRemoveEffect::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->trackIndex.first, this->trackIndex.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto dock = mixer->getPluginDock()) {
					/** Prepare Effect State */
					auto state = std::make_unique<vsp4::Plugin>();
					if (!state->ParseFromArray(this->data.getData(), this->data.getSize())) {
						return false;
					}

					/** Add Effect */
					dock->insertPlugin(this->effect);

					/** Recover Effect State */
					auto effect = dock->getPluginProcessor(this->effect);
					dock->setPluginBypass(this->effect, state->bypassed());
					effect->parse(state.get(), Serializable::createParseConfigQuickly());

					return true;
				}
			}
		}
	}
	return false;
}

const juce::String ActionRemoveEffect::getStatusStr() const {
	return "[" + juce::String{ (int)(this->trackIndex.first) } + ", " + juce::String{ this->trackIndex.second } + ", " + juce::String{ this->effect } + "]";
}

void ActionRemoveEffect::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->trackIndex.first);
	stream.writeInt(this->trackIndex.second);
	stream.writeInt(this->effect);

	stream.writeInt64(this->data.getSize());
	stream.write(this->data.getData(), this->data.getSize());
}

ActionRemoveInstr::ActionRemoveInstr(int index)
	: index(index) {}

bool ActionRemoveInstr::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->index)) {
			if (auto seq = track->getSequencer()) {
				/** Save Instr State */
				auto instr = seq->getInstrProcessor();
				if (!instr) { return false; }
				auto state = instr->serialize(Serializable::createSerializeConfigQuickly());

				auto statePtr = dynamic_cast<vsp4::Plugin*>(state.get());
				if (!statePtr) { return false; }
				statePtr->set_bypassed(seq->getInstrumentBypass());

				this->data.setSize(state->ByteSizeLong());
				state->SerializeToArray(this->data.getData(), this->data.getSize());

				/** Remove Instr */
				seq->removeInstr();

				return true;
			}
		}
	}
	return false;
}

bool ActionRemoveInstr::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->index)) {
			if (auto seq = track->getSequencer()) {
				/** Prepare Instr State */
				auto state = std::make_unique<vsp4::Plugin>();
				if (!state->ParseFromArray(this->data.getData(), this->data.getSize())) {
					return false;
				}

				/** Add Instr */
				seq->prepareInstr();

				/** Recover Instr State */
				auto instr = seq->getInstrProcessor();
				seq->setInstrumentBypass(state->bypassed());
				instr->parse(state.get(), Serializable::createParseConfigQuickly());

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionRemoveInstr::getStatusStr() const {
	return "[" + juce::String(this->index) + "]";
}

void ActionRemoveInstr::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->index);

	stream.writeInt64(this->data.getSize());
	stream.write(this->data.getData(), this->data.getSize());
}

ActionRemoveTrackSideChainBus::ActionRemoveTrackSideChainBus(
	quickAPI::TrackIndex index) : index(index) {
}

bool ActionRemoveTrackSideChainBus::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Remove Bus */
		if (graph->removeTrackAdditionalAudioBus(this->index.first, this->index.second)) {
			return true;
		}
	}
	return false;
}

bool ActionRemoveTrackSideChainBus::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Add Bus */
		if (graph->addTrackAdditionalAudioBus(this->index.first, this->index.second)) {
			return true;
		}
	}
	return false;
}

const juce::String ActionRemoveTrackSideChainBus::getStatusStr() const {
	return "[" + juce::String{ (int)(this->index.first) } + ", " + juce::String{ this->index.second } + "]";
}

void ActionRemoveTrackSideChainBus::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->index.first);
	stream.writeInt(this->index.second);
}

ActionRemoveInstrParamCCConnection::ActionRemoveInstrParamCCConnection(
	int instr, int cc)
	: instr(instr), cc(cc) {}

bool ActionRemoveInstrParamCCConnection::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					this->param = instr->getCCParamConnection(this->cc);

					instr->removeCCParamConnection(this->cc);

					return true;
				}
			}
		}
	}
	return false;
}

bool ActionRemoveInstrParamCCConnection::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->instr)) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					instr->connectParamCC(this->param, this->cc);

					return true;
				}
			}
		}
	}
	return false;
}

const juce::String ActionRemoveInstrParamCCConnection::getStatusStr() const {
	return "MIDI CC " + juce::String{ this->cc };
}

void ActionRemoveInstrParamCCConnection::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->instr);
	stream.writeInt(this->cc);
	stream.writeInt(this->param);
}

ActionRemoveEffectParamCCConnection::ActionRemoveEffectParamCCConnection(
	quickAPI::TrackIndex track, int effect, int cc)
	: track(track), effect(effect), cc(cc) {}

bool ActionRemoveEffectParamCCConnection::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
						this->param = effect->getCCParamConnection(this->cc);

						effect->removeCCParamConnection(this->cc);

						return true;
					}
				}
			}
		}
	}
	return false;
}

bool ActionRemoveEffectParamCCConnection::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track.first, this->track.second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
						effect->connectParamCC(this->param, this->cc);

						return true;
					}
				}
			}
		}
	}
	return false;
}

const juce::String ActionRemoveEffectParamCCConnection::getStatusStr() const {
	return "MIDI CC " + juce::String{ this->cc };
}

void ActionRemoveEffectParamCCConnection::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt((int)this->track.first);
	stream.writeInt(this->track.second);
	stream.writeInt(this->effect);
	stream.writeInt(this->cc);
	stream.writeInt(this->param);
}

ActionRemoveSequencerBlock::ActionRemoveSequencerBlock(
	int seqIndex, int index)
	: seqIndex(seqIndex), index(index) {}

bool ActionRemoveSequencerBlock::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->seqIndex)) {
			if (auto seq = track->getSequencer()) {
				std::tie(this->startTime, this->endTime, this->offset)
					= seq->getSeq(this->index);
				seq->removeSeq(this->index);

				return true;
			}
		}
	}
	return false;
}

bool ActionRemoveSequencerBlock::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->seqIndex)) {
			if (auto seq = track->getSequencer()) {
				seq->addSeq(
					{ this->startTime, this->endTime, this->offset });

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionRemoveSequencerBlock::getStatusStr() const {
	return "[" + juce::String(this->seqIndex) + "]";
}

void ActionRemoveSequencerBlock::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->seqIndex);
	stream.writeInt(this->index);
	stream.writeDouble(this->startTime);
	stream.writeDouble(this->endTime);
	stream.writeDouble(this->offset);
}

ActionRemoveLabel::ActionRemoveLabel(int index)
	: index(index) {}

bool ActionRemoveLabel::doAction() {
	if (this->index >= 0 && this->index < PlayPosition::getInstance()->getTempoLabelNum()) {
		this->isTempo = PlayPosition::getInstance()->isTempoLabelTempoEvent(this->index);
		this->time = PlayPosition::getInstance()->getTempoLabelTime(this->index);

		if (this->isTempo) {
			this->tempo = PlayPosition::getInstance()->getTempoLabelTempo(this->index);
		}
		else {
			std::tie(this->numerator, this->denominator) =
				PlayPosition::getInstance()->getTempoLabelBeat(this->index);
		}

		PlayPosition::getInstance()->removeTempoLabel(this->index);

		return true;
	}
	return false;
}

bool ActionRemoveLabel::undoAction() {
	if (this->isTempo) {
		PlayPosition::getInstance()->addTempoLabelTempo(
			this->time, this->tempo, this->index);
	}
	else {
		PlayPosition::getInstance()->addTempoLabelBeat(
			this->time, this->numerator, this->denominator, this->index);
	}
	return true;
}

const juce::String ActionRemoveLabel::getStatusStr() const {
	return "Index: " + juce::String{ this->index } + ", Time: " + juce::String{ this->time } + "s, Is Tempo: " + juce::String{ this->isTempo ? "Yes" : "No" }
	+ ", Tempo: " + juce::String{ this->tempo } + ", Beat: " + juce::String{ this->numerator } + " / " + juce::String{ this->denominator };
}

void ActionRemoveLabel::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->index);
	stream.writeDouble(this->time);
	stream.writeDouble(this->tempo);
	stream.writeInt(this->numerator);
	stream.writeInt(this->denominator);
	stream.writeBool(this->isTempo);
}

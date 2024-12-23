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
	ACTION_CHECK_PLUGIN_LOADING(
		"Don't do this while loading plugin.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change plugin black list while searching plugin.");

	Plugin::getInstance()->removeFromPluginBlackList(this->path);
	
	this->output("Remove from plugin black list.");
	ACTION_RESULT(true);
}

ActionRemovePluginSearchPath::ActionRemovePluginSearchPath(
	const juce::String& path)
	: path(path) {}

bool ActionRemovePluginSearchPath::doAction() {
	ACTION_CHECK_PLUGIN_LOADING(
		"Don't do this while loading plugin.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change plugin search path while searching plugin.");

	Plugin::getInstance()->removeFromPluginSearchPath(this->path);
	
	this->output("Remove from plugin search path.");
	ACTION_RESULT(true);
}

ActionRemoveTrack::ActionRemoveTrack(quickAPI::TrackIndex index)
	: ACTION_DB{ index } {}

bool ActionRemoveTrack::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveTrack);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(audioInput).size());
	for (auto& i : ACTION_DATA(audioInput)) {
		writeRecoveryIntValue(i.first);
		writeRecoveryIntValue(i.second);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSend).size());
	for (auto i : ACTION_DATA(midiSend)) {
		writeRecoveryIntValue((int)(i.first));
		writeRecoveryIntValue(i.second);
	}

	writeRecoverySizeValue(ACTION_DATA(audioSend).size());
	for (auto& i : ACTION_DATA(audioSend)) {
		writeRecoveryIntValue((int)(i.first.first));
		writeRecoveryIntValue(i.first.second);
		
		writeRecoverySizeValue(i.second.size());
		for (auto& j : i.second) {
			writeRecoveryIntValue(j.first);
			writeRecoveryIntValue(j.second);
		}
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Check Track */
		if (ACTION_DATA(index).second < 0 || ACTION_DATA(index).second >= graph->getTrackNum(ACTION_DATA(index).first)) { ACTION_RESULT(false); }

		/** Save Connections */
		ACTION_DATA(midiInput) = graph->isTrackMIDIInputConnected(ACTION_DATA(index).first, ACTION_DATA(index).second);
		ACTION_DATA(audioInput) = graph->getTrackAudioInputChannels(ACTION_DATA(index).first, ACTION_DATA(index).second);

		const int midiSendSlots = MainGraph::getMIDISendSlotNum();
		const int audioSendSlots = MainGraph::getAudioSendSlotNum();

		ACTION_DATA(midiSend).clearQuick();
		for (int i = 0; i < midiSendSlots; i++) {
			ACTION_DATA(midiSend).add(graph->getTrackMIDISendDst(ACTION_DATA(index).first, ACTION_DATA(index).second, i));
		}

		ACTION_DATA(audioSend).clearQuick();
		for (int i = 0; i < audioSendSlots; i++) {
			ACTION_DATA(audioSend).add({
				graph->getTrackAudioSendDst(ACTION_DATA(index).first, ACTION_DATA(index).second, i),
				graph->getTrackAudioSendChannels(ACTION_DATA(index).first, ACTION_DATA(index).second, i)
				});
		}

		/** Save Track State */
		auto track = graph->getTrackProcessor(ACTION_DATA(index).first, ACTION_DATA(index).second);
		if (!track) { ACTION_RESULT(false); }
		auto state = track->serialize(Serializable::createSerializeConfigQuickly());

		auto statePtr = dynamic_cast<vsp4::Track*>(state.get());
		if (!statePtr) { ACTION_RESULT(false); }

		ACTION_DATA(data).setSize(state->ByteSizeLong());
		state->SerializeToArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize());

		/** Remove Track */
		graph->removeTrack(ACTION_DATA(index).first, ACTION_DATA(index).second);

		juce::String result;
		result += "Remove Mixer Track: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "]" + "\n";
		result += "Total Mixer Track Num: " + juce::String{ graph->getTrackNum(ACTION_DATA(index).first) } + "\n";
		this->output(result);
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveTrack::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveTrack);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(audioInput).size());
	for (auto& i : ACTION_DATA(audioInput)) {
		writeRecoveryIntValue(i.first);
		writeRecoveryIntValue(i.second);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSend).size());
	for (auto i : ACTION_DATA(midiSend)) {
		writeRecoveryIntValue((int)(i.first));
		writeRecoveryIntValue(i.second);
	}

	writeRecoverySizeValue(ACTION_DATA(audioSend).size());
	for (auto& i : ACTION_DATA(audioSend)) {
		writeRecoveryIntValue((int)(i.first.first));
		writeRecoveryIntValue(i.first.second);

		writeRecoverySizeValue(i.second.size());
		for (auto& j : i.second) {
			writeRecoveryIntValue(j.first);
			writeRecoveryIntValue(j.second);
		}
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Prepare Track State */
		auto state = std::make_unique<vsp4::Track>();
		if (!state->ParseFromArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize())) {
			ACTION_RESULT(false);
		}

		/** Add Track */
		graph->insertTrack(ACTION_DATA(index).first, ACTION_DATA(index).second,
			utils::getChannelSet(static_cast<utils::BusType>(state->bus())));
		
		/** Recover Track State */
		auto track = graph->getTrackProcessor(ACTION_DATA(index).first, ACTION_DATA(index).second);
		track->parse(state.get(), Serializable::createParseConfigQuickly());

		/** Recover Connections */
		if (ACTION_DATA(midiInput)) {
			graph->connectTrackMIDIInput(ACTION_DATA(index).first, ACTION_DATA(index).second);
		}
		for (auto& i : ACTION_DATA(audioInput)) {
			graph->connectTrackAudioInput(
				ACTION_DATA(index).first, ACTION_DATA(index).second, i.first, i.second);
		}
		for (int i = 0; i < ACTION_DATA(midiSend).size(); i++) {
			auto& dst = ACTION_DATA(midiSend).getReference(i);
			if (dst.second >= 0) {
				graph->connectTrackMIDISend(
					ACTION_DATA(index).first, ACTION_DATA(index).second, i, dst.first, dst.second);
			}
		}
		for (int i = 0; i < ACTION_DATA(audioSend).size(); i++) {
			auto& dst = ACTION_DATA(audioSend).getReference(i);
			if (dst.first.second >= 0) {
				for (auto& j : dst.second) {
					graph->connectTrackAudioSend(
						ACTION_DATA(index).first, ACTION_DATA(index).second, i,
						dst.first.first, dst.first.second, j.first, j.second);
				}
			}
		}

		juce::String result;
		result += "Undo Remove Mixer Track: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "]" + "\n";
		result += "Total Mixer Track Num: " + juce::String{ graph->getTrackNum(ACTION_DATA(index).first) } + "\n";
		this->output(result);
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveTrackAudioInput::ActionRemoveTrackAudioInput(
	quickAPI::TrackIndex index, int srcc, int dstc)
	: ACTION_DB{ index, srcc, dstc } {}

bool ActionRemoveTrackAudioInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveTrackAudioInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->disconnectTrackAudioInput(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			this->output("Disonnect Audio Input: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "] " + juce::String{ ACTION_DATA(srcc) } + " - " + juce::String{ ACTION_DATA(dstc) } + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveTrackAudioInput::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveTrackAudioInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->connectTrackAudioInput(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo Disonnect Audio Input: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "] " + juce::String{ ACTION_DATA(srcc) } + " - " + juce::String{ ACTION_DATA(dstc) } + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveTrackMIDIInput::ActionRemoveTrackMIDIInput(
	quickAPI::TrackIndex index)
	: ACTION_DB{ index } {
}

bool ActionRemoveTrackMIDIInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveTrackMIDIInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->disconnectTrackMIDIInput(
			ACTION_DATA(index).first, ACTION_DATA(index).second)) {
			this->output("Disonnect MIDI Input: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "] " + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveTrackMIDIInput::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveTrackMIDIInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->connectTrackMIDIInput(
			ACTION_DATA(index).first, ACTION_DATA(index).second);

		this->output("Undo Disonnect MIDI Input: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "] " + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveTrackAudioSend::ActionRemoveTrackAudioSend(
	quickAPI::TrackIndex index, int slot,
	quickAPI::SendDst dst, int srcc, int dstc)
	: ACTION_DB{ index, slot, dst, srcc, dstc } {
}

bool ActionRemoveTrackAudioSend::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveTrackAudioSend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->disconnectTrackAudioSend(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot),
			ACTION_DATA(dst).first, ACTION_DATA(dst).second, ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			this->output("Disconnect Audio Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + juce::String{ ACTION_DATA(srcc) } + " - " + juce::String{ ACTION_DATA(dstc) } + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveTrackAudioSend::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveTrackAudioSend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->connectTrackAudioSend(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot),
			ACTION_DATA(dst).first, ACTION_DATA(dst).second, ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo Disconnect Audio Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + juce::String{ ACTION_DATA(srcc) } + " - " + juce::String{ ACTION_DATA(dstc) } + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveTrackMIDISend::ActionRemoveTrackMIDISend(
	quickAPI::TrackIndex index, int slot,
	quickAPI::SendDst dst)
	: ACTION_DB{ index, slot, dst } {
}

bool ActionRemoveTrackMIDISend::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveTrackMIDISend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->disconnectTrackMIDISend(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot),
			ACTION_DATA(dst).first, ACTION_DATA(dst).second)) {
			this->output("Disconnect MIDI Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveTrackMIDISend::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveTrackMIDISend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->connectTrackMIDISend(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot),
			ACTION_DATA(dst).first, ACTION_DATA(dst).second);

		this->output("Undo Disconnect MIDI Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveTrackAudioSendAllChannel::ActionRemoveTrackAudioSendAllChannel(
	quickAPI::TrackIndex index, int slot, quickAPI::SendDst dst)
	: ACTION_DB{ index, slot, dst } {}

bool ActionRemoveTrackAudioSendAllChannel::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveTrackAudioSendAllChannel);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(channels).size());
	for (auto& i : ACTION_DATA(channels)) {
		writeRecoveryIntValue(i.first);
		writeRecoveryIntValue(i.second);
	}

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		ACTION_DATA(channels) = graph->getTrackAudioSendChannels(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot));

		if (graph->disconnectTrackAudioSend(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot),
			ACTION_DATA(dst).first, ACTION_DATA(dst).second)) {
			this->output("Disconnect Audio Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveTrackAudioSendAllChannel::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveTrackAudioSendAllChannel);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(channels).size());
	for (auto& i : ACTION_DATA(channels)) {
		writeRecoveryIntValue(i.first);
		writeRecoveryIntValue(i.second);
	}

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		for (auto& i : ACTION_DATA(channels)) {
			graph->connectTrackAudioSend(
				ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot),
				ACTION_DATA(dst).first, ACTION_DATA(dst).second, i.first, i.second);
		}

		this->output("Undo Disconnect Audio Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveTrackAudioSendOnSlot::ActionRemoveTrackAudioSendOnSlot(
	quickAPI::TrackIndex index, int slot)
	: ACTION_DB{ index, slot } {}

bool ActionRemoveTrackAudioSendOnSlot::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveTrackAudioSendOnSlot);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(channels).size());
	for (auto& i : ACTION_DATA(channels)) {
		writeRecoveryIntValue(i.first);
		writeRecoveryIntValue(i.second);
	}

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		ACTION_DATA(channels) = graph->getTrackAudioSendChannels(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot));
		ACTION_DATA(dst) = graph->getTrackAudioSendDst(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot));

		if (graph->disconnectTrackAudioSend(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot))) {
			this->output("Disconnect Audio Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveTrackAudioSendOnSlot::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveTrackAudioSendOnSlot);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(channels).size());
	for (auto& i : ACTION_DATA(channels)) {
		writeRecoveryIntValue(i.first);
		writeRecoveryIntValue(i.second);
	}

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		for (auto& i : ACTION_DATA(channels)) {
			graph->connectTrackAudioSend(
				ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot),
				ACTION_DATA(dst).first, ACTION_DATA(dst).second, i.first, i.second);
		}

		this->output("Undo Disconnect Audio Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveTrackMIDISendOnSlot::ActionRemoveTrackMIDISendOnSlot(
	quickAPI::TrackIndex index, int slot)
	: ACTION_DB{ index, slot } {
}

bool ActionRemoveTrackMIDISendOnSlot::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveTrackMIDISendOnSlot);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		ACTION_DATA(dst) = graph->getTrackMIDISendDst(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot));

		if (graph->disconnectTrackMIDISend(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot))) {
			this->output("Disconnect MIDI Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveTrackMIDISendOnSlot::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveTrackMIDISendOnSlot);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->connectTrackMIDISend(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot),
			ACTION_DATA(dst).first, ACTION_DATA(dst).second);

		this->output("Undo Disconnect MIDI Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveEffect::ActionRemoveEffect(
	quickAPI::TrackIndex trackIndex, int effect)
	: ACTION_DB{ trackIndex, effect } {}

bool ActionRemoveEffect::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveEffect);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(trackIndex).first, ACTION_DATA(trackIndex).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto dock = mixer->getPluginDock()) {
					/** Check Effect */
					if (ACTION_DATA(effect) < 0 || ACTION_DATA(effect) >= dock->getSlotNum()) { ACTION_RESULT(false); }

					/** Save Effect State */
					auto effect = dock->getPluginProcessor(ACTION_DATA(effect));
					if (!effect) { ACTION_RESULT(false); }
					auto state = effect->serialize(Serializable::createSerializeConfigQuickly());

					auto statePtr = dynamic_cast<vsp4::Plugin*>(state.get());
					if (!statePtr) { ACTION_RESULT(false); }
					statePtr->set_bypassed(dock->getPluginBypass(ACTION_DATA(effect)));

					ACTION_DATA(data).setSize(state->ByteSizeLong());
					state->SerializeToArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize());

					/** Remove Effect */
					dock->removePlugin(ACTION_DATA(effect));

					this->output("Remove Plugin: [" + juce::String{ (int)(ACTION_DATA(trackIndex).first) } + ", " + juce::String{ ACTION_DATA(trackIndex).second } + ", " + juce::String{ ACTION_DATA(effect) } + "]" + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveEffect::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change effect while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveEffect);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(trackIndex).first, ACTION_DATA(trackIndex).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto dock = mixer->getPluginDock()) {
					/** Prepare Effect State */
					auto state = std::make_unique<vsp4::Plugin>();
					if (!state->ParseFromArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize())) {
						ACTION_RESULT(false);
					}

					/** Add Effect */
					dock->insertPlugin(ACTION_DATA(effect));

					/** Recover Effect State */
					auto effect = dock->getPluginProcessor(ACTION_DATA(effect));
					dock->setPluginBypass(ACTION_DATA(effect), state->bypassed());
					effect->parse(state.get(), Serializable::createParseConfigQuickly());

					this->output("Undo Remove Plugin: [" + juce::String{ (int)(ACTION_DATA(trackIndex).first) } + ", " + juce::String{ ACTION_DATA(trackIndex).second } + ", " + juce::String{ ACTION_DATA(effect) } + "]" + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveInstr::ActionRemoveInstr(int index)
	: ACTION_DB{ index } {}

bool ActionRemoveInstr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveInstr);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(index))) {
			if (auto seq = track->getSequencer()) {
				/** Save Instr State */
				auto instr = seq->getInstrProcessor();
				if (!instr) { ACTION_RESULT(false); }
				auto state = instr->serialize(Serializable::createSerializeConfigQuickly());

				auto statePtr = dynamic_cast<vsp4::Plugin*>(state.get());
				if (!statePtr) { ACTION_RESULT(false); }
				statePtr->set_bypassed(seq->getInstrumentBypass());

				ACTION_DATA(data).setSize(state->ByteSizeLong());
				state->SerializeToArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize());

				/** Remove Instr */
				seq->removeInstr();

				this->output("Remove Instrument: [" + juce::String(ACTION_DATA(index)) + "]" + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveInstr::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change instrument while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveInstr);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(index))) {
			if (auto seq = track->getSequencer()) {
				/** Prepare Instr State */
				auto state = std::make_unique<vsp4::Plugin>();
				if (!state->ParseFromArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize())) {
					ACTION_RESULT(false);
				}

				/** Add Instr */
				seq->prepareInstr();

				/** Recover Instr State */
				auto instr = seq->getInstrProcessor();
				seq->setInstrumentBypass(state->bypassed());
				instr->parse(state.get(), Serializable::createParseConfigQuickly());

				this->output("Undo Remove Instrument: [" + juce::String(ACTION_DATA(index)) + "]" + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveTrackSideChainBus::ActionRemoveTrackSideChainBus(
	quickAPI::TrackIndex index) : ACTION_DB{ index } {
}

bool ActionRemoveTrackSideChainBus::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveTrackSideChainBus);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Remove Bus */
		if (graph->removeTrackAdditionalAudioBus(ACTION_DATA(index).first, ACTION_DATA(index).second)) {
			this->output("Remove mixer track side chain bus: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "]\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveTrackSideChainBus::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveTrackSideChainBus);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Add Bus */
		if (graph->addTrackAdditionalAudioBus(ACTION_DATA(index).first, ACTION_DATA(index).second)) {
			this->output("Undo remove mixer track side chain bus: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "]\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveInstrParamCCConnection::ActionRemoveInstrParamCCConnection(
	int instr, int cc)
	: ACTION_DB{ instr, cc } {}

bool ActionRemoveInstrParamCCConnection::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveInstrParamCCConnection);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					ACTION_DATA(param) = instr->getCCParamConnection(ACTION_DATA(cc));

					instr->removeCCParamConnection(ACTION_DATA(cc));

					this->output("Remove Instr Param MIDI CC Connection: " "MIDI CC " + juce::String(ACTION_DATA(cc)) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveInstrParamCCConnection::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveInstrParamCCConnection);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(instr))) {
			if (auto seq = track->getSequencer()) {
				if (auto instr = seq->getInstrProcessor()) {
					instr->connectParamCC(ACTION_DATA(param), ACTION_DATA(cc));

					this->output("Undo Remove Instr Param MIDI CC Connection: " "MIDI CC " + juce::String(ACTION_DATA(cc)) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveEffectParamCCConnection::ActionRemoveEffectParamCCConnection(
	quickAPI::TrackIndex track, int effect, int cc)
	: ACTION_DB{ track, effect, cc } {}

bool ActionRemoveEffectParamCCConnection::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveEffectParamCCConnection);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
						ACTION_DATA(param) = effect->getCCParamConnection(ACTION_DATA(cc));

						effect->removeCCParamConnection(ACTION_DATA(cc));

						this->output("Remove Effect Param MIDI CC Connection: " "MIDI CC " + juce::String(ACTION_DATA(cc)) + "\n");
						ACTION_RESULT(true);
					}
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveEffectParamCCConnection::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveEffectParamCCConnection);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track).first, ACTION_DATA(track).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
						effect->connectParamCC(ACTION_DATA(param), ACTION_DATA(cc));

						this->output("Undo Remove Effect Param MIDI CC Connection: " "MIDI CC " + juce::String(ACTION_DATA(cc)) + "\n");
						ACTION_RESULT(true);
					}
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveSequencerBlock::ActionRemoveSequencerBlock(
	int seqIndex, int index)
	: ACTION_DB{ seqIndex, index } {}

bool ActionRemoveSequencerBlock::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveSequencerBlock);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(seqIndex))) {
			if (auto seq = track->getSequencer()) {
				std::tie(ACTION_DATA(startTime), ACTION_DATA(endTime), ACTION_DATA(offset))
					= seq->getSeq(ACTION_DATA(index));
				seq->removeSeq(ACTION_DATA(index));

				this->output("Remove sequencer block [" + juce::String(ACTION_DATA(seqIndex)) + "]\n"
					+ "Total sequencer blocks: " + juce::String(seq->getSeqNum()) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	this->output("Can't remove sequencer block [" + juce::String(ACTION_DATA(seqIndex)) + "]\n");
	ACTION_RESULT(false);
}

bool ActionRemoveSequencerBlock::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveSequencerBlock);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(seqIndex))) {
			if (auto seq = track->getSequencer()) {
				seq->addSeq(
					{ ACTION_DATA(startTime), ACTION_DATA(endTime), ACTION_DATA(offset) });

				this->output("Undo remove sequencer block [" + juce::String(ACTION_DATA(seqIndex)) + "]\n"
					+ "Total sequencer blocks: " + juce::String(seq->getSeqNum()) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	this->output("Can't undo remove sequencer block [" + juce::String(ACTION_DATA(seqIndex)) + "]\n");
	ACTION_RESULT(false);
}

ActionRemoveTempo::ActionRemoveTempo(int index)
	: ACTION_DB{ index } {}

bool ActionRemoveTempo::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveTempo);
	ACTION_WRITE_DB();

	if (ACTION_DATA(index) >= 0 && ACTION_DATA(index) < PlayPosition::getInstance()->getTempoLabelNum()) {
		ACTION_DATA(isTempo) = PlayPosition::getInstance()->isTempoLabelTempoEvent(ACTION_DATA(index));
		ACTION_DATA(time) = PlayPosition::getInstance()->getTempoLabelTime(ACTION_DATA(index));

		if (ACTION_DATA(isTempo)) {
			ACTION_DATA(tempo) = PlayPosition::getInstance()->getTempoLabelTempo(ACTION_DATA(index));
		}
		else {
			std::tie(ACTION_DATA(numerator), ACTION_DATA(denominator)) =
				PlayPosition::getInstance()->getTempoLabelBeat(ACTION_DATA(index));
		}

		PlayPosition::getInstance()->removeTempoLabel(ACTION_DATA(index));

		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveTempo::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveTempo);
	ACTION_WRITE_DB();

	if (ACTION_DATA(isTempo)) {
		PlayPosition::getInstance()->addTempoLabelTempo(
			ACTION_DATA(time), ACTION_DATA(tempo), ACTION_DATA(index));
	}
	else {
		PlayPosition::getInstance()->addTempoLabelBeat(
			ACTION_DATA(time), ACTION_DATA(numerator), ACTION_DATA(denominator), ACTION_DATA(index));
	}
	ACTION_RESULT(true);
}

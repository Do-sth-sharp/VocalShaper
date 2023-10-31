#include "ActionRemove.h"

#include "../AudioCore.h"
#include "../plugin/Plugin.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

ActionRemovePluginBlackList::ActionRemovePluginBlackList(
	const juce::String& path)
	: path(path) {}

bool ActionRemovePluginBlackList::doAction() {
	if (Plugin::getInstance()->pluginSearchThreadIsRunning()) {
		this->output("Don't change plugin black list while searching plugin.");
		return false;
	}

	Plugin::getInstance()->removeFromPluginBlackList(this->path);
	
	this->output("Remove from plugin black list.");
	return true;
}

ActionRemovePluginSearchPath::ActionRemovePluginSearchPath(
	const juce::String& path)
	: path(path) {}

bool ActionRemovePluginSearchPath::doAction() {
	if (Plugin::getInstance()->pluginSearchThreadIsRunning()) {
		this->output("Don't change plugin search path while searching plugin.");
		return false;
	}

	Plugin::getInstance()->removeFromPluginSearchPath(this->path);
	
	this->output("Remove from plugin search path.");
	return true;
}

ActionRemoveMixerTrack::ActionRemoveMixerTrack(int index)
	: index(index) {}

bool ActionRemoveMixerTrack::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Check Track */
		if (this->index < 0 || this->index >= graph->getTrackNum()) { return false; }

		/** Save Connections */
		this->audioT2Trk = graph->getTrackInputFromTrackConnections(this->index);
		this->audioSrc2Trk = graph->getTrackInputFromSrcConnections(this->index);
		this->audioInstr2Trk = graph->getTrackInputFromInstrConnections(this->index);
		this->audioI2Trk = graph->getTrackInputFromDeviceConnections(this->index);
		this->audioTrk2T = graph->getTrackOutputToTrackConnections(this->index);
		this->audioTrk2O = graph->getTrackOutputToDeviceConnections(this->index);
		this->midiSrc2Trk = graph->getTrackMidiInputFromSrcConnections(this->index);
		this->midiI2Trk = graph->getTrackMidiInputFromDeviceConnections(this->index);
		this->midiTrk2O = graph->getTrackMidiOutputToDeviceConnections(this->index);

		/** Save Track State */
		auto track = graph->getTrackProcessor(this->index);
		if (!track) { return false; }
		auto state = track->serialize();

		auto statePtr = dynamic_cast<vsp4::MixerTrack*>(state.get());
		if (!statePtr) { return false; }
		statePtr->set_bypassed(graph->getTrackBypass(this->index));

		this->data.setSize(state->ByteSizeLong());
		state->SerializeToArray(this->data.getData(), this->data.getSize());

		/** Remove Track */
		graph->removeTrack(this->index);

		juce::String result;
		result += "Remove Mixer Track: " + juce::String(this->index) + "\n";
		result += "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n";
		this->output(result);
		return true;
	}
	return false;
}

bool ActionRemoveMixerTrack::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Prepare Track State */
		auto state = std::make_unique<vsp4::MixerTrack>();
		if (!state->ParseFromArray(this->data.getData(), this->data.getSize())) {
			return false;
		}

		/** Add Track */
		graph->insertTrack(this->index,
			utils::getChannelSet(static_cast<utils::TrackType>(state->type())));
		
		/** Recover Track State */
		auto track = graph->getTrackProcessor(this->index);
		graph->setTrackBypass(this->index, state->bypassed());
		track->parse(state.get());

		/** Recover Connections */
		for (auto [src, srcc, dst, dstc] : this->audioT2Trk) {
			graph->setAudioTrk2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : this->audioSrc2Trk) {
			graph->setAudioSrc2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : this->audioInstr2Trk) {
			graph->setAudioInstr2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : this->audioI2Trk) {
			graph->setAudioI2TrkConnection(dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : this->audioTrk2T) {
			graph->setAudioTrk2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : this->audioTrk2O) {
			graph->setAudioTrk2OConnection(src, srcc, dstc);
		}

		for (auto [src, dst] : this->midiI2Trk) {
			graph->setMIDII2TrkConnection(dst);
		}

		for (auto [src, dst] : this->midiSrc2Trk) {
			graph->setMIDISrc2TrkConnection(src, dst);
		}

		for (auto [src, dst] : this->midiTrk2O) {
			graph->setMIDITrk2OConnection(src);
		}

		juce::String result;
		result += "Undo Remove Mixer Track: " + juce::String(this->index) + "\n";
		result += "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n";
		this->output(result);
		return true;
	}
	return false;
}

ActionRemoveMixerTrackSend::ActionRemoveMixerTrackSend(
	int src, int srcc, int dst, int dstc)
	: src(src), srcc(srcc), dst(dst), dstc(dstc) {}

bool ActionRemoveMixerTrackSend::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioTrk2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + " (Removed)\n");
		return true;
	}
	return false;
}

bool ActionRemoveMixerTrackSend::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output("Undo " + juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + " (Removed)\n");
		return true;
	}
	return false;
}

ActionRemoveMixerTrackInputFromDevice::ActionRemoveMixerTrackInputFromDevice(
	int srcc, int dst, int dstc)
	: srcc(srcc), dst(dst), dstc(dstc) {}

bool ActionRemoveMixerTrackInputFromDevice::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioI2TrkConnection(
			this->dst, this->srcc, this->dstc);

		this->output("[Device] " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + " (Removed)\n");
		return true;
	}
	return false;
}

bool ActionRemoveMixerTrackInputFromDevice::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioI2TrkConnection(
			this->dst, this->srcc, this->dstc);

		this->output("Undo [Device] " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + " (Removed)\n");
		return true;
	}
	return false;
}

ActionRemoveMixerTrackOutput::ActionRemoveMixerTrackOutput(
	int src, int srcc, int dstc)
	: src(src), srcc(srcc), dstc(dstc) {}

bool ActionRemoveMixerTrackOutput::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioTrk2OConnection(
			this->src, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + "[Device] " + juce::String(this->dstc) + " (Removed)\n");
		return true;
	}
	return false;
}

bool ActionRemoveMixerTrackOutput::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2OConnection(
			this->src, this->srcc, this->dstc);

		this->output("Undo " + juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + "[Device] " + juce::String(this->dstc) + " (Removed)\n");
		return true;
	}
	return false;
}

ActionRemoveEffect::ActionRemoveEffect(
	int track, int effect)
	: track(track), effect(effect) {}

bool ActionRemoveEffect::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto dock = track->getPluginDock()) {
				/** Check Effect */
				if (this->effect < 0 || this->effect >= dock->getPluginNum()) { return false; }

				/** Save Connections */
				this->additional = dock->getPluginAdditionalBusConnections(this->effect);

				/** Save Effect State */
				auto effect = dock->getPluginProcessor(this->effect);
				if (!effect) { return false; }
				auto state = effect->serialize();

				auto statePtr = dynamic_cast<vsp4::Plugin*>(state.get());
				if (!statePtr) { return false; }
				statePtr->set_bypassed(dock->getPluginBypass(this->effect));

				this->data.setSize(state->ByteSizeLong());
				state->SerializeToArray(this->data.getData(), this->data.getSize());

				/** Remove Effect */
				dock->removePlugin(this->effect);

				this->output("Remove Plugin: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "]" + "\n");
				return true;
			}
		}
	}
	return false;
}

bool ActionRemoveEffect::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto dock = track->getPluginDock()) {
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
				effect->parse(state.get());

				/** Recover Connections */
				for (auto [src, srcc, dst, dstc] : this->additional) {
					dock->addAdditionalBusConnection(dst, srcc, dstc);
				}

				this->output("Undo Remove Plugin: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "]" + "\n");
				return true;
			}
		}
	}
	return false;
}

ActionRemoveEffectAdditionalInput::ActionRemoveEffectAdditionalInput(
	int track, int effect, int srcc, int dstc)
	: track(track), effect(effect), srcc(srcc), dstc(dstc) {}

bool ActionRemoveEffectAdditionalInput::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->removeAdditionalBusConnection(this->effect, this->srcc, this->dstc);

				this->output("Unlink Plugin Channel: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + juce::String(this->srcc) + " - " + juce::String(this->dstc) + "\n");
				return true;
			}
		}
	}
	return false;
}

bool ActionRemoveEffectAdditionalInput::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->addAdditionalBusConnection(this->effect, this->srcc, this->dstc);

				this->output("Undo Unlink Plugin Channel: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + juce::String(this->srcc) + " - " + juce::String(this->dstc) + "\n");
				return true;
			}
		}
	}
	return false;
}

ActionRemoveInstr::ActionRemoveInstr(int index)
	: index(index) {}

bool ActionRemoveInstr::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Check Instr */
		if (this->index < 0 || this->index >= graph->getInstrumentNum()) { return false; }

		/** Save Connections */
		this->audioInstr2Trk = graph->getInstrOutputToTrackConnections(this->index);
		this->midiSrc2Instr = graph->getInstrMidiInputFromSrcConnections(this->index);
		this->midiI2Instr = graph->getInstrMidiInputFromDeviceConnections(this->index);

		/** Save Instr State */
		auto instr = graph->getInstrumentProcessor(this->index);
		if (!instr) { return false; }
		auto state = instr->serialize();

		auto statePtr = dynamic_cast<vsp4::Plugin*>(state.get());
		if (!statePtr) { return false; }
		statePtr->set_bypassed(graph->getInstrumentBypass(this->index));

		this->data.setSize(state->ByteSizeLong());
		state->SerializeToArray(this->data.getData(), this->data.getSize());

		/** Remove Instr */
		graph->removeInstrument(this->index);

		this->output("Remove Instrument: [" + juce::String(this->index) + "]" + "\n");
		return true;
	}
	return false;
}

bool ActionRemoveInstr::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Prepare Instr State */
		auto state = std::make_unique<vsp4::Plugin>();
		if (!state->ParseFromArray(this->data.getData(), this->data.getSize())) {
			return false;
		}

		/** Add Instr */
		graph->insertInstrument(this->index,
			utils::getChannelSet(static_cast<utils::TrackType>(state->info().decoratortype())));

		/** Recover Instr State */
		auto instr = graph->getInstrumentProcessor(this->index);
		graph->setInstrumentBypass(this->index, state->bypassed());
		instr->parse(state.get());

		/** Recover Connections */
		for (auto [src, srcc, dst, dstc] : this->audioInstr2Trk) {
			graph->setAudioInstr2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, dst] : this->midiSrc2Instr) {
			graph->setMIDISrc2InstrConnection(src, dst);
		}

		for (auto [src, dst] : this->midiI2Instr) {
			graph->setMIDII2InstrConnection(dst);
		}

		this->output("Undo Remove Instrument: [" + juce::String(this->index) + "]" + "\n");
		return true;
	}
	return false;
}

ActionRemoveInstrOutput::ActionRemoveInstrOutput(
	int src, int srcc, int dst, int dstc)
	: src(src), srcc(srcc), dst(dst), dstc(dstc) {}

bool ActionRemoveInstrOutput::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioInstr2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + " (Removed)\n");
		return true;
	}

	return false;
}

bool ActionRemoveInstrOutput::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioInstr2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output("Undo " + juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + " (Removed)\n");
		return true;
	}

	return false;
}

ActionRemoveInstrMidiInput::ActionRemoveInstrMidiInput(int dst)
	: dst(dst) {}

bool ActionRemoveInstrMidiInput::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDII2InstrConnection(this->dst);

		this->output(juce::String("[MIDI Input]") + " - " + juce::String(this->dst) + " (Removed)\n");
		return true;
	}

	return false;
}

bool ActionRemoveInstrMidiInput::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDII2InstrConnection(this->dst);

		this->output(juce::String("Undo [MIDI Input]") + " - " + juce::String(this->dst) + " (Removed)\n");
		return true;
	}

	return false;
}

ActionRemoveInstrParamCCConnection::ActionRemoveInstrParamCCConnection(
	int instr, int cc)
	: instr(instr), cc(cc) {}

bool ActionRemoveInstrParamCCConnection::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto instr = graph->getInstrumentProcessor(this->instr)) {
			this->param = instr->getCCParamConnection(this->cc);

			instr->removeCCParamConnection(this->cc);

			this->output("Remove Instr Param MIDI CC Connection: " "MIDI CC " + juce::String(this->cc) + "\n");
			return true;
		}
	}
	return false;
}

bool ActionRemoveInstrParamCCConnection::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto instr = graph->getInstrumentProcessor(this->instr)) {
			instr->connectParamCC(this->param, this->cc);

			this->output("Undo Remove Instr Param MIDI CC Connection: " "MIDI CC " + juce::String(this->cc) + "\n");
			return true;
		}
	}
	return false;
}

ActionRemoveEffectParamCCConnection::ActionRemoveEffectParamCCConnection(
	int track, int effect, int cc)
	: track(track), effect(effect), cc(cc) {}

bool ActionRemoveEffectParamCCConnection::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					this->param = effect->getCCParamConnection(this->cc);

					effect->removeCCParamConnection(this->cc);

					this->output("Remove Effect Param MIDI CC Connection: " "MIDI CC " + juce::String(this->cc) + "\n");
					return true;
				}
			}
		}
	}
	return false;
}

bool ActionRemoveEffectParamCCConnection::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					effect->connectParamCC(this->param, this->cc);

					this->output("Undo Remove Effect Param MIDI CC Connection: " "MIDI CC " + juce::String(this->cc) + "\n");
					return true;
				}
			}
		}
	}
	return false;
}

ActionRemoveMixerTrackMidiInput::ActionRemoveMixerTrackMidiInput(int index)
	: index(index) {}

bool ActionRemoveMixerTrackMidiInput::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDII2TrkConnection(this->index);

		this->output(juce::String("[MIDI Input]") + " - " + juce::String(this->index) + " (Removed)\n");
		return true;
	}
	return false;
}

bool ActionRemoveMixerTrackMidiInput::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDII2TrkConnection(this->index);

		this->output(juce::String("Undo [MIDI Input]") + " - " + juce::String(this->index) + " (Removed)\n");
		return true;
	}
	return false;
}

ActionRemoveMixerTrackMidiOutput::ActionRemoveMixerTrackMidiOutput(int index)
	: index(index) {}

bool ActionRemoveMixerTrackMidiOutput::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDITrk2OConnection(this->index);

		this->output(juce::String(this->index) + " - " + "[MIDI Output]" + " (Removed)\n");
		return true;
	}
	return false;
}

bool ActionRemoveMixerTrackMidiOutput::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDITrk2OConnection(this->index);

		this->output("Undo " + juce::String(this->index) + " - " + "[MIDI Output]" + " (Removed)\n");
		return true;
	}
	return false;
}

ActionRemoveSource::ActionRemoveSource(int index)
	: index(index) {}

bool ActionRemoveSource::doAction() {
	if (auto manager = CloneableSourceManager::getInstance()) {
		manager->removeSource(this->index);
		this->output("Total Source Num: " + juce::String(manager->getSourceNum()) + "\n");
		return true;
	}
	return false;
}

ActionRemoveSequencerTrack::ActionRemoveSequencerTrack(int index)
	: index(index) {}

bool ActionRemoveSequencerTrack::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Check Track */
		if (this->index < 0 || this->index >= graph->getSourceNum()) { return false; }

		/** Save Connections */
		this->audioSrc2Trk = graph->getSourceOutputToTrackConnections(this->index);
		this->midiSrc2Instr = graph->getSourceMidiOutputToInstrConnections(this->index);
		this->midiSrc2Trk = graph->getSourceMidiOutputToTrackConnections(this->index);

		/** Save Track State */
		auto track = graph->getSourceProcessor(this->index);
		if (!track) { return false; }
		auto state = track->serialize();

		auto statePtr = dynamic_cast<vsp4::SeqTrack*>(state.get());
		if (!statePtr) { return false; }
		statePtr->set_bypassed(graph->getSourceBypass(this->index));

		this->data.setSize(state->ByteSizeLong());
		state->SerializeToArray(this->data.getData(), this->data.getSize());

		/** Remove Track */
		graph->removeSource(this->index);

		juce::String result;
		result += "Remove Sequencer Track: " + juce::String(this->index) + "\n";
		result += "Total Sequencer Track Num: " + juce::String(graph->getSourceNum()) + "\n";
		return true;
	}

	return false;
}

bool ActionRemoveSequencerTrack::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Prepare Track State */
		auto state = std::make_unique<vsp4::SeqTrack>();
		if (!state->ParseFromArray(this->data.getData(), this->data.getSize())) {
			return false;
		}

		/** Add Track */
		graph->insertSource(this->index,
			utils::getChannelSet(static_cast<utils::TrackType>(state->type())));

		/** Recover Track State */
		auto track = graph->getSourceProcessor(this->index);
		graph->setSourceBypass(this->index, state->bypassed());
		track->parse(state.get());

		/** Recover Connections */
		for (auto [src, srcc, dst, dstc] : this->audioSrc2Trk) {
			graph->setAudioSrc2TrkConnection(src, dst, srcc, dstc);
		}
		
		for (auto [src, dst] : this->midiSrc2Instr) {
			graph->setMIDISrc2InstrConnection(src, dst);
		}

		for (auto [src, dst] : this->midiSrc2Trk) {
			graph->setMIDISrc2TrkConnection(src, dst);
		}

		juce::String result;
		result += "Undo Remove Sequencer Track: " + juce::String(this->index) + "\n";
		result += "Total Sequencer Track Num: " + juce::String(graph->getSourceNum()) + "\n";
		this->output(result);
		return true;
	}
	return false;
}

ActionRemoveSequencerTrackMidiOutputToMixer::ActionRemoveSequencerTrackMidiOutputToMixer(
	int src, int dst)
	: src(src), dst(dst) {}

bool ActionRemoveSequencerTrackMidiOutputToMixer::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDISrc2TrkConnection(this->src, this->dst);

		this->output(juce::String(this->src) + " - " + juce::String(this->dst) + " (Removed)\n");
		return true;
	}
	return false;
}

bool ActionRemoveSequencerTrackMidiOutputToMixer::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDISrc2TrkConnection(this->src, this->dst);

		this->output("Undo " + juce::String(this->src) + " - " + juce::String(this->dst) + " (Removed)\n");
		return true;
	}
	return false;
}

ActionRemoveSequencerTrackMidiOutputToInstr::ActionRemoveSequencerTrackMidiOutputToInstr(
	int src, int dst)
	: src(src), dst(dst) {}

bool ActionRemoveSequencerTrackMidiOutputToInstr::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDISrc2InstrConnection(this->src, this->dst);

		this->output(juce::String(this->src) + " - " + juce::String(this->dst) + " (Removed)\n");
		return true;
	}
	return false;
}

bool ActionRemoveSequencerTrackMidiOutputToInstr::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDISrc2InstrConnection(this->src, this->dst);

		this->output("Undo " + juce::String(this->src) + " - " + juce::String(this->dst) + " (Removed)\n");
		return true;
	}
	return false;
}

ActionRemoveSequencerTrackOutput::ActionRemoveSequencerTrackOutput(
	int src, int srcc, int dst, int dstc)
	: src(src), srcc(srcc), dst(dst), dstc(dstc) {}

bool ActionRemoveSequencerTrackOutput::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioSrc2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + " (Removed)\n");
		return true;
	}
	return false;
}

bool ActionRemoveSequencerTrackOutput::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioSrc2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output("Undo " + juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + " (Removed)\n");
		return true;
	}
	return false;
}

ActionRemoveSequencerSourceInstance::ActionRemoveSequencerSourceInstance(
	int track, int seq)
	: track(track), seq(seq) {}

bool ActionRemoveSequencerSourceInstance::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto seqTrack = graph->getSourceProcessor(this->track)) {
			std::tie(this->start, this->end, this->offset, std::ignore, this->index)
				= seqTrack->getSeq(this->seq);

			seqTrack->removeSeq(this->seq);

			juce::String result;
			result += "Remove Sequencer Source Instance [" + juce::String(this->track) + ", " + juce::String(this->seq) + "]\n";
			result += "Total Sequencer Source Instance: " + juce::String(seqTrack->getSeqNum()) + "\n";
			this->output(result);
			return true;
		}
	}
	return false;
}

bool ActionRemoveSequencerSourceInstance::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto seqTrack = graph->getSourceProcessor(this->track)) {
			auto ptrSrc = CloneableSourceManager::getInstance()->getSource(this->index);
			if (!ptrSrc) { return false; }

			seqTrack->addSeq({ this->start, this->end, this->offset, ptrSrc, this->index });

			juce::String result;
			result += "Undo Remove Sequencer Source Instance [" + juce::String(this->track) + ", " + juce::String(this->seq) + "]\n";
			result += "Total Sequencer Source Instance: " + juce::String(seqTrack->getSeqNum()) + "\n";
			this->output(result);
			return true;
		}
	}
	return false;
}

ActionRemoveRecorderSourceInstance::ActionRemoveRecorderSourceInstance(int seq)
	: seq(seq) {}

bool ActionRemoveRecorderSourceInstance::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto recorder = graph->getRecorder()) {
			std::tie(std::ignore, this->index, this->offset)
				= recorder->getTask(this->seq);

			recorder->removeTask(this->seq);

			juce::String result;
			result += "Remove Recorder Source Instance [" + juce::String(this->seq) + "]\n";
			result += "Total Recorder Source Instance: " + juce::String(recorder->getTaskNum()) + "\n";
			this->output(result);
			return true;
		}
	}
	return false;
}

bool ActionRemoveRecorderSourceInstance::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto recorder = graph->getRecorder()) {
			auto ptrSrc = CloneableSourceManager::getInstance()->getSource(this->index);
			if (!ptrSrc) { return false; }

			recorder->insertTask({ ptrSrc, this->index, this->offset }, this->seq);

			juce::String result;
			result += "Undo Remove Recorder Source Instance [" + juce::String(this->seq) + "]\n";
			result += "Total Recorder Source Instance: " + juce::String(recorder->getTaskNum()) + "\n";
			this->output(result);
			return true;
		}
	}
	return false;
}

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

ActionRemoveMixerTrack::ActionRemoveMixerTrack(int index)
	: ACTION_DB{ index } {}

bool ActionRemoveMixerTrack::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveMixerTrack);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(audioT2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioT2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioSrc2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioInstr2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioInstr2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioI2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioI2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioTrk2T).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioTrk2T)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioTrk2O).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioTrk2O)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSrc2Trk).size());
	for (auto [src, dst] : ACTION_DATA(midiSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(midiI2Trk).size());
	for (auto [src, dst] : ACTION_DATA(midiI2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(midiTrk2O).size());
	for (auto [src, dst] : ACTION_DATA(midiTrk2O)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Check Track */
		if (ACTION_DATA(index) < 0 || ACTION_DATA(index) >= graph->getTrackNum()) { ACTION_RESULT(false); }

		/** Save Connections */
		ACTION_DATA(audioT2Trk) = graph->getTrackInputFromTrackConnections(ACTION_DATA(index));
		ACTION_DATA(audioSrc2Trk) = graph->getTrackInputFromSrcConnections(ACTION_DATA(index));
		ACTION_DATA(audioInstr2Trk) = graph->getTrackInputFromInstrConnections(ACTION_DATA(index));
		ACTION_DATA(audioI2Trk) = graph->getTrackInputFromDeviceConnections(ACTION_DATA(index));
		ACTION_DATA(audioTrk2T) = graph->getTrackOutputToTrackConnections(ACTION_DATA(index));
		ACTION_DATA(audioTrk2O) = graph->getTrackOutputToDeviceConnections(ACTION_DATA(index));
		ACTION_DATA(midiSrc2Trk) = graph->getTrackMidiInputFromSrcConnections(ACTION_DATA(index));
		ACTION_DATA(midiI2Trk) = graph->getTrackMidiInputFromDeviceConnections(ACTION_DATA(index));
		ACTION_DATA(midiTrk2O) = graph->getTrackMidiOutputToDeviceConnections(ACTION_DATA(index));

		/** Save Track State */
		auto track = graph->getTrackProcessor(ACTION_DATA(index));
		if (!track) { ACTION_RESULT(false); }
		auto state = track->serialize();

		auto statePtr = dynamic_cast<vsp4::MixerTrack*>(state.get());
		if (!statePtr) { ACTION_RESULT(false); }
		statePtr->set_bypassed(graph->getTrackBypass(ACTION_DATA(index)));

		ACTION_DATA(data).setSize(state->ByteSizeLong());
		state->SerializeToArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize());

		/** Remove Track */
		graph->removeTrack(ACTION_DATA(index));

		juce::String result;
		result += "Remove Mixer Track: " + juce::String(ACTION_DATA(index)) + "\n";
		result += "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n";
		this->output(result);
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveMixerTrack::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveMixerTrack);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(audioT2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioT2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioSrc2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioInstr2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioInstr2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioI2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioI2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioTrk2T).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioTrk2T)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioTrk2O).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioTrk2O)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSrc2Trk).size());
	for (auto [src, dst] : ACTION_DATA(midiSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(midiI2Trk).size());
	for (auto [src, dst] : ACTION_DATA(midiI2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(midiTrk2O).size());
	for (auto [src, dst] : ACTION_DATA(midiTrk2O)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Prepare Track State */
		auto state = std::make_unique<vsp4::MixerTrack>();
		if (!state->ParseFromArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize())) {
			ACTION_RESULT(false);
		}

		/** Add Track */
		graph->insertTrack(ACTION_DATA(index),
			utils::getChannelSet(static_cast<utils::TrackType>(state->type())));
		
		/** Recover Track State */
		auto track = graph->getTrackProcessor(ACTION_DATA(index));
		graph->setTrackBypass(ACTION_DATA(index), state->bypassed());
		track->parse(state.get());

		/** Recover Connections */
		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioT2Trk)) {
			graph->setAudioTrk2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioSrc2Trk)) {
			graph->setAudioSrc2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioInstr2Trk)) {
			graph->setAudioInstr2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioI2Trk)) {
			graph->setAudioI2TrkConnection(dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioTrk2T)) {
			graph->setAudioTrk2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioTrk2O)) {
			graph->setAudioTrk2OConnection(src, srcc, dstc);
		}

		for (auto [src, dst] : ACTION_DATA(midiI2Trk)) {
			graph->setMIDII2TrkConnection(dst);
		}

		for (auto [src, dst] : ACTION_DATA(midiSrc2Trk)) {
			graph->setMIDISrc2TrkConnection(src, dst);
		}

		for (auto [src, dst] : ACTION_DATA(midiTrk2O)) {
			graph->setMIDITrk2OConnection(src);
		}

		juce::String result;
		result += "Undo Remove Mixer Track: " + juce::String(ACTION_DATA(index)) + "\n";
		result += "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n";
		this->output(result);
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveMixerTrackSend::ActionRemoveMixerTrackSend(
	int src, int srcc, int dst, int dstc)
	: ACTION_DB{ src, srcc, dst, dstc } {}

bool ActionRemoveMixerTrackSend::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveMixerTrackSend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isAudioTrk2TrkConnected(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			ACTION_RESULT(false);
		}

		graph->removeAudioTrk2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output(juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveMixerTrackSend::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveMixerTrackSend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveMixerTrackInputFromDevice::ActionRemoveMixerTrackInputFromDevice(
	int srcc, int dst, int dstc)
	: ACTION_DB{ srcc, dst, dstc } {}

bool ActionRemoveMixerTrackInputFromDevice::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveMixerTrackInputFromDevice);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isAudioI2TrkConnected(
			ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			ACTION_RESULT(false);
		}

		graph->removeAudioI2TrkConnection(
			ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("[Device] " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveMixerTrackInputFromDevice::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveMixerTrackInputFromDevice);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioI2TrkConnection(
			ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo [Device] " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveMixerTrackOutput::ActionRemoveMixerTrackOutput(
	int src, int srcc, int dstc)
	: ACTION_DB{ src, srcc, dstc } {}

bool ActionRemoveMixerTrackOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveMixerTrackOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isAudioTrk2OConnected(
			ACTION_DATA(src), ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			ACTION_RESULT(false);
		}

		graph->removeAudioTrk2OConnection(
			ACTION_DATA(src), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output(juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + "[Device] " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveMixerTrackOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveMixerTrackOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2OConnection(
			ACTION_DATA(src), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + "[Device] " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveEffect::ActionRemoveEffect(
	int track, int effect)
	: ACTION_DB{ track, effect } {}

bool ActionRemoveEffect::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveEffect);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(additional).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(additional)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto dock = track->getPluginDock()) {
				/** Check Effect */
				if (ACTION_DATA(effect) < 0 || ACTION_DATA(effect) >= dock->getPluginNum()) { ACTION_RESULT(false); }

				/** Save Connections */
				ACTION_DATA(additional) = dock->getPluginAdditionalBusConnections(ACTION_DATA(effect));

				/** Save Effect State */
				auto effect = dock->getPluginProcessor(ACTION_DATA(effect));
				if (!effect) { ACTION_RESULT(false); }
				auto state = effect->serialize();

				auto statePtr = dynamic_cast<vsp4::Plugin*>(state.get());
				if (!statePtr) { ACTION_RESULT(false); }
				statePtr->set_bypassed(dock->getPluginBypass(ACTION_DATA(effect)));

				ACTION_DATA(data).setSize(state->ByteSizeLong());
				state->SerializeToArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize());

				/** Remove Effect */
				dock->removePlugin(ACTION_DATA(effect));

				this->output("Remove Plugin: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "]" + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveEffect::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change effect while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveEffect);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(additional).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(additional)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto dock = track->getPluginDock()) {
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
				effect->parse(state.get());

				/** Recover Connections */
				for (auto [src, srcc, dst, dstc] : ACTION_DATA(additional)) {
					dock->addAdditionalBusConnection(dst, srcc, dstc);
				}

				this->output("Undo Remove Plugin: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "]" + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveEffectAdditionalInput::ActionRemoveEffectAdditionalInput(
	int track, int effect, int srcc, int dstc)
	: ACTION_DB{ track, effect, srcc, dstc } {}

bool ActionRemoveEffectAdditionalInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveEffectAdditionalInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				if (!pluginDock->isAdditionalBusConnected(
					ACTION_DATA(effect), ACTION_DATA(srcc), ACTION_DATA(dstc))) {
					ACTION_RESULT(false);
				}

				pluginDock->removeAdditionalBusConnection(ACTION_DATA(effect), ACTION_DATA(srcc), ACTION_DATA(dstc));

				this->output("Unlink Plugin Channel: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dstc)) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveEffectAdditionalInput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveEffectAdditionalInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->addAdditionalBusConnection(ACTION_DATA(effect), ACTION_DATA(srcc), ACTION_DATA(dstc));

				this->output("Undo Unlink Plugin Channel: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dstc)) + "\n");
				ACTION_RESULT(true);
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

	writeRecoverySizeValue(ACTION_DATA(audioInstr2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioInstr2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSrc2Instr).size());
	for (auto [src, dst] : ACTION_DATA(midiSrc2Instr)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(midiI2Instr).size());
	for (auto [src, dst] : ACTION_DATA(midiI2Instr)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Check Instr */
		if (ACTION_DATA(index) < 0 || ACTION_DATA(index) >= graph->getInstrumentNum()) { ACTION_RESULT(false); }

		/** Save Connections */
		ACTION_DATA(audioInstr2Trk) = graph->getInstrOutputToTrackConnections(ACTION_DATA(index));
		ACTION_DATA(midiSrc2Instr) = graph->getInstrMidiInputFromSrcConnections(ACTION_DATA(index));
		ACTION_DATA(midiI2Instr) = graph->getInstrMidiInputFromDeviceConnections(ACTION_DATA(index));

		/** Save Instr State */
		auto instr = graph->getInstrumentProcessor(ACTION_DATA(index));
		if (!instr) { ACTION_RESULT(false); }
		auto state = instr->serialize();

		auto statePtr = dynamic_cast<vsp4::Plugin*>(state.get());
		if (!statePtr) { ACTION_RESULT(false); }
		statePtr->set_bypassed(graph->getInstrumentBypass(ACTION_DATA(index)));

		ACTION_DATA(data).setSize(state->ByteSizeLong());
		state->SerializeToArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize());

		/** Remove Instr */
		graph->removeInstrument(ACTION_DATA(index));

		this->output("Remove Instrument: [" + juce::String(ACTION_DATA(index)) + "]" + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveInstr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change instrument while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveInstr);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(audioInstr2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioInstr2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSrc2Instr).size());
	for (auto [src, dst] : ACTION_DATA(midiSrc2Instr)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(midiI2Instr).size());
	for (auto [src, dst] : ACTION_DATA(midiI2Instr)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Prepare Instr State */
		auto state = std::make_unique<vsp4::Plugin>();
		if (!state->ParseFromArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize())) {
			ACTION_RESULT(false);
		}

		/** Add Instr */
		graph->insertInstrument(ACTION_DATA(index),
			utils::getChannelSet(static_cast<utils::TrackType>(state->info().decoratortype())));

		/** Recover Instr State */
		auto instr = graph->getInstrumentProcessor(ACTION_DATA(index));
		graph->setInstrumentBypass(ACTION_DATA(index), state->bypassed());
		instr->parse(state.get());

		/** Recover Connections */
		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioInstr2Trk)) {
			graph->setAudioInstr2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, dst] : ACTION_DATA(midiSrc2Instr)) {
			graph->setMIDISrc2InstrConnection(src, dst);
		}

		for (auto [src, dst] : ACTION_DATA(midiI2Instr)) {
			graph->setMIDII2InstrConnection(dst);
		}

		this->output("Undo Remove Instrument: [" + juce::String(ACTION_DATA(index)) + "]" + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveInstrOutput::ActionRemoveInstrOutput(
	int src, int srcc, int dst, int dstc)
	: ACTION_DB{ src, srcc, dst, dstc } {}

bool ActionRemoveInstrOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveInstrOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isAudioInstr2TrkConnected(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			ACTION_RESULT(false);
		}

		graph->removeAudioInstr2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output(juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}

	ACTION_RESULT(false);
}

bool ActionRemoveInstrOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveInstrOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioInstr2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}

	ACTION_RESULT(false);
}

ActionRemoveInstrMidiInput::ActionRemoveInstrMidiInput(int dst)
	: ACTION_DB{ dst } {}

bool ActionRemoveInstrMidiInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveInstrMidiInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isMIDII2InstrConnected(ACTION_DATA(dst))) {
			ACTION_RESULT(false);
		}

		graph->removeMIDII2InstrConnection(ACTION_DATA(dst));

		this->output(juce::String("[MIDI Input]") + " - " + juce::String(ACTION_DATA(dst)) + " (Removed)\n");
		ACTION_RESULT(true);
	}

	ACTION_RESULT(false);
}

bool ActionRemoveInstrMidiInput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveInstrMidiInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDII2InstrConnection(ACTION_DATA(dst));

		this->output(juce::String("Undo [MIDI Input]") + " - " + juce::String(ACTION_DATA(dst)) + " (Removed)\n");
		ACTION_RESULT(true);
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
		if (auto instr = graph->getInstrumentProcessor(ACTION_DATA(instr))) {
			ACTION_DATA(param) = instr->getCCParamConnection(ACTION_DATA(cc));

			instr->removeCCParamConnection(ACTION_DATA(cc));

			this->output("Remove Instr Param MIDI CC Connection: " "MIDI CC " + juce::String(ACTION_DATA(cc)) + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveInstrParamCCConnection::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveInstrParamCCConnection);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto instr = graph->getInstrumentProcessor(ACTION_DATA(instr))) {
			instr->connectParamCC(ACTION_DATA(param), ACTION_DATA(cc));

			this->output("Undo Remove Instr Param MIDI CC Connection: " "MIDI CC " + juce::String(ACTION_DATA(cc)) + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveEffectParamCCConnection::ActionRemoveEffectParamCCConnection(
	int track, int effect, int cc)
	: ACTION_DB{ track, effect, cc } {}

bool ActionRemoveEffectParamCCConnection::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveEffectParamCCConnection);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
					ACTION_DATA(param) = effect->getCCParamConnection(ACTION_DATA(cc));

					effect->removeCCParamConnection(ACTION_DATA(cc));

					this->output("Remove Effect Param MIDI CC Connection: " "MIDI CC " + juce::String(ACTION_DATA(cc)) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveEffectParamCCConnection::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveEffectParamCCConnection);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
					effect->connectParamCC(ACTION_DATA(param), ACTION_DATA(cc));

					this->output("Undo Remove Effect Param MIDI CC Connection: " "MIDI CC " + juce::String(ACTION_DATA(cc)) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveMixerTrackMidiInput::ActionRemoveMixerTrackMidiInput(int index)
	: ACTION_DB{ index } {}

bool ActionRemoveMixerTrackMidiInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveMixerTrackMidiInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isMIDII2TrkConnected(ACTION_DATA(index))) {
			ACTION_RESULT(false);
		}

		graph->removeMIDII2TrkConnection(ACTION_DATA(index));

		this->output(juce::String("[MIDI Input]") + " - " + juce::String(ACTION_DATA(index)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveMixerTrackMidiInput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveMixerTrackMidiInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDII2TrkConnection(ACTION_DATA(index));

		this->output(juce::String("Undo [MIDI Input]") + " - " + juce::String(ACTION_DATA(index)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveMixerTrackMidiOutput::ActionRemoveMixerTrackMidiOutput(int index)
	: ACTION_DB{ index } {}

bool ActionRemoveMixerTrackMidiOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveMixerTrackMidiOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isMIDITrk2OConnected(ACTION_DATA(index))) {
			ACTION_RESULT(false);
		}

		graph->removeMIDITrk2OConnection(ACTION_DATA(index));

		this->output(juce::String(ACTION_DATA(index)) + " - " + "[MIDI Output]" + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveMixerTrackMidiOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveMixerTrackMidiOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDITrk2OConnection(ACTION_DATA(index));

		this->output("Undo " + juce::String(ACTION_DATA(index)) + " - " + "[MIDI Output]" + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveSource::ActionRemoveSource(int index)
	: ACTION_DB{ index } {}

bool ActionRemoveSource::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveSource);
	ACTION_WRITE_DB();

	if (auto manager = CloneableSourceManager::getInstance()) {
		if (auto src = manager->getSource(ACTION_DATA(index))) {
			if (AudioIOList::getInstance()->isTask(src)) {
				this->error("Unavailable source status!");
				ACTION_RESULT(false);
			}
			if (auto p = dynamic_cast<CloneableSynthSource*>(src.getSource())) {
				if (p->isSynthRunning()) {
					this->error("Unavailable source status!");
					ACTION_RESULT(false);
				}
			}
		}

		manager->removeSource(ACTION_DATA(index));
		this->output("Total Source Num: " + juce::String(manager->getSourceNum()) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveSequencerTrack::ActionRemoveSequencerTrack(int index)
	: ACTION_DB{ index } {}

bool ActionRemoveSequencerTrack::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveSequencerTrack);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(audioSrc2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSrc2Instr).size());
	for (auto [src, dst] : ACTION_DATA(midiSrc2Instr)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSrc2Trk).size());
	for (auto [src, dst] : ACTION_DATA(midiSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Check Track */
		if (ACTION_DATA(index) < 0 || ACTION_DATA(index) >= graph->getSourceNum()) { ACTION_RESULT(false); }

		/** Save Connections */
		ACTION_DATA(audioSrc2Trk) = graph->getSourceOutputToTrackConnections(ACTION_DATA(index));
		ACTION_DATA(midiSrc2Instr) = graph->getSourceMidiOutputToInstrConnections(ACTION_DATA(index));
		ACTION_DATA(midiSrc2Trk) = graph->getSourceMidiOutputToTrackConnections(ACTION_DATA(index));

		/** Save Track State */
		auto track = graph->getSourceProcessor(ACTION_DATA(index));
		if (!track) { ACTION_RESULT(false); }
		auto state = track->serialize();

		auto statePtr = dynamic_cast<vsp4::SeqTrack*>(state.get());
		if (!statePtr) { ACTION_RESULT(false); }
		statePtr->set_bypassed(graph->getSourceBypass(ACTION_DATA(index)));

		ACTION_DATA(data).setSize(state->ByteSizeLong());
		state->SerializeToArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize());

		/** Remove Track */
		graph->removeSource(ACTION_DATA(index));

		juce::String result;
		result += "Remove Sequencer Track: " + juce::String(ACTION_DATA(index)) + "\n";
		result += "Total Sequencer Track Num: " + juce::String(graph->getSourceNum()) + "\n";
		ACTION_RESULT(true);
	}

	ACTION_RESULT(false);
}

bool ActionRemoveSequencerTrack::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveSequencerTrack);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(audioSrc2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSrc2Instr).size());
	for (auto [src, dst] : ACTION_DATA(midiSrc2Instr)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSrc2Trk).size());
	for (auto [src, dst] : ACTION_DATA(midiSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Prepare Track State */
		auto state = std::make_unique<vsp4::SeqTrack>();
		if (!state->ParseFromArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize())) {
			ACTION_RESULT(false);
		}

		/** Add Track */
		graph->insertSource(ACTION_DATA(index),
			utils::getChannelSet(static_cast<utils::TrackType>(state->type())));

		/** Recover Track State */
		auto track = graph->getSourceProcessor(ACTION_DATA(index));
		graph->setSourceBypass(ACTION_DATA(index), state->bypassed());
		track->parse(state.get());

		/** Recover Connections */
		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioSrc2Trk)) {
			graph->setAudioSrc2TrkConnection(src, dst, srcc, dstc);
		}
		
		for (auto [src, dst] : ACTION_DATA(midiSrc2Instr)) {
			graph->setMIDISrc2InstrConnection(src, dst);
		}

		for (auto [src, dst] : ACTION_DATA(midiSrc2Trk)) {
			graph->setMIDISrc2TrkConnection(src, dst);
		}

		juce::String result;
		result += "Undo Remove Sequencer Track: " + juce::String(ACTION_DATA(index)) + "\n";
		result += "Total Sequencer Track Num: " + juce::String(graph->getSourceNum()) + "\n";
		this->output(result);
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveSequencerTrackMidiOutputToMixer::ActionRemoveSequencerTrackMidiOutputToMixer(
	int src, int dst)
	: ACTION_DB{ src, dst } {}

bool ActionRemoveSequencerTrackMidiOutputToMixer::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveSequencerTrackMidiOutputToMixer);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isMIDISrc2TrkConnected(ACTION_DATA(src), ACTION_DATA(dst))) {
			ACTION_RESULT(false);
		}

		graph->removeMIDISrc2TrkConnection(ACTION_DATA(src), ACTION_DATA(dst));

		this->output(juce::String(ACTION_DATA(src)) + " - " + juce::String(ACTION_DATA(dst)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveSequencerTrackMidiOutputToMixer::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveSequencerTrackMidiOutputToMixer);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDISrc2TrkConnection(ACTION_DATA(src), ACTION_DATA(dst));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + " - " + juce::String(ACTION_DATA(dst)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveSequencerTrackMidiOutputToInstr::ActionRemoveSequencerTrackMidiOutputToInstr(
	int src, int dst)
	: ACTION_DB{ src, dst } {}

bool ActionRemoveSequencerTrackMidiOutputToInstr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveSequencerTrackMidiOutputToInstr);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isMIDISrc2InstrConnected(ACTION_DATA(src), ACTION_DATA(dst))) {
			ACTION_RESULT(false);
		}

		graph->removeMIDISrc2InstrConnection(ACTION_DATA(src), ACTION_DATA(dst));

		this->output(juce::String(ACTION_DATA(src)) + " - " + juce::String(ACTION_DATA(dst)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveSequencerTrackMidiOutputToInstr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveSequencerTrackMidiOutputToInstr);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDISrc2InstrConnection(ACTION_DATA(src), ACTION_DATA(dst));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + " - " + juce::String(ACTION_DATA(dst)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveSequencerTrackOutput::ActionRemoveSequencerTrackOutput(
	int src, int srcc, int dst, int dstc)
	: ACTION_DB{ src, srcc, dst, dstc } {}

bool ActionRemoveSequencerTrackOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveSequencerTrackOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isAudioSrc2TrkConnected(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			ACTION_RESULT(false);
		}

		graph->removeAudioSrc2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output(juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveSequencerTrackOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveSequencerTrackOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioSrc2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveSequencerSourceInstance::ActionRemoveSequencerSourceInstance(
	int track, int seq)
	: ACTION_DB{ track, seq } {}

bool ActionRemoveSequencerSourceInstance::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveSequencerSourceInstance);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto seqTrack = graph->getSourceProcessor(ACTION_DATA(track))) {
			if (ACTION_DATA(seq) < 0 || ACTION_DATA(seq) >= seqTrack->getSeqNum()) {
				ACTION_RESULT(false);
			}

			std::tie(ACTION_DATA(start), ACTION_DATA(end), ACTION_DATA(offset), std::ignore, ACTION_DATA(index))
				= seqTrack->getSeq(ACTION_DATA(seq));

			seqTrack->removeSeq(ACTION_DATA(seq));

			juce::String result;
			result += "Remove Sequencer Source Instance [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(seq)) + "]\n";
			result += "Total Sequencer Source Instance: " + juce::String(seqTrack->getSeqNum()) + "\n";
			this->output(result);
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveSequencerSourceInstance::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveSequencerSourceInstance);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto seqTrack = graph->getSourceProcessor(ACTION_DATA(track))) {
			auto ptrSrc = CloneableSourceManager::getInstance()->getSource(ACTION_DATA(index));
			if (!ptrSrc) { ACTION_RESULT(false); }

			seqTrack->addSeq({ ACTION_DATA(start), ACTION_DATA(end), ACTION_DATA(offset), ptrSrc, ACTION_DATA(index) });

			juce::String result;
			result += "Undo Remove Sequencer Source Instance [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(seq)) + "]\n";
			result += "Total Sequencer Source Instance: " + juce::String(seqTrack->getSeqNum()) + "\n";
			this->output(result);
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveRecorderSourceInstance::ActionRemoveRecorderSourceInstance(int seq)
	: ACTION_DB{ seq } {}

bool ActionRemoveRecorderSourceInstance::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveRecorderSourceInstance);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto recorder = graph->getRecorder()) {
			if (ACTION_DATA(seq) < 0 || ACTION_DATA(seq) >= recorder->getTaskNum()) {
				ACTION_RESULT(false);
			}

			std::tie(std::ignore, ACTION_DATA(index), ACTION_DATA(offset), ACTION_DATA(compensate))
				= recorder->getTask(ACTION_DATA(seq));

			recorder->removeTask(ACTION_DATA(seq));

			juce::String result;
			result += "Remove Recorder Source Instance [" + juce::String(ACTION_DATA(seq)) + "]\n";
			result += "Total Recorder Source Instance: " + juce::String(recorder->getTaskNum()) + "\n";
			this->output(result);
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveRecorderSourceInstance::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveRecorderSourceInstance);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto recorder = graph->getRecorder()) {
			auto ptrSrc = CloneableSourceManager::getInstance()->getSource(ACTION_DATA(index));
			if (!ptrSrc) { ACTION_RESULT(false); }

			recorder->insertTask({ ptrSrc, ACTION_DATA(index), ACTION_DATA(offset), ACTION_DATA(compensate) }, ACTION_DATA(seq));

			juce::String result;
			result += "Undo Remove Recorder Source Instance [" + juce::String(ACTION_DATA(seq)) + "]\n";
			result += "Total Recorder Source Instance: " + juce::String(recorder->getTaskNum()) + "\n";
			this->output(result);
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

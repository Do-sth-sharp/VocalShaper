#include "ActionAdd.h"

#include "../AudioCore.h"
#include "../plugin/Plugin.h"
#include "../plugin/PluginLoader.h"
#include "../misc/PlayPosition.h"
#include "../Utils.h"

ActionAddPluginBlackList::ActionAddPluginBlackList(const juce::String& plugin)
	: plugin(plugin) {}

bool ActionAddPluginBlackList::doAction() {
	ACTION_CHECK_PLUGIN_LOADING(
		"Don't do this while loading plugin.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change plugin black list while searching plugin.");

	Plugin::getInstance()->addToPluginBlackList(this->plugin);
	this->output("Add to plugin black list.");
	return true;
}

ActionAddPluginSearchPath::ActionAddPluginSearchPath(const juce::String& path)
	: path(path) {}

bool ActionAddPluginSearchPath::doAction() {
	ACTION_CHECK_PLUGIN_LOADING(
		"Don't do this while loading plugin.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change plugin search path while searching plugin.");

	Plugin::getInstance()->addToPluginSearchPath(this->path);
	this->output("Add to plugin search path.");
	return true;
}

ActionAddTrack::ActionAddTrack(
	quickAPI::TrackIndex index, int bus)
	: ACTION_DB{ index, bus } {}

bool ActionAddTrack::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddTrack);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		juce::AudioChannelSet busType = utils::getChannelSet(
			static_cast<utils::BusType>(ACTION_DATA(bus)));

		ACTION_DATA(newIndex) = graph->insertTrack(
			ACTION_DATA(index).first, ACTION_DATA(index).second, busType);

		if (ACTION_DATA(newIndex) >= 0) {
			this->output("Add Track: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "]" + busType.getDescription() + "\n"
				+ "Total Track Num: " + juce::String{ graph->getTrackNum(ACTION_DATA(index).first) } + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionAddTrack::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();
	ACTION_WRITE_DB();

	ACTION_WRITE_TYPE_UNDO(ActionAddTrack);

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeTrack(ACTION_DATA(index).first, ACTION_DATA(newIndex));

		this->output(juce::String{ "Undo Add Track.\n" }
		+ "Total Track Num: " + juce::String{ graph->getTrackNum(ACTION_DATA(index).first) } + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddTrackAudioInput::ActionAddTrackAudioInput(
	quickAPI::TrackIndex index,
	int srcc, int dstc)
	: ACTION_DB{ index, srcc, dstc } {}

bool ActionAddTrackAudioInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddTrackAudioInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->connectTrackAudioInput(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			this->output("Connect Audio Input: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "] " + juce::String{ ACTION_DATA(srcc) } + " - " + juce::String{ ACTION_DATA(dstc) } + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionAddTrackAudioInput::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddTrackAudioInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->disconnectTrackAudioInput(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo Connect Audio Input: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "] " + juce::String{ ACTION_DATA(srcc) } + " - " + juce::String{ ACTION_DATA(dstc) } + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddTrackMIDIInput::ActionAddTrackMIDIInput(
	quickAPI::TrackIndex index)
	: ACTION_DB{ index } {}

bool ActionAddTrackMIDIInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddTrackMIDIInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->connectTrackMIDIInput(
			ACTION_DATA(index).first, ACTION_DATA(index).second)) {
			this->output("Connect MIDI Input: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "]" + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionAddTrackMIDIInput::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddTrackMIDIInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->disconnectTrackMIDIInput(
			ACTION_DATA(index).first, ACTION_DATA(index).second);

		this->output("Undo Connect MIDI Input: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "]" + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddTrackAudioSend::ActionAddTrackAudioSend(
	quickAPI::TrackIndex index, int slot,
	quickAPI::SendDst dst, int srcc, int dstc)
	: ACTION_DB{ index, slot, dst, srcc, dstc } {}

bool ActionAddTrackAudioSend::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddTrackAudioSend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->connectTrackAudioSend(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot),
			ACTION_DATA(dst).first, ACTION_DATA(dst).second, ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			this->output("Connect Audio Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + juce::String{ ACTION_DATA(srcc) } + " - " + juce::String{ ACTION_DATA(dstc) } + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionAddTrackAudioSend::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddTrackAudioSend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->disconnectTrackAudioSend(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot),
			ACTION_DATA(dst).first, ACTION_DATA(dst).second, ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo Connect Audio Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + juce::String{ ACTION_DATA(srcc) } + " - " + juce::String{ ACTION_DATA(dstc) } + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddTrackMIDISend::ActionAddTrackMIDISend(
	quickAPI::TrackIndex index, int slot,
	quickAPI::SendDst dst)
	: ACTION_DB{ index, slot, dst } {
}

bool ActionAddTrackMIDISend::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddTrackMIDISend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (graph->connectTrackMIDISend(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot),
			ACTION_DATA(dst).first, ACTION_DATA(dst).second)) {
			this->output("Connect MIDI Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionAddTrackMIDISend::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddTrackMIDISend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->disconnectTrackMIDISend(
			ACTION_DATA(index).first, ACTION_DATA(index).second, ACTION_DATA(slot),
			ACTION_DATA(dst).first, ACTION_DATA(dst).second);

		this->output("Undo Connect MIDI Send: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + " : " + juce::String{ ACTION_DATA(slot) } + "] " + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddEffect::ActionAddEffect(
	quickAPI::TrackIndex trackIndex,
	int effect, const juce::String& pid)
	: ACTION_DB{ trackIndex, effect, pid } {}

bool ActionAddEffect::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change effect while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddEffect);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(pid);

	if (auto des = Plugin::getInstance()->findPlugin(ACTION_DATA(pid), false)) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(ACTION_DATA(trackIndex).first, ACTION_DATA(trackIndex).second)) {
				if (auto mixer = track->getMixer()) {
					if (auto pluginDock = mixer->getPluginDock()) {
						if (auto ptr = pluginDock->insertPlugin(ACTION_DATA(effect))) {
							PluginLoader::getInstance()->loadPlugin(*(des.get()), false, ptr);

							this->output("Insert Plugin: [" + juce::String{ (int)(ACTION_DATA(trackIndex).first) } + ", " + juce::String{ ACTION_DATA(trackIndex).second } + ", " + juce::String(ACTION_DATA(effect)) + "] " + ACTION_DATA(pid) + "\n");
							ACTION_RESULT(true);
						}
					}
				}
			}
		}
	}

	this->error("Can't Insert Plugin: [" + juce::String{ (int)(ACTION_DATA(trackIndex).first) } + ", " + juce::String{ ACTION_DATA(trackIndex).second } + ", " + juce::String(ACTION_DATA(effect)) + "] " + ACTION_DATA(pid) + "\n");
	ACTION_RESULT(false);
}

bool ActionAddEffect::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddEffect);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(pid);

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(trackIndex).first, ACTION_DATA(trackIndex).second)) {
			if (auto mixer = track->getMixer()) {
				if (auto pluginDock = mixer->getPluginDock()) {
					pluginDock->removePlugin(ACTION_DATA(effect));

					this->output("Undo Insert Plugin: [" + juce::String{ (int)(ACTION_DATA(trackIndex).first) } + ", " + juce::String{ ACTION_DATA(trackIndex).second } + ", " + juce::String(ACTION_DATA(effect)) + "] " + ACTION_DATA(pid) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}

	this->error("Can't Undo Insert Plugin: [" + juce::String{ (int)(ACTION_DATA(trackIndex).first) } + ", " + juce::String{ ACTION_DATA(trackIndex).second } + ", " + juce::String(ACTION_DATA(effect)) + "] " + ACTION_DATA(pid) + "\n");
	ACTION_RESULT(false);
}

ActionAddInstr::ActionAddInstr(
	int index, const juce::String& pid, bool addARA)
	: ACTION_DB{ index, pid, addARA } {}

bool ActionAddInstr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change instrument while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddInstr);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(pid);

	if (auto des = Plugin::getInstance()->findPlugin(ACTION_DATA(pid), true, ACTION_DATA(addARA))) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(index))) {
				if (auto seq = track->getSequencer()) {
					if (auto ptr = seq->prepareInstr()) {
						PluginLoader::getInstance()->loadPlugin(*(des.get()), ACTION_DATA(addARA), ptr);

						this->output("Insert Plugin: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + "\n");
						ACTION_RESULT(true);
					}
				}
			}
		}
	}

	this->error("Can't Insert Plugin: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + "\n");
	ACTION_RESULT(false);
}

bool ActionAddInstr::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddInstr);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(pid);

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(index))) {
			if (auto seq = track->getSequencer()) {
				seq->removeInstr();

				this->output("Undo Insert Plugin: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	
	this->error("Can't Undo Insert Plugin: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + "\n");
	ACTION_RESULT(false);
}

ActionAddTrackSideChainBus::ActionAddTrackSideChainBus(
	quickAPI::TrackIndex index) : ACTION_DB{ index } {}

bool ActionAddTrackSideChainBus::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddTrackSideChainBus);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(index).first, ACTION_DATA(index).second)) {
			if (track->addAdditionalAudioBus()) {
				this->output("Add mixer track side chain bus: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "] \n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionAddTrackSideChainBus::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddTrackSideChainBus);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(index).first, ACTION_DATA(index).second)) {
			if (track->removeAdditionalAudioBus()) {
				this->output("Undo add mixer track side chain bus: [" + juce::String{ (int)(ACTION_DATA(index).first) } + ", " + juce::String{ ACTION_DATA(index).second } + "] \n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionAddSequencerBlock::ActionAddSequencerBlock(
	int trackIndex, double startTime, double endTime, double offset)
	: ACTION_DB{ trackIndex, startTime, endTime, offset } {}

bool ActionAddSequencerBlock::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddSequencerBlock);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(trackIndex))) {
			if (auto seq = track->getSequencer()) {
				ACTION_DATA(index) = seq->addSeq(
					{ ACTION_DATA(startTime), ACTION_DATA(endTime), ACTION_DATA(offset) });

				this->output("Add sequencer block [" + juce::String(ACTION_DATA(trackIndex)) + "]\n"
					+ "Total sequencer blocks: " + juce::String(seq->getSeqNum()) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	this->output("Can't add sequencer block [" + juce::String(ACTION_DATA(trackIndex)) + "]\n");
	ACTION_RESULT(false);
}

bool ActionAddSequencerBlock::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddSequencerBlock);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, ACTION_DATA(trackIndex))) {
			if (auto seq = track->getSequencer()) {
				seq->removeSeq(ACTION_DATA(index));

				this->output("Undo add sequencer block [" + juce::String(ACTION_DATA(trackIndex)) + "]\n"
					+ "Total sequencer blocks: " + juce::String(seq->getSeqNum()) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	this->output("Can't undo add sequencer block [" + juce::String(ACTION_DATA(trackIndex)) + "]\n");
	ACTION_RESULT(false);
}

ActionAddTempoTempo::ActionAddTempoTempo(
	double time, double tempo)
	: ACTION_DB{ time, tempo } {}

bool ActionAddTempoTempo::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddTempoTempo);
	ACTION_WRITE_DB();

	ACTION_DATA(index) = PlayPosition::getInstance()
		->addTempoLabelTempo(ACTION_DATA(time), ACTION_DATA(tempo), ACTION_DATA(index));
	ACTION_RESULT(true);
}

bool ActionAddTempoTempo::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddTempoTempo);
	ACTION_WRITE_DB();

	PlayPosition::getInstance()->removeTempoLabel(ACTION_DATA(index));
	ACTION_RESULT(true);
}

ActionAddTempoBeat::ActionAddTempoBeat(
	double time, int numerator, int denominator)
	: ACTION_DB{ time, numerator, denominator } {}

bool ActionAddTempoBeat::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddTempoBeat);
	ACTION_WRITE_DB();

	ACTION_DATA(index) = PlayPosition::getInstance()
		->addTempoLabelBeat(ACTION_DATA(time),
			ACTION_DATA(numerator), ACTION_DATA(denominator), ACTION_DATA(index));
	ACTION_RESULT(true);
}

bool ActionAddTempoBeat::undoAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddTempoBeat);
	ACTION_WRITE_DB();

	PlayPosition::getInstance()->removeTempoLabel(ACTION_DATA(index));
	ACTION_RESULT(true);
}

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

ActionAddMixerTrack::ActionAddMixerTrack(int index, int type)
	: ACTION_DB{ index, type } {}

bool ActionAddMixerTrack::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddMixerTrack);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		juce::AudioChannelSet trackType = utils::getChannelSet(
			static_cast<utils::TrackType>(ACTION_DATA(type)));

		graph->insertTrack(ACTION_DATA(index), trackType);

		this->output("Add Mixer Track: [" + juce::String(ACTION_DATA(type)) + "]" + trackType.getDescription() + "\n"
			+ "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionAddMixerTrack::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();
	ACTION_WRITE_DB();

	ACTION_WRITE_TYPE_UNDO(ActionAddMixerTrack);

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeTrack((ACTION_DATA(index) > -1 && ACTION_DATA(index) < graph->getTrackNum())
			? ACTION_DATA(index) : (graph->getTrackNum() - 1));

		this->output(juce::String{ "Undo Add Mixer Track.\n" }
			+ "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddMixerTrackSend::ActionAddMixerTrackSend(
	int src, int srcc, int dst, int dstc)
	: ACTION_DB{ src, srcc, dst, dstc } {}

bool ActionAddMixerTrackSend::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddMixerTrackSend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output(juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionAddMixerTrackSend::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddMixerTrackSend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioTrk2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddMixerTrackInputFromDevice::ActionAddMixerTrackInputFromDevice(
	int srcc, int dst, int dstc)
	: ACTION_DB{ srcc, dst, dstc } {}

bool ActionAddMixerTrackInputFromDevice::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddMixerTrackInputFromDevice);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioI2TrkConnection(
			ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("[Device] " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionAddMixerTrackInputFromDevice::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddMixerTrackInputFromDevice);

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioI2TrkConnection(
			ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo [Device] " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddMixerTrackOutput::ActionAddMixerTrackOutput(
	int src, int srcc, int dstc)
	: ACTION_DB{ src, srcc, dstc } {}

bool ActionAddMixerTrackOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddMixerTrackOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2OConnection(
			ACTION_DATA(src), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output(juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + "[Device] " + juce::String(ACTION_DATA(dstc)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionAddMixerTrackOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddMixerTrackOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioTrk2OConnection(
			ACTION_DATA(src), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + "[Device] " + juce::String(ACTION_DATA(dstc)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddEffect::ActionAddEffect(
	int track, int effect, const juce::String& pid)
	: ACTION_DB{ track, effect, pid } {}

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
			if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
				if (auto pluginDock = track->getPluginDock()) {
					if (auto ptr = pluginDock->insertPlugin(ACTION_DATA(effect))) {
						PluginLoader::getInstance()->loadPlugin(*(des.get()), ptr);

						this->output("Insert Plugin: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + ACTION_DATA(pid) + "\n");
						ACTION_RESULT(true);
					}
				}
			}
		}
	}

	this->error("Can't Insert Plugin: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + ACTION_DATA(pid) + "\n");
	ACTION_RESULT(false);
}

bool ActionAddEffect::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddEffect);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(pid);

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->removePlugin((ACTION_DATA(effect) > -1 && ACTION_DATA(effect) < pluginDock->getPluginNum())
					? ACTION_DATA(effect) : (pluginDock->getPluginNum() - 1));

				this->output("Undo Insert Plugin: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + ACTION_DATA(pid) + "\n");
				ACTION_RESULT(true);
			}
		}
	}

	this->error("Can't Undo Insert Plugin: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + ACTION_DATA(pid) + "\n");
	ACTION_RESULT(false);
}

ActionAddInstr::ActionAddInstr(
	int index, const juce::String& pid)
	: ACTION_DB{ index, pid } {}

bool ActionAddInstr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change instrument while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddInstr);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(pid);

	if (auto des = Plugin::getInstance()->findPlugin(ACTION_DATA(pid), true)) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getSourceProcessor(ACTION_DATA(index))) {
				if (auto ptr = track->prepareInstr()) {
					PluginLoader::getInstance()->loadPlugin(*(des.get()), ptr);

					this->output("Insert Plugin: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}

	this->error("Can't Insert Plugin: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + "\n");
	ACTION_RESULT(false);
}

bool ActionAddInstr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddInstr);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(pid);

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(index))) {
			track->removeInstr();

			this->output("Undo Insert Plugin: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + "\n");
			ACTION_RESULT(true);
		}
	}
	
	this->error("Can't Undo Insert Plugin: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + "\n");
	ACTION_RESULT(false);
}

ActionAddMixerTrackMidiInput::ActionAddMixerTrackMidiInput(int dst)
	: ACTION_DB{ dst } {}

bool ActionAddMixerTrackMidiInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddMixerTrackMidiInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDII2TrkConnection(ACTION_DATA(dst));

		this->output(juce::String("[MIDI Input]") + " - " + juce::String(ACTION_DATA(dst)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionAddMixerTrackMidiInput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddMixerTrackMidiInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDII2TrkConnection(ACTION_DATA(dst));

		this->output(juce::String("Undo [MIDI Input]") + " - " + juce::String(ACTION_DATA(dst)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddMixerTrackMidiOutput::ActionAddMixerTrackMidiOutput(int src)
	: ACTION_DB{ src } {}

bool ActionAddMixerTrackMidiOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddMixerTrackMidiOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDITrk2OConnection(ACTION_DATA(src));

		this->output(juce::String(ACTION_DATA(src)) + " - " + "[MIDI Output]" + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionAddMixerTrackMidiOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddMixerTrackMidiOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDITrk2OConnection(ACTION_DATA(src));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + " - " + "[MIDI Output]" + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddMixerTrackSideChainBus::ActionAddMixerTrackSideChainBus(
	int track) : ACTION_DB{ track } {}

bool ActionAddMixerTrackSideChainBus::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddMixerTrackSideChainBus);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (track->addAdditionalAudioBus()) {
				this->output("Add mixer track side chain bus: " + juce::String(ACTION_DATA(track)));
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionAddMixerTrackSideChainBus::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddMixerTrackSideChainBus);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (track->removeAdditionalAudioBus()) {
				this->output("Undo add mixer track side chain bus: " + juce::String(ACTION_DATA(track)));
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionAddSequencerTrack::ActionAddSequencerTrack(
	int index, int type)
	: ACTION_DB{ index, type } {}

bool ActionAddSequencerTrack::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddSequencerTrack);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		juce::AudioChannelSet trackType = utils::getChannelSet(
			static_cast<utils::TrackType>(ACTION_DATA(type)));

		graph->insertSource(ACTION_DATA(index), trackType);

		this->output("Add Sequencer Track: [" + juce::String(ACTION_DATA(type)) + "]" + trackType.getDescription() + "\n"
			+ "Total Sequencer Track Num: " + juce::String(graph->getSourceNum()) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionAddSequencerTrack::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddSequencerTrack);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		juce::AudioChannelSet trackType = utils::getChannelSet(
			static_cast<utils::TrackType>(ACTION_DATA(type)));

		graph->removeSource((ACTION_DATA(index) > -1 && ACTION_DATA(index) < graph->getSourceNum())
			? ACTION_DATA(index) : (graph->getSourceNum() - 1));

		this->output("Undo Add Sequencer Track: [" + juce::String(ACTION_DATA(type)) + "]" + trackType.getDescription() + "\n"
			+ "Total Sequencer Track Num: " + juce::String(graph->getSourceNum()) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddSequencerTrackMidiOutputToMixer::ActionAddSequencerTrackMidiOutputToMixer(
	int src, int dst)
	: ACTION_DB{ src, dst } {}

bool ActionAddSequencerTrackMidiOutputToMixer::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddSequencerTrackMidiOutputToMixer);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDISrc2TrkConnection(ACTION_DATA(src), ACTION_DATA(dst));

		this->output(juce::String(ACTION_DATA(src)) + " - " + juce::String(ACTION_DATA(dst)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionAddSequencerTrackMidiOutputToMixer::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddSequencerTrackMidiOutputToMixer);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDISrc2TrkConnection(ACTION_DATA(src), ACTION_DATA(dst));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + " - " + juce::String(ACTION_DATA(dst)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddSequencerTrackOutput::ActionAddSequencerTrackOutput(
	int src, int srcc, int dst, int dstc)
	: ACTION_DB{ src, srcc, dst, dstc } {}

bool ActionAddSequencerTrackOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddSequencerTrackOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioSrc2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output(juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionAddSequencerTrackOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddSequencerTrackOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioSrc2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddSequencerBlock::ActionAddSequencerBlock(
	int seqIndex, double startTime, double endTime, double offset)
	: ACTION_DB{ seqIndex, startTime, endTime, offset } {}

bool ActionAddSequencerBlock::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddSequencerBlock);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(seqIndex))) {
			ACTION_DATA(index) = track->addSeq(
				{ ACTION_DATA(startTime), ACTION_DATA(endTime), ACTION_DATA(offset) });

			this->output("Add sequencer block [" + juce::String(ACTION_DATA(seqIndex)) + "]\n"
				+ "Total sequencer blocks: " + juce::String(track->getSeqNum()) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't add sequencer block [" + juce::String(ACTION_DATA(seqIndex)) + "]\n");
	ACTION_RESULT(false);
}

bool ActionAddSequencerBlock::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddSequencerBlock);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(seqIndex))) {
			track->removeSeq(ACTION_DATA(index));

			this->output("Undo add sequencer block [" + juce::String(ACTION_DATA(seqIndex)) + "]\n"
				+ "Total sequencer blocks: " + juce::String(track->getSeqNum()) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't undo add sequencer block [" + juce::String(ACTION_DATA(seqIndex)) + "]\n");
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
		->addTempoLabelTempo(ACTION_DATA(time), ACTION_DATA(tempo));
	ACTION_RESULT(true);
}

bool ActionAddTempoTempo::undo() {
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

bool ActionAddTempoBeat::ActionAddTempoBeat::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddTempoBeat);
	ACTION_WRITE_DB();

	ACTION_DATA(index) = PlayPosition::getInstance()
		->addTempoLabelBeat(ACTION_DATA(time),
			ACTION_DATA(numerator), ACTION_DATA(denominator));
	ACTION_RESULT(true);
}

bool ActionAddTempoBeat::ActionAddTempoBeat::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddTempoBeat);
	ACTION_WRITE_DB();

	PlayPosition::getInstance()->removeTempoLabel(ACTION_DATA(index));
	ACTION_RESULT(true);
}

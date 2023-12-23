#include "ActionAdd.h"
#include "ActionUtils.h"

#include "../AudioCore.h"
#include "../plugin/Plugin.h"
#include "../plugin/PluginLoader.h"
#include "../Utils.h"

#include "../source/CloneableSourceManagerTemplates.h"

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
	: index(index), type(type) {}

bool ActionAddMixerTrack::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		juce::AudioChannelSet trackType = utils::getChannelSet(
			static_cast<utils::TrackType>(this->type));

		graph->insertTrack(this->index, trackType);

		this->output("Add Mixer Track: [" + juce::String(this->type) + "]" + trackType.getDescription() + "\n"
			+ "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrack::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeTrack((this->index > -1 && this->index < graph->getTrackNum())
			? this->index : (graph->getTrackNum() - 1));

		this->output(juce::String{ "Undo Add Mixer Track.\n" }
			+ "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n");
		return true;
	}
	return false;
}

ActionAddMixerTrackSend::ActionAddMixerTrackSend(
	int src, int srcc, int dst, int dstc)
	: src(src), srcc(srcc), dst(dst), dstc(dstc) {}

bool ActionAddMixerTrackSend::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrackSend::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioTrk2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output("Undo " + juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

ActionAddMixerTrackInputFromDevice::ActionAddMixerTrackInputFromDevice(
	int srcc, int dst, int dstc)
	: srcc(srcc), dst(dst), dstc(dstc) {}

bool ActionAddMixerTrackInputFromDevice::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioI2TrkConnection(
			this->dst, this->srcc, this->dstc);

		this->output("[Device] " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrackInputFromDevice::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioI2TrkConnection(
			this->dst, this->srcc, this->dstc);

		this->output("Undo [Device] " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

ActionAddMixerTrackOutput::ActionAddMixerTrackOutput(
	int src, int srcc, int dstc)
	: src(src), srcc(srcc), dstc(dstc) {}

bool ActionAddMixerTrackOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2OConnection(
			this->src, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + "[Device] " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrackOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioTrk2OConnection(
			this->src, this->srcc, this->dstc);

		this->output("Undo " + juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + "[Device] " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

ActionAddEffect::ActionAddEffect(
	int track, int effect, const juce::String& pid)
	: track(track), effect(effect), pid(pid) {}

bool ActionAddEffect::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_LOADING(
		"Don't do this while loading plugin.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change plugin black list while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	if (auto des = Plugin::getInstance()->findPlugin(this->pid, false)) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(this->track)) {
				if (auto pluginDock = track->getPluginDock()) {
					if (auto ptr = pluginDock->insertPlugin(this->effect)) {
						PluginLoader::getInstance()->loadPlugin(*(des.get()), ptr);

						this->output("Insert Plugin: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + this->pid + "\n");
						return true;
					}
				}
			}
		}
	}

	this->output("Can't Insert Plugin: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + this->pid + "\n");
	return false;
}

bool ActionAddEffect::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->removePlugin((this->effect > -1 && this->effect < pluginDock->getPluginNum())
					? this->effect : (pluginDock->getPluginNum() - 1));

				this->output("Undo Insert Plugin: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + this->pid + "\n");
				return true;
			}
		}
	}

	this->output("Can't Undo Insert Plugin: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + this->pid + "\n");
	return false;
}

ActionAddEffectAdditionalInput::ActionAddEffectAdditionalInput(
	int track, int effect, int srcc, int dstc)
	: track(track), effect(effect), srcc(srcc), dstc(dstc) {}

bool ActionAddEffectAdditionalInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->addAdditionalBusConnection(this->effect, this->srcc, this->dstc);

				this->output("Link Plugin Channel: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + juce::String(this->srcc) + " - " + juce::String(this->dstc) + "\n");
				return true;
			}
		}
	}
	return false;
}

bool ActionAddEffectAdditionalInput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->removeAdditionalBusConnection(this->effect, this->srcc, this->dstc);

				this->output("Undo Link Plugin Channel: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + juce::String(this->srcc) + " - " + juce::String(this->dstc) + "\n");
				return true;
			}
		}
	}
	return false;
}

ActionAddInstr::ActionAddInstr(
	int index, int type, const juce::String& pid)
	: index(index), type(type), pid(pid) {}

bool ActionAddInstr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_LOADING(
		"Don't do this while loading plugin.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change plugin black list while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	auto pluginType = utils::getChannelSet(static_cast<utils::TrackType>(this->type));
	if (auto des = Plugin::getInstance()->findPlugin(this->pid, true)) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto ptr = graph->insertInstrument(this->index, pluginType)) {
				PluginLoader::getInstance()->loadPlugin(*(des.get()), ptr);

				this->output("Insert Plugin: [" + juce::String(this->index) + "] " + this->pid + " : " + pluginType.getDescription() + "\n");
				return true;
			}
		}
	}

	this->output("Can't Insert Plugin: [" + juce::String(this->index) + "] " + this->pid + " : " + pluginType.getDescription() + "\n");
	return false;
}

bool ActionAddInstr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	auto pluginType = utils::getChannelSet(static_cast<utils::TrackType>(this->type));
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeInstrument((this->index > -1 && this->index < graph->getInstrumentNum())
			? this->index : (graph->getInstrumentNum() - 1));

		this->output("Undo Insert Plugin: [" + juce::String(this->index) + "] " + this->pid + " : " + pluginType.getDescription() + "\n");
		return true;
	}
	
	this->output("Can't Undo Insert Plugin: [" + juce::String(this->index) + "] " + this->pid + " : " + pluginType.getDescription() + "\n");
	return false;
}

ActionAddInstrOutput::ActionAddInstrOutput(
	int src, int srcc, int dst, int dstc)
	: src(src), srcc(srcc), dst(dst), dstc(dstc) {}

bool ActionAddInstrOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioInstr2TrkConnection(this->src, this->dst, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

bool ActionAddInstrOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioInstr2TrkConnection(this->src, this->dst, this->srcc, this->dstc);

		this->output("Undo " + juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

ActionAddInstrMidiInput::ActionAddInstrMidiInput(int dst)
	: dst(dst) {}

bool ActionAddInstrMidiInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDII2InstrConnection(this->dst);

		this->output(juce::String("[MIDI Input]") + " - " + juce::String(this->dst) + "\n");
		return true;
	}
	return false;
}

bool ActionAddInstrMidiInput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDII2InstrConnection(this->dst);

		this->output(juce::String("Undo [MIDI Input]") + " - " + juce::String(this->dst) + "\n");
		return true;
	}
	return false;
}

ActionAddMixerTrackMidiInput::ActionAddMixerTrackMidiInput(int dst)
	: dst(dst) {}

bool ActionAddMixerTrackMidiInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDII2TrkConnection(this->dst);

		this->output(juce::String("[MIDI Input]") + " - " + juce::String(this->dst) + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrackMidiInput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDII2TrkConnection(this->dst);

		this->output(juce::String("Undo [MIDI Input]") + " - " + juce::String(this->dst) + "\n");
		return true;
	}
	return false;
}

ActionAddMixerTrackMidiOutput::ActionAddMixerTrackMidiOutput(int src)
	: src(src) {}

bool ActionAddMixerTrackMidiOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDITrk2OConnection(this->src);

		this->output(juce::String(this->src) + " - " + "[MIDI Output]" + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrackMidiOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDITrk2OConnection(this->src);

		this->output("Undo " + juce::String(this->src) + " - " + "[MIDI Output]" + "\n");
		return true;
	}
	return false;
}

ActionAddAudioSourceThenLoad::ActionAddAudioSourceThenLoad(
	const juce::String& path, bool copy)
	: path(path), copy(copy) {}

bool ActionAddAudioSourceThenLoad::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	CloneableSourceManager::getInstance()
		->createNewSourceThenLoadAsync<CloneableAudioSource>(this->path, this->copy);

	this->output("Add Audio Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

bool ActionAddAudioSourceThenLoad::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add Audio Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

ActionAddAudioSourceThenInit::ActionAddAudioSourceThenInit(
	double sampleRate, int channels, double length)
	: sampleRate(sampleRate), channels(channels), length(length) {}

bool ActionAddAudioSourceThenInit::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	CloneableSourceManager::getInstance()
		->createNewSource<CloneableAudioSource>(
			this->sampleRate, this->channels, this->length);

	this->output("Add Audio Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

bool ActionAddAudioSourceThenInit::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add Audio Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

ActionAddMidiSourceThenLoad::ActionAddMidiSourceThenLoad(
	const juce::String& path, bool copy)
	: path(path), copy(copy) {}

bool ActionAddMidiSourceThenLoad::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	CloneableSourceManager::getInstance()
		->createNewSourceThenLoadAsync<CloneableMIDISource>(this->path, this->copy);

	this->output("Add MIDI Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

bool ActionAddMidiSourceThenLoad::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add MIDI Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

ActionAddMidiSourceThenInit::ActionAddMidiSourceThenInit() {}

bool ActionAddMidiSourceThenInit::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	CloneableSourceManager::getInstance()
		->createNewSource<CloneableMIDISource>();

	this->output("Add MIDI Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

bool ActionAddMidiSourceThenInit::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add MIDI Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

ActionAddSynthSourceThenLoad::ActionAddSynthSourceThenLoad(
	const juce::String& path, bool copy)
	: path(path), copy(copy) {}

bool ActionAddSynthSourceThenLoad::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	CloneableSourceManager::getInstance()
		->createNewSourceThenLoadAsync<CloneableSynthSource>(this->path, this->copy);

	this->output("Add Synth Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

bool ActionAddSynthSourceThenLoad::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add Synth Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

ActionAddSynthSourceThenInit::ActionAddSynthSourceThenInit() {}

bool ActionAddSynthSourceThenInit::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	CloneableSourceManager::getInstance()
		->createNewSource<CloneableSynthSource>();

	this->output("Add Synth Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

bool ActionAddSynthSourceThenInit::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add Synth Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

ActionAddSequencerTrack::ActionAddSequencerTrack(
	int index, int type)
	: index(index), type(type) {}

bool ActionAddSequencerTrack::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		juce::AudioChannelSet trackType = utils::getChannelSet(
			static_cast<utils::TrackType>(this->type));

		graph->insertSource(this->index, trackType);

		this->output("Add Sequencer Track: [" + juce::String(this->type) + "]" + trackType.getDescription() + "\n"
			+ "Total Sequencer Track Num: " + juce::String(graph->getSourceNum()) + "\n");
		return true;
	}
	return false;
}

bool ActionAddSequencerTrack::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		juce::AudioChannelSet trackType = utils::getChannelSet(
			static_cast<utils::TrackType>(this->type));

		graph->removeSource((this->index > -1 && this->index < graph->getSourceNum())
			? this->index : (graph->getSourceNum() - 1));

		this->output("Undo Add Sequencer Track: [" + juce::String(this->type) + "]" + trackType.getDescription() + "\n"
			+ "Total Sequencer Track Num: " + juce::String(graph->getSourceNum()) + "\n");
		return true;
	}
	return false;
}

ActionAddSequencerTrackMidiOutputToMixer::ActionAddSequencerTrackMidiOutputToMixer(
	int src, int dst)
	: src(src), dst(dst) {}

bool ActionAddSequencerTrackMidiOutputToMixer::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDISrc2TrkConnection(this->src, this->dst);

		this->output(juce::String(this->src) + " - " + juce::String(this->dst) + "\n");
		return true;
	}
	return false;
}

bool ActionAddSequencerTrackMidiOutputToMixer::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDISrc2TrkConnection(this->src, this->dst);

		this->output("Undo " + juce::String(this->src) + " - " + juce::String(this->dst) + "\n");
		return true;
	}
	return false;
}

ActionAddSequencerTrackMidiOutputToInstr::ActionAddSequencerTrackMidiOutputToInstr(
	int src, int dst)
	: src(src), dst(dst) {}

bool ActionAddSequencerTrackMidiOutputToInstr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDISrc2InstrConnection(this->src, this->dst);

		this->output(juce::String(this->src) + " - " + juce::String(this->dst) + "\n");
		return true;
	}
	return false;
}

bool ActionAddSequencerTrackMidiOutputToInstr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDISrc2InstrConnection(this->src, this->dst);

		this->output("Undo " + juce::String(this->src) + " - " + juce::String(this->dst) + "\n");
		return true;
	}
	return false;
}

ActionAddSequencerTrackOutput::ActionAddSequencerTrackOutput(
	int src, int srcc, int dst, int dstc)
	: src(src), srcc(srcc), dst(dst), dstc(dstc) {}

bool ActionAddSequencerTrackOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioSrc2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

bool ActionAddSequencerTrackOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioSrc2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output("Undo " + juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

ActionAddSequencerSourceInstance::ActionAddSequencerSourceInstance(
	int track, int src, double start, double end, double offset)
	: track(track), src(src), start(start), end(end), offset(offset) {}

bool ActionAddSequencerSourceInstance::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto seqTrack = graph->getSourceProcessor(this->track)) {
			auto ptrSrc = CloneableSourceManager::getInstance()->getSource(this->src);
			if (!ptrSrc) { return false; }

			this->index = seqTrack->addSeq(
				{ this->start, this->end, this->offset, ptrSrc, this->src });

			this->output("Add Sequencer Source Instance [" + juce::String(this->track) + "] : [" + juce::String(this->src) + "]\n"
				+ "Total Sequencer Source Instance: " + juce::String(seqTrack->getSeqNum()) + "\n");
			return true;
		}
	}
	return false;
}

bool ActionAddSequencerSourceInstance::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto seqTrack = graph->getSourceProcessor(this->track)) {
			seqTrack->removeSeq(this->index);

			this->output("Undo Add Sequencer Source Instance [" + juce::String(this->track) + "] : [" + juce::String(this->src) + "]\n"
				+ "Total Sequencer Source Instance: " + juce::String(seqTrack->getSeqNum()) + "\n");
			return true;
		}
	}
	return false;
}

ActionAddRecorderSourceInstance::ActionAddRecorderSourceInstance(
	int src, double offset, int compensate)
	: src(src), offset(offset), compensate(compensate) {}

bool ActionAddRecorderSourceInstance::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto recorder = graph->getRecorder()) {
			auto ptrSrc = CloneableSourceManager::getInstance()->getSource(this->src);
			if (!ptrSrc) { return false; }

			recorder->insertTask({ ptrSrc, this->src, this->offset, this->compensate });

			this->output("Add Recorder Source Instance [" + juce::String(this->src) + "]\n"
				+ "Total Recorder Source Instance: " + juce::String(recorder->getTaskNum()) + "\n");
			return true;
		}
	}
	return false;
}

bool ActionAddRecorderSourceInstance::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto recorder = graph->getRecorder()) {
			recorder->removeTask(recorder->getTaskNum() - 1);

			this->output("Undo Add Recorder Source Instance [" + juce::String(this->src) + "]\n"
				+ "Total Recorder Source Instance: " + juce::String(recorder->getTaskNum()) + "\n");
			return true;
		}
	}
	return false;
}

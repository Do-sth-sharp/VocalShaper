#include "ActionAdd.h"

#include "../AudioCore.h"
#include "../Utils.h"

#include "../source/CloneableSourceManagerTemplates.h"

ActionAddPluginBlackList::ActionAddPluginBlackList(const juce::String& plugin)
	: plugin(plugin) {}

bool ActionAddPluginBlackList::doAction() {
	if (AudioCore::getInstance()->pluginSearchThreadIsRunning()) {
		this->output("Don't change plugin black list while searching plugin.");
		return false;
	}

	AudioCore::getInstance()->addToPluginBlackList(this->plugin);
	this->output("Add to plugin black list.");
	return true;
}

ActionAddPluginSearchPath::ActionAddPluginSearchPath(const juce::String& path)
	: path(path) {}

bool ActionAddPluginSearchPath::doAction() {
	if (AudioCore::getInstance()->pluginSearchThreadIsRunning()) {
		this->output("Don't change plugin search path while searching plugin.");
		return false;
	}

	AudioCore::getInstance()->addToPluginSearchPath(this->path);
	this->output("Add to plugin search path.");
	return true;
}

ActionAddMixerTrack::ActionAddMixerTrack(int index, int type)
	: index(index), type(type) {}

bool ActionAddMixerTrack::doAction() {
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
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeTrack(this->index);

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
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrackSend::undo() {
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
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioI2TrkConnection(
			this->dst, this->srcc, this->dstc);

		this->output("[Device] " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrackInputFromDevice::undo() {
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
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2OConnection(
			this->src, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + "[Device] " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrackOutput::undo() {
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
	if (AudioCore::getInstance()->addEffect(this->pid, this->track, this->effect)) {
		this->output("Insert Plugin: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + this->pid + "\n");
		return true;
	}
	
	this->output("Insert Plugin: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + this->pid + "\n");
	return false;
}

bool ActionAddEffect::undo() {
	if (AudioCore::getInstance()->removeEffect(this->track, this->effect)) {
		this->output("Undo Insert Plugin: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + this->pid + "\n");
		return true;
	}

	this->output("Can't Undo Insert Plugin: [" + juce::String(this->track) + ", " + juce::String(this->effect) + "] " + this->pid + "\n");
	return false;
}

ActionAddEffectAdditionalInput::ActionAddEffectAdditionalInput(
	int track, int effect, int srcc, int dstc)
	: track(track), effect(effect), srcc(srcc), dstc(dstc) {}

bool ActionAddEffectAdditionalInput::doAction() {
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
	auto pluginType = utils::getChannelSet(static_cast<utils::TrackType>(this->type));
	if (AudioCore::getInstance()->addInstrument(this->pid, this->index, pluginType)) {
		this->output("Insert Plugin: [" + juce::String(this->index) + "] " + this->pid + " : " + pluginType.getDescription() + "\n");
		return true;
	}
	this->output("Can't Insert Plugin: [" + juce::String(this->index) + "] " + this->pid + " : " + pluginType.getDescription() + "\n");
	return false;
}

bool ActionAddInstr::undo() {
	auto pluginType = utils::getChannelSet(static_cast<utils::TrackType>(this->type));
	if (AudioCore::getInstance()->removeInstrument(this->index)) {
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
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioInstr2TrkConnection(this->src, this->dst, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

bool ActionAddInstrOutput::undo() {
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
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDII2InstrConnection(this->dst);

		this->output(juce::String("[MIDI Input]") + " - " + juce::String(this->dst) + "\n");
		return true;
	}
	return false;
}

bool ActionAddInstrMidiInput::undo() {
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
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDII2TrkConnection(this->dst);

		this->output(juce::String("[MIDI Input]") + " - " + juce::String(this->dst) + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrackMidiInput::undo() {
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
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDITrk2OConnection(this->src);

		this->output(juce::String(this->src) + " - " + "[MIDI Output]" + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrackMidiOutput::undo() {
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
	CloneableSourceManager::getInstance()
		->createNewSourceThenLoadAsync<CloneableAudioSource>(this->path, this->copy);

	this->output("Add Audio Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

bool ActionAddAudioSourceThenLoad::undo() {
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
	CloneableSourceManager::getInstance()
		->createNewSource<CloneableAudioSource>(
			this->sampleRate, this->channels, this->length);

	this->output("Add Audio Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

bool ActionAddAudioSourceThenInit::undo() {
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
	CloneableSourceManager::getInstance()
		->createNewSourceThenLoadAsync<CloneableMIDISource>(this->path, this->copy);

	this->output("Add MIDI Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

bool ActionAddMidiSourceThenLoad::undo() {
	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add MIDI Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

ActionAddMidiSourceThenInit::ActionAddMidiSourceThenInit() {}

bool ActionAddMidiSourceThenInit::doAction() {
	CloneableSourceManager::getInstance()
		->createNewSource<CloneableMIDISource>();

	this->output("Add MIDI Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

bool ActionAddMidiSourceThenInit::undo() {
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
	CloneableSourceManager::getInstance()
		->createNewSourceThenLoadAsync<CloneableSynthSource>(this->path, this->copy);

	this->output("Add Synth Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

bool ActionAddSynthSourceThenLoad::undo() {
	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add Synth Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

ActionAddSynthSourceThenInit::ActionAddSynthSourceThenInit() {}

bool ActionAddSynthSourceThenInit::doAction() {
	CloneableSourceManager::getInstance()
		->createNewSource<CloneableSynthSource>();

	this->output("Add Synth Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	return true;
}

bool ActionAddSynthSourceThenInit::undo() {
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
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		juce::AudioChannelSet trackType = utils::getChannelSet(
			static_cast<utils::TrackType>(this->type));

		graph->removeSource(this->index);

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
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDISrc2TrkConnection(this->src, this->dst);

		this->output(juce::String(this->src) + " - " + juce::String(this->dst) + "\n");
		return true;
	}
	return false;
}

bool ActionAddSequencerTrackMidiOutputToMixer::undo() {
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
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDISrc2InstrConnection(this->src, this->dst);

		this->output(juce::String(this->src) + " - " + juce::String(this->dst) + "\n");
		return true;
	}
	return false;
}

bool ActionAddSequencerTrackMidiOutputToInstr::undo() {
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
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioSrc2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

bool ActionAddSequencerTrackOutput::undo() {
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
	this->index = AudioCore::getInstance()->addSequencerSourceInstance(
		this->track, this->src, this->start, this->end, this->offset);

	this->output("Add Sequencer Source Instance [" + juce::String(this->track) + "] : [" + juce::String(this->src) + "]\n"
		+ "Total Sequencer Source Instance: " + juce::String(AudioCore::getInstance()->getSequencerSourceInstanceNum(this->track)) + "\n");
	return true;
}

bool ActionAddSequencerSourceInstance::undo() {
	AudioCore::getInstance()->removeSequencerSourceInstance(
		this->track, this->index);

	this->output("Undo Add Sequencer Source Instance [" + juce::String(this->track) + "] : [" + juce::String(this->src) + "]\n"
		+ "Total Sequencer Source Instance: " + juce::String(AudioCore::getInstance()->getSequencerSourceInstanceNum(this->track)) + "\n");
	return true;
}

ActionAddRecorderSourceInstance::ActionAddRecorderSourceInstance(
	int src, double offset)
	: src(src), offset(offset) {}

bool ActionAddRecorderSourceInstance::doAction() {
	AudioCore::getInstance()->addRecorderSourceInstance(this->src, this->offset);

	this->output("Add Recorder Source Instance [" + juce::String(this->src) + "]\n"
		+ "Total Recorder Source Instance: " + juce::String(AudioCore::getInstance()->getRecorderSourceInstanceNum()) + "\n");
	return true;
}

bool ActionAddRecorderSourceInstance::undo() {
	AudioCore::getInstance()->removeRecorderSourceInstance(
		AudioCore::getInstance()->getRecorderSourceInstanceNum() - 1);

	this->output("Undo Add Recorder Source Instance [" + juce::String(this->src) + "]\n"
		+ "Total Recorder Source Instance: " + juce::String(AudioCore::getInstance()->getRecorderSourceInstanceNum()) + "\n");
	return true;
}

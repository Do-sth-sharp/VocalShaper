#include "ActionAdd.h"

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
	ACTION_CHECK_PLUGIN_LOADING(
		"Don't do this while loading plugin.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change plugin black list while searching plugin.");

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

	this->output("Can't Insert Plugin: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + ACTION_DATA(pid) + "\n");
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

	this->output("Can't Undo Insert Plugin: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + ACTION_DATA(pid) + "\n");
	ACTION_RESULT(false);
}

ActionAddEffectAdditionalInput::ActionAddEffectAdditionalInput(
	int track, int effect, int srcc, int dstc)
	: ACTION_DB{ track, effect, srcc, dstc } {}

bool ActionAddEffectAdditionalInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddEffectAdditionalInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->addAdditionalBusConnection(ACTION_DATA(effect), ACTION_DATA(srcc), ACTION_DATA(dstc));

				this->output("Link Plugin Channel: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dstc)) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionAddEffectAdditionalInput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddEffectAdditionalInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->removeAdditionalBusConnection(ACTION_DATA(effect), ACTION_DATA(srcc), ACTION_DATA(dstc));

				this->output("Undo Link Plugin Channel: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "] " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dstc)) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionAddInstr::ActionAddInstr(
	int index, int type, const juce::String& pid)
	: ACTION_DB{ index, type, pid } {}

bool ActionAddInstr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_LOADING(
		"Don't do this while loading plugin.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change plugin black list while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddInstr);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(pid);

	auto pluginType = utils::getChannelSet(static_cast<utils::TrackType>(ACTION_DATA(type)));
	if (auto des = Plugin::getInstance()->findPlugin(ACTION_DATA(pid), true)) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto ptr = graph->insertInstrument(ACTION_DATA(index), pluginType)) {
				PluginLoader::getInstance()->loadPlugin(*(des.get()), ptr);

				this->output("Insert Plugin: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + " : " + pluginType.getDescription() + "\n");
				ACTION_RESULT(true);
			}
		}
	}

	this->output("Can't Insert Plugin: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + " : " + pluginType.getDescription() + "\n");
	ACTION_RESULT(false);
}

bool ActionAddInstr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddInstr);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(pid);

	auto pluginType = utils::getChannelSet(static_cast<utils::TrackType>(ACTION_DATA(type)));
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeInstrument((ACTION_DATA(index) > -1 && ACTION_DATA(index) < graph->getInstrumentNum())
			? ACTION_DATA(index) : (graph->getInstrumentNum() - 1));

		this->output("Undo Insert Plugin: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + " : " + pluginType.getDescription() + "\n");
		ACTION_RESULT(true);
	}
	
	this->output("Can't Undo Insert Plugin: [" + juce::String(ACTION_DATA(index)) + "] " + ACTION_DATA(pid) + " : " + pluginType.getDescription() + "\n");
	ACTION_RESULT(false);
}

ActionAddInstrOutput::ActionAddInstrOutput(
	int src, int srcc, int dst, int dstc)
	: ACTION_DB{ src, srcc, dst, dstc } {}

bool ActionAddInstrOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddInstrOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioInstr2TrkConnection(ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output(juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionAddInstrOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddInstrOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioInstr2TrkConnection(ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionAddInstrMidiInput::ActionAddInstrMidiInput(int dst)
	: ACTION_DB{ dst } {}

bool ActionAddInstrMidiInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddInstrMidiInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDII2InstrConnection(ACTION_DATA(dst));

		this->output(juce::String("[MIDI Input]") + " - " + juce::String(ACTION_DATA(dst)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionAddInstrMidiInput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddInstrMidiInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDII2InstrConnection(ACTION_DATA(dst));

		this->output(juce::String("Undo [MIDI Input]") + " - " + juce::String(ACTION_DATA(dst)) + "\n");
		ACTION_RESULT(true);
	}
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

ActionAddAudioSourceThenLoad::ActionAddAudioSourceThenLoad(
	const juce::String& path, bool copy)
	: ACTION_DB{ path, copy } {}

bool ActionAddAudioSourceThenLoad::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddAudioSourceThenLoad);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(path);

	CloneableSourceManager::getInstance()
		->createNewSourceThenLoadAsync<CloneableAudioSource>(ACTION_DATA(path), ACTION_DATA(copy));

	this->output("Add Audio Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	ACTION_RESULT(true);
}

bool ActionAddAudioSourceThenLoad::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddAudioSourceThenLoad);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(path);

	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add Audio Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	ACTION_RESULT(true);
}

ActionAddAudioSourceThenInit::ActionAddAudioSourceThenInit(
	double sampleRate, int channels, double length)
	: ACTION_DB{ sampleRate, channels, length } {}

bool ActionAddAudioSourceThenInit::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddAudioSourceThenInit);
	ACTION_WRITE_DB();

	CloneableSourceManager::getInstance()
		->createNewSource<CloneableAudioSource>(
			ACTION_DATA(sampleRate), ACTION_DATA(channels), ACTION_DATA(length));

	this->output("Add Audio Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	ACTION_RESULT(true);
}

bool ActionAddAudioSourceThenInit::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddAudioSourceThenInit);
	ACTION_WRITE_DB();

	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add Audio Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	ACTION_RESULT(true);
}

ActionAddMidiSourceThenLoad::ActionAddMidiSourceThenLoad(
	const juce::String& path, bool copy)
	: ACTION_DB{ path, copy } {}

bool ActionAddMidiSourceThenLoad::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddMidiSourceThenLoad);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(path);

	CloneableSourceManager::getInstance()
		->createNewSourceThenLoadAsync<CloneableMIDISource>(ACTION_DATA(path), ACTION_DATA(copy));

	this->output("Add MIDI Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	ACTION_RESULT(true);
}

bool ActionAddMidiSourceThenLoad::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddMidiSourceThenLoad);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(path);

	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add MIDI Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	ACTION_RESULT(true);
}

ActionAddMidiSourceThenInit::ActionAddMidiSourceThenInit() {}

bool ActionAddMidiSourceThenInit::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_WRITE_TYPE(ActionAddMidiSourceThenInit);

	CloneableSourceManager::getInstance()
		->createNewSource<CloneableMIDISource>();

	this->output("Add MIDI Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	ACTION_RESULT(true);
}

bool ActionAddMidiSourceThenInit::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddMidiSourceThenInit);

	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add MIDI Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	ACTION_RESULT(true);
}

ActionAddSynthSourceThenLoad::ActionAddSynthSourceThenLoad(
	const juce::String& path, bool copy)
	: ACTION_DB{ path, copy } {}

bool ActionAddSynthSourceThenLoad::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddSynthSourceThenLoad);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(path);

	CloneableSourceManager::getInstance()
		->createNewSourceThenLoadAsync<CloneableSynthSource>(ACTION_DATA(path), ACTION_DATA(copy));

	this->output("Add Synth Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	ACTION_RESULT(true);
}

bool ActionAddSynthSourceThenLoad::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddSynthSourceThenLoad);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(path);

	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add Synth Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	ACTION_RESULT(true);
}

ActionAddSynthSourceThenInit::ActionAddSynthSourceThenInit() {}

bool ActionAddSynthSourceThenInit::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddSynthSourceThenInit);

	CloneableSourceManager::getInstance()
		->createNewSource<CloneableSynthSource>();

	this->output("Add Synth Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	ACTION_RESULT(true);
}

bool ActionAddSynthSourceThenInit::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddSynthSourceThenInit);

	CloneableSourceManager::getInstance()
		->removeSource(CloneableSourceManager::getInstance()->getSourceNum() - 1);

	this->output("Undo Add Synth Source.\n"
		"Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n");
	ACTION_RESULT(true);
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

ActionAddSequencerTrackMidiOutputToInstr::ActionAddSequencerTrackMidiOutputToInstr(
	int src, int dst)
	: ACTION_DB{ src, dst } {}

bool ActionAddSequencerTrackMidiOutputToInstr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddSequencerTrackMidiOutputToInstr);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDISrc2InstrConnection(ACTION_DATA(src), ACTION_DATA(dst));

		this->output(juce::String(ACTION_DATA(src)) + " - " + juce::String(ACTION_DATA(dst)) + "\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionAddSequencerTrackMidiOutputToInstr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddSequencerTrackMidiOutputToInstr);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeMIDISrc2InstrConnection(ACTION_DATA(src), ACTION_DATA(dst));

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

ActionAddSequencerSourceInstance::ActionAddSequencerSourceInstance(
	int track, int src, double start, double end, double offset)
	: ACTION_DB{ track, src, start, end, offset } {}

bool ActionAddSequencerSourceInstance::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddSequencerSourceInstance);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto seqTrack = graph->getSourceProcessor(ACTION_DATA(track))) {
			auto ptrSrc = CloneableSourceManager::getInstance()->getSource(ACTION_DATA(src));
			if (!ptrSrc) { ACTION_RESULT(false); }

			ACTION_DATA(index) = seqTrack->addSeq(
				{ ACTION_DATA(start), ACTION_DATA(end), ACTION_DATA(offset), ptrSrc, ACTION_DATA(src) });

			this->output("Add Sequencer Source Instance [" + juce::String(ACTION_DATA(track)) + "] : [" + juce::String(ACTION_DATA(src)) + "]\n"
				+ "Total Sequencer Source Instance: " + juce::String(seqTrack->getSeqNum()) + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionAddSequencerSourceInstance::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddSequencerSourceInstance);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto seqTrack = graph->getSourceProcessor(ACTION_DATA(track))) {
			seqTrack->removeSeq(ACTION_DATA(index));

			this->output("Undo Add Sequencer Source Instance [" + juce::String(ACTION_DATA(track)) + "] : [" + juce::String(ACTION_DATA(src)) + "]\n"
				+ "Total Sequencer Source Instance: " + juce::String(seqTrack->getSeqNum()) + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

ActionAddRecorderSourceInstance::ActionAddRecorderSourceInstance(
	int src, double offset, int compensate)
	: ACTION_DB{ src, offset, compensate } {}

bool ActionAddRecorderSourceInstance::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionAddRecorderSourceInstance);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto recorder = graph->getRecorder()) {
			auto ptrSrc = CloneableSourceManager::getInstance()->getSource(ACTION_DATA(src));
			if (!ptrSrc) { ACTION_RESULT(false); }

			recorder->insertTask({ ptrSrc, ACTION_DATA(src), ACTION_DATA(offset), ACTION_DATA(compensate) });

			this->output("Add Recorder Source Instance [" + juce::String(ACTION_DATA(src)) + "]\n"
				+ "Total Recorder Source Instance: " + juce::String(recorder->getTaskNum()) + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionAddRecorderSourceInstance::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionAddRecorderSourceInstance);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto recorder = graph->getRecorder()) {
			recorder->removeTask(recorder->getTaskNum() - 1);

			this->output("Undo Add Recorder Source Instance [" + juce::String(ACTION_DATA(src)) + "]\n"
				+ "Total Recorder Source Instance: " + juce::String(recorder->getTaskNum()) + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

#include "ActionOther.h"
#include "ActionDispatcher.h"

#include "../AudioCore.h"
#include "../misc/PlayPosition.h"
#include "../plugin/Plugin.h"
#include "../source/SourceManager.h"
#include "../recovery/DataControl.hpp"
#include "../Utils.h"

ActionClearPlugin::ActionClearPlugin() {}

bool ActionClearPlugin::doAction() {
	ACTION_CHECK_PLUGIN_LOADING(
		"Don't do this while loading plugin.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change plugin list while searching plugin.");

	Plugin::getInstance()->clearPluginTemporary();

	this->output("Clear plugin list.");
	return true;
}

ActionSearchPlugin::ActionSearchPlugin() {}

bool ActionSearchPlugin::doAction() {
	ACTION_CHECK_PLUGIN_LOADING(
		"Don't do this while loading plugin.");
	if (Plugin::getInstance()->pluginSearchThreadIsRunning()) { return true; }

	Plugin::getInstance()->clearPluginList();
	Plugin::getInstance()->getPluginList();

	this->output("Searching Audio Plugin...");
	return true;
}

ActionPlay::ActionPlay() {}

bool ActionPlay::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	auto pos = AudioCore::getInstance()->getPosition();
	AudioCore::getInstance()->play();

	this->output("Start play at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n");
	return true;
}

ActionPause::ActionPause() {}

bool ActionPause::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	AudioCore::getInstance()->pause();
	auto pos = AudioCore::getInstance()->getPosition();

	this->output("Pause play at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n");
	return true;
}

ActionStop::ActionStop() {}

bool ActionStop::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	auto pos = AudioCore::getInstance()->getPosition();
	AudioCore::getInstance()->stop();

	this->output("Stop play at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n");
	return true;
}

ActionRewind::ActionRewind() {}

bool ActionRewind::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	AudioCore::getInstance()->rewind();

	this->output("Rewind play\n");
	return true;
}

ActionStartRecord::ActionStartRecord() {}

bool ActionStartRecord::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	auto pos = AudioCore::getInstance()->getPosition();
	AudioCore::getInstance()->record(true);

	this->output("Start record at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n");
	return true;
}

ActionStopRecord::ActionStopRecord() {};

bool ActionStopRecord::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	auto pos = AudioCore::getInstance()->getPosition();
	AudioCore::getInstance()->record(false);

	this->output("Stop record at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n");
	return true;
}

ActionRenderNow::ActionRenderNow(
	const juce::String& path, const juce::String& name,
	const juce::String& extension, const juce::Array<int>& tracks,
	const juce::StringPairArray& metaData, int bitDepth, int quality)
	: path(path), name(name), extension(extension), tracks(tracks),
	metaData(metaData), bitDepth(bitDepth), quality(quality) {}

bool ActionRenderNow::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	if (AudioCore::getInstance()->renderNow(
		this->tracks, this->path, this->name, this->extension,
		this->metaData, this->bitDepth, this->quality)) {
		juce::String result;

		result += "Start rendering:\n";
		result += "    Path: " + this->path + "\n";
		result += "    Name: " + this->name + "\n";
		result += "    Format: " + this->extension + "\n";
		result += "    Tracks: ";
		for (auto& i : this->tracks) {
			result += juce::String(i) + " ";
		}
		result += "\n";

		this->output(result);
		return true;
	}

	this->error("Can't start to render. Maybe rendering is already started!\n");
	return false;
}

ActionNewProject::ActionNewProject(const juce::String& path)
	: path(path) {}

bool ActionNewProject::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	if (AudioCore::getInstance()->newProj(this->path)) {
		ActionDispatcher::getInstance()->clearUndoList();
		resetRecoveryMemoryBlock();

		this->output("Create new project at: " + this->path + "\n");
		return true;
	}
	this->error("Can't create new project at: " + this->path + "\n");
	return false;
}

ActionSave::ActionSave(const juce::String& name)
	: ACTION_DB{ name } {}

bool ActionSave::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	ACTION_WRITE_TYPE(ActionSave);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(name);

	if (AudioCore::getInstance()->save(ACTION_DATA(name))) {
		this->output("Saved project data to: " + ACTION_DATA(name) + "\n");
		ACTION_RESULT(true);
	}
	this->error("Can't save project data to: " + ACTION_DATA(name) + "\n");
	ACTION_RESULT(false);
}

ActionLoad::ActionLoad(const juce::String& path)
	: path(path) {}

bool ActionLoad::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");
	ACTION_CHECK_PLUGIN_LOADING(
		"Don't do this while loading plugin.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't load project while searching plugin.");

	if (AudioCore::getInstance()->load(this->path)) {
		ActionDispatcher::getInstance()->clearUndoList();
		resetRecoveryMemoryBlock();

		this->output("Load project data from: " + this->path + "\n");
		return true;
	}
	this->error("Can't load project data from: " + this->path + "\n");
	return false;
}

ActionInitAudioSource::ActionInitAudioSource(int index,
	double sampleRate, double length)
	: index(index), sampleRate(sampleRate), length(length) {};

bool ActionInitAudioSource::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(this->index)) {
			track->applyAudio();
			auto ref = track->getAudioRef();
			int channels = track->getAudioChannelSet().size();
			SourceManager::getInstance()->initAudio(ref,
				channels, this->sampleRate, this->length);

			this->output("Init audio source: [" + juce::String{ this->index } + "] " + juce::String{ this->sampleRate } + ", " + juce::String{ this->length } + "s\n");
			return true;
		}
	}
	this->error("Can't init audio source: [" + juce::String{ this->index } + "] " + juce::String{ this->sampleRate } + ", " + juce::String{ this->length } + "s\n");
	return false;
}

ActionInitMidiSource::ActionInitMidiSource(int index)
	: index(index) {}

bool ActionInitMidiSource::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(this->index)) {
			track->applyMIDI();
			auto ref = track->getMIDIRef();
			SourceManager::getInstance()->initMIDI(ref);

			this->output("Init midi source: [" + juce::String{ this->index } + "]\n");
			return true;
		}
	}
	this->error("Can't init midi source: [" + juce::String{ this->index } + "]\n");
	return false;
}

ActionLoadAudioSource::ActionLoadAudioSource(int index,
	const juce::String& path, const SourceCallback& callback)
	: index(index), path(path), callback(callback) {};

bool ActionLoadAudioSource::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(this->index)) {
			track->applyAudio();
			auto ref = track->getAudioRef();
			SourceIO::getInstance()->addTask(
				{ SourceIO::TaskType::Read, ref, this->path,
				false, this->callback });

			this->output("Load audio source: [" + juce::String{ this->index } + "]" + this->path + "\n");
			return true;
		}
	}
	this->error("Can't load audio source: [" + juce::String{ this->index } + "]" + this->path + "\n");
	return false;
}

ActionLoadMidiSource::ActionLoadMidiSource(int index,
	const juce::String& path, bool getTempo,
	const SourceCallback& callback)
	: index(index), path(path), getTempo(getTempo), callback(callback) {}

bool ActionLoadMidiSource::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(this->index)) {
			track->applyMIDI();
			auto ref = track->getMIDIRef();
			SourceIO::getInstance()->addTask(
				{ SourceIO::TaskType::Read, ref, this->path,
				this->getTempo, this->callback });

			this->output("Load midi source: [" + juce::String{ this->index } + "]" + this->path + "\n");
			return true;
		}
	}
	this->error("Can't load midi source: [" + juce::String{ this->index } + "]" + this->path + "\n");
	return false;
}

ActionSaveAudioSource::ActionSaveAudioSource(int index,
	const juce::String& path)
	: index(index), path(path) {}

bool ActionSaveAudioSource::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(this->index)) {
			auto ref = track->getAudioRef();
			SourceIO::getInstance()->addTask(
				{ SourceIO::TaskType::Write, ref, this->path, false, {} });

			this->output("Save audio source: [" + juce::String{ this->index } + "]" + this->path + "\n");
			return true;
		}
	}
	this->error("Can't save audio source: [" + juce::String{ this->index } + "]" + this->path + "\n");
	return false;
}

ActionSaveMidiSource::ActionSaveMidiSource(int index,
	const juce::String& path)
	: index(index), path(path) {}

bool ActionSaveMidiSource::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(this->index)) {
			auto ref = track->getMIDIRef();
			SourceIO::getInstance()->addTask(
				{ SourceIO::TaskType::Write, ref, this->path, false, {} });

			this->output("Save midi source: [" + juce::String{ this->index } + "]" + this->path + "\n");
			return true;
		}
	}
	this->error("Can't save midi source: [" + juce::String{ this->index } + "]" + this->path + "\n");
	return false;
}

ActionSynth::ActionSynth(int index)
	: index(index) {}

bool ActionSynth::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(this->index)) {
			track->startSynth();

			this->output("Synth: [" + juce::String{ this->index } + "]\n");
			return true;
		}
	}
	this->error("Can't synth: [" + juce::String{ this->index } + "]\n");
	return false;
}

ActionSplitSequencerBlock::ActionSplitSequencerBlock(
	int track, int block, double time)
	: ACTION_DB{ track, block, time } {}

bool ActionSplitSequencerBlock::doAction() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionSplitSequencerBlock);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(track))) {
			if (track->splitSeq(ACTION_DATA(block), ACTION_DATA(time))) {
				this->output("Split seq block: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String{ ACTION_DATA(block) } + "]\n");
				ACTION_RESULT(true);
			}
		}
	}
	this->output("Can't split seq block: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String{ ACTION_DATA(block) } + "]\n");
	ACTION_RESULT(false);
}

bool ActionSplitSequencerBlock::undo() {
	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionSplitSequencerBlock);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(track))) {
			if (track->stickSeqWithNext(ACTION_DATA(block))) {
				this->output("Undo split seq block: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String{ ACTION_DATA(block) } + "]\n");
				ACTION_RESULT(true);
			}
		}
	}
	this->output("Can't undo split seq block: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String{ ACTION_DATA(block) } + "]\n");
	ACTION_RESULT(false);
}

#include "ActionOther.h"
#include "ActionDispatcher.h"

#include "../AudioCore.h"
#include "../misc/PlayPosition.h"
#include "../plugin/Plugin.h"
#include "../source/SourceManager.h"
#include "../source/SourceIO.h"
#include "../recovery/DataControl.hpp"
#include "../Utils.h"

ActionClearPlugin::ActionClearPlugin() {}

bool ActionClearPlugin::doAction() {
	Plugin::getInstance()->clearPluginTemporary();
	return true;
}

const juce::String ActionClearPlugin::getStatusStr() const {
	return "";
}

ActionSearchPlugin::ActionSearchPlugin() {}

bool ActionSearchPlugin::doAction() {
	if (Plugin::getInstance()->pluginSearchThreadIsRunning()) { return true; }

	Plugin::getInstance()->clearPluginList();
	Plugin::getInstance()->getPluginList();

	return true;
}

const juce::String ActionSearchPlugin::getStatusStr() const {
	return "";
}

ActionPlay::ActionPlay() {}

bool ActionPlay::doAction() {
	AudioCore::getInstance()->play();
	return true;
}

const juce::String ActionPlay::getStatusStr() const {
	auto pos = AudioCore::getInstance()->getPosition();
	return juce::String{ pos->getTimeInSeconds().orFallback(0) } + "s";
}

ActionPause::ActionPause() {}

bool ActionPause::doAction() {
	AudioCore::getInstance()->pause();
	return true;
}

const juce::String ActionPause::getStatusStr() const {
	auto pos = AudioCore::getInstance()->getPosition();
	return juce::String{ pos->getTimeInSeconds().orFallback(0) } + "s";
}

ActionStop::ActionStop() {}

bool ActionStop::doAction() {
	AudioCore::getInstance()->stop();
	return true;
}

const juce::String ActionStop::getStatusStr() const {
	auto pos = AudioCore::getInstance()->getPosition();
	return juce::String{ pos->getTimeInSeconds().orFallback(0) } + "s";
}

ActionRewind::ActionRewind() {}

bool ActionRewind::doAction() {
	AudioCore::getInstance()->rewind();
	return true;
}

const juce::String ActionRewind::getStatusStr() const { return ""; }

ActionStartRecord::ActionStartRecord() {}

bool ActionStartRecord::doAction() {
	AudioCore::getInstance()->record(true);
	return true;
}

const juce::String ActionStartRecord::getStatusStr() const {
	auto pos = AudioCore::getInstance()->getPosition();
	return juce::String{ pos->getTimeInSeconds().orFallback(0) } + "s";
}

ActionStopRecord::ActionStopRecord() {};

bool ActionStopRecord::doAction() {
	AudioCore::getInstance()->record(false);
	return true;
}

const juce::String ActionStopRecord::getStatusStr() const {
	auto pos = AudioCore::getInstance()->getPosition();
	return juce::String{ pos->getTimeInSeconds().orFallback(0) } + "s";
}

ActionRenderNow::ActionRenderNow(
	const juce::String& path, const juce::String& name,
	const juce::String& extension, const juce::Array<int>& tracks,
	const juce::StringPairArray& metaData, int bitDepth, int quality)
	: path(path), name(name), extension(extension), tracks(tracks),
	metaData(metaData), bitDepth(bitDepth), quality(quality) {}

bool ActionRenderNow::doAction() {
	if (AudioCore::getInstance()->renderNow(
		this->tracks, this->path, this->name, this->extension,
		this->metaData, this->bitDepth, this->quality)) {
		return true;
	}
	return false;
}

const juce::String ActionRenderNow::getStatusStr() const {
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

	return result;
}

ActionNewProject::ActionNewProject(const juce::String& path)
	: path(path) {}

bool ActionNewProject::doAction() {
	if (AudioCore::getInstance()->newProj(this->path)) {
		ActionDispatcher::getInstance()->clearUndoList();
		resetRecoveryMemoryBlock();
		return true;
	}
	return false;
}

const juce::String ActionNewProject::getStatusStr() const {
	return this->path;
}

ActionSave::ActionSave(const juce::String& name)
	: name(name) {}

bool ActionSave::doAction() {
	if (AudioCore::getInstance()->save(this->name)) {
		return true;
	}
	return false;
}

const juce::String ActionSave::getStatusStr() const {
	return this->name;
}

void ActionSave::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeString(this->name);
}

ActionLoad::ActionLoad(const juce::String& path)
	: path(path) {}

bool ActionLoad::doAction() {
	if (AudioCore::getInstance()->load(this->path)) {
		ActionDispatcher::getInstance()->clearUndoList();
		resetRecoveryMemoryBlock();
		return true;
	}
	return false;
}

const juce::String ActionLoad::getStatusStr() const {
	return this->path;
}

ActionInitAudioSource::ActionInitAudioSource(
	int index, const juce::String& name,
	double sampleRate, int channels, double length)
	: index(index), name(name),
	sampleRate(sampleRate), channels(channels), length(length) {};

bool ActionInitAudioSource::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->index)) {
			if (auto seq = track->getSequencer()) {
				seq->applyAudio();
				auto ref = seq->getAudioRef();
				SourceManager::getInstance()->initAudio(ref, this->name,
					this->channels, this->sampleRate, this->length);

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionInitAudioSource::getStatusStr() const {
	return "[" + juce::String{ this->index } + ", " + this->name + "] " + juce::String{ this->sampleRate } + ", " + juce::String{ this->channels } + ", " + juce::String{ this->length } + "s";
}

ActionInitMidiSource::ActionInitMidiSource(
	int index, const juce::String& name)
	: index(index), name(name) {}

bool ActionInitMidiSource::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->index)) {
			if (auto seq = track->getSequencer()) {
				seq->applyMIDI();
				auto ref = seq->getMIDIRef();
				SourceManager::getInstance()->initMIDI(ref, this->name);

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionInitMidiSource::getStatusStr() const {
	return "[" + juce::String{ this->index } + ", " + this->name + "]";
}

ActionLoadAudioSource::ActionLoadAudioSource(int index,
	const juce::String& path, const SourceCallback& callback)
	: index(index), path(path), callback(callback) {};

bool ActionLoadAudioSource::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->index)) {
			if (auto seq = track->getSequencer()) {
				seq->applyAudio();
				auto ref = seq->getAudioRef();
				SourceIO::getInstance()->addTask(
					{ SourceIO::TaskType::Read, ref, this->path,
					false, this->callback });

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionLoadAudioSource::getStatusStr() const {
	return "[" + juce::String{ this->index } + "]" + this->path;
}

ActionLoadMidiSource::ActionLoadMidiSource(int index,
	const juce::String& path, bool getTempo,
	const SourceCallback& callback)
	: index(index), path(path), getTempo(getTempo), callback(callback) {}

bool ActionLoadMidiSource::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->index)) {
			if (auto seq = track->getSequencer()) {
				seq->applyMIDI();
				auto ref = seq->getMIDIRef();
				SourceIO::getInstance()->addTask(
					{ SourceIO::TaskType::Read, ref, this->path,
					this->getTempo, this->callback });

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionLoadMidiSource::getStatusStr() const {
	return "[" + juce::String{ this->index } + "]" + this->path;
}

ActionSaveAudioSource::ActionSaveAudioSource(int index,
	const juce::String& path)
	: index(index), path(path) {}

bool ActionSaveAudioSource::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->index)) {
			if (auto seq = track->getSequencer()) {
				auto ref = seq->getAudioRef();
				SourceIO::getInstance()->addTask(
					{ SourceIO::TaskType::Write, ref, this->path, false, {} });

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionSaveAudioSource::getStatusStr() const {
	return "[" + juce::String{ this->index } + "]" + this->path;
}

ActionSaveMidiSource::ActionSaveMidiSource(int index,
	const juce::String& path)
	: index(index), path(path) {}

bool ActionSaveMidiSource::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->index)) {
			if (auto seq = track->getSequencer()) {
				auto ref = seq->getMIDIRef();
				SourceIO::getInstance()->addTask(
					{ SourceIO::TaskType::Write, ref, this->path, false, {} });

				return true;
			}
		}
	}
	return false;
}

const juce::String ActionSaveMidiSource::getStatusStr() const {
	return "[" + juce::String{ this->index } + "]" + this->path;
}

ActionSplitSequencerBlock::ActionSplitSequencerBlock(
	int track, int block, double time)
	: track(track), block(block), time(time) {}

bool ActionSplitSequencerBlock::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				if (seq->splitSeq(this->block, this->time)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool ActionSplitSequencerBlock::undoAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(MainGraph::TrackType::Track, this->track)) {
			if (auto seq = track->getSequencer()) {
				if (seq->stickSeqWithNext(this->block)) {
					return true;
				}
			}
		}
	}
	return false;
}

const juce::String ActionSplitSequencerBlock::getStatusStr() const {
	return "[" + juce::String{ this->track } + ", " + juce::String{ this->block } + "]";
}

void ActionSplitSequencerBlock::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeInt(this->track);
	stream.writeInt(this->block);
	stream.writeDouble(this->time);
}

ActionLoadPluginState::ActionLoadPluginState(
	quickAPI::PluginHolder plugin, const juce::String& path)
	: plugin(plugin), path(path) {}

bool ActionLoadPluginState::doAction() {
	if (this->plugin) {
		juce::MemoryBlock state;
		if (!utils::readFileToBlock(this->path, state)) {
			return false;
		}
		this->plugin->getStateInformation(this->oldState);

		this->plugin->setStateInformation(
			state.getData(), state.getSize());

		return true;
	}
	return false;
}

bool ActionLoadPluginState::undoAction() {
	if (this->plugin) {
		this->plugin->setStateInformation(
			this->oldState.getData(), this->oldState.getSize());
		return true;
	}
	return false;
}

const juce::String ActionLoadPluginState::getStatusStr() const {
	return "[" + juce::String{ this->plugin ? this->plugin->getName() : "" } + "] " + this->path;
}

void ActionLoadPluginState::getRecoveryData(juce::MemoryOutputStream& stream) {
	stream.writeString(this->plugin ? this->plugin->getName() : "");
	stream.writeString(this->path);

	stream.writeInt64(this->oldState.getSize());
	stream.write(this->oldState.getData(), this->oldState.getSize());
}

ActionSavePluginState::ActionSavePluginState(
	quickAPI::PluginHolder plugin, const juce::String& path)
	: plugin(plugin), path(path) {}

bool ActionSavePluginState::doAction() {
	if (this->plugin) {
		juce::MemoryBlock state;
		this->plugin->getStateInformation(state);

		if (utils::writeBlockToFile(this->path, state)) {
			return true;
		}
	}
	return false;
}

const juce::String ActionSavePluginState::getStatusStr() const {
	return "[" + juce::String{ this->plugin ? this->plugin->getName() : "" } + "] " + this->path;
}

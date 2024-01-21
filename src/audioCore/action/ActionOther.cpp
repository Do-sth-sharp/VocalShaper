#include "ActionOther.h"
#include "ActionDispatcher.h"

#include "../AudioCore.h"
#include "../misc/PlayPosition.h"
#include "../plugin/Plugin.h"
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

ActionSynthSource::ActionSynthSource(int index)
	: index(index) {}

bool ActionSynthSource::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	if (auto src = CloneableSourceManager::getInstance()->getSource(this->index)) {
		if (AudioIOList::getInstance()->isTask(src)) {
			this->error("Unavailable source status!");
			ACTION_RESULT(false);
		}
		if (src->isSynthRunning()) {
			this->error("Unavailable source status!");
			ACTION_RESULT(false);
		}
		if (src->checkRecording() && PlayPosition::getInstance()->getPosition()->getIsRecording()) {
			this->error("Unavailable source status!");
			ACTION_RESULT(false);
		}

		auto dst = src->getDstSource();
		if (dst && dst->isSynthRunning()) {
			this->error("Unavailable dst source status!");
			ACTION_RESULT(false);
		}
		if (dst && AudioIOList::getInstance()->isTask(dst)) {
			this->error("Unavailable dst source status!");
			ACTION_RESULT(false);
		}
		if (dst && dst->checkRecording() && PlayPosition::getInstance()->getPosition()->getIsRecording()) {
			this->error("Unavailable dst source status!");
			ACTION_RESULT(false);
		}
	}

	if (CloneableSourceManager::getInstance()->synthSource(this->index)) {
		this->output("Start synth source: [" + juce::String(this->index) + "]\n");
		return true;
	}
	this->error("Can't start synth source: [" + juce::String(this->index) + "]\n");
	return false;
}

ActionCloneSource::ActionCloneSource(int index)
	: ACTION_DB{ index } {}

bool ActionCloneSource::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionCloneSource);
	ACTION_WRITE_DB();

	if (auto manager = CloneableSourceManager::getInstance()) {
		if (auto src = manager->getSource(ACTION_DATA(index))) {
			if (AudioIOList::getInstance()->isTask(src)) {
				this->error("Unavailable source status!");
				ACTION_RESULT(false);
			}
			if (src->isSynthRunning()) {
				this->error("Unavailable source status!");
				ACTION_RESULT(false);
			}
			if (src->checkRecording() && PlayPosition::getInstance()->getPosition()->getIsRecording()) {
				this->error("Unavailable source status!");
				ACTION_RESULT(false);
			}
		}

		if (manager->cloneSourceAsync(ACTION_DATA(index))) {
			juce::String result;

			result += "Clone Source: [" + juce::String(ACTION_DATA(index)) + "]\n";
			result += "Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n";

			this->output(result);
			ACTION_RESULT(true);
		}
	}
	this->error("Can't Clone Source: [" + juce::String(ACTION_DATA(index)) + "]\n");
	ACTION_RESULT(false);
}

ActionReloadSource::ActionReloadSource(
	int index, const juce::String& path, bool copy)
	: ACTION_DB{ index, path, copy } {}

bool ActionReloadSource::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionReloadSource);
	ACTION_WRITE_DB();
	ACTION_WRITE_STRING(path);

	if (auto manager = CloneableSourceManager::getInstance()) {
		if (auto src = manager->getSource(ACTION_DATA(index))) {
			if (AudioIOList::getInstance()->isTask(src)) {
				this->error("Unavailable source status!");
				ACTION_RESULT(false);
			}
			if (src->isSynthRunning()) {
				this->error("Unavailable source status!");
				ACTION_RESULT(false);
			}
			if (src->checkRecording() && PlayPosition::getInstance()->getPosition()->getIsRecording()) {
				this->error("Unavailable source status!");
				ACTION_RESULT(false);
			}
		}

		if (manager->reloadSourceAsync(ACTION_DATA(index),
			ACTION_DATA(path), ACTION_DATA(copy))) {
			juce::String result;

			result += "Reload Source: [" + juce::String(ACTION_DATA(index)) + "]\n";
			result += "Total Source Num: " + juce::String(manager->getSourceNum()) + "\n";

			this->output(result);
			ACTION_RESULT(true);
		}
	}
	this->error("Can't Reload Source: [" + juce::String(ACTION_DATA(index)) + "]\n");
	ACTION_RESULT(false);
}

ActionSaveSource::ActionSaveSource(
	int index, const juce::String& path)
	: index(index), path(path) {}

bool ActionSaveSource::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto manager = CloneableSourceManager::getInstance()) {
		if (auto src = manager->getSource(this->index)) {
			if (AudioIOList::getInstance()->isTask(src)) {
				this->error("Unavailable source status!");
				ACTION_RESULT(false);
			}
			if (src->isSynthRunning()) {
				this->error("Unavailable source status!");
				ACTION_RESULT(false);
			}
			if (src->checkRecording() && PlayPosition::getInstance()->getPosition()->getIsRecording()) {
				this->error("Unavailable source status!");
				ACTION_RESULT(false);
			}
		}

		if (manager->saveSource(this->index, this->path)) {
			this->output("Save Source Data: " + this->path + "\n");
			return true;
		}
	}
	this->error("Can't Save Source Data: " + this->path + "\n");
	return false;
}

ActionSaveSourceAsync::ActionSaveSourceAsync(
	int index, const juce::String& path)
	: index(index), path(path) {}

bool ActionSaveSourceAsync::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	if (auto manager = CloneableSourceManager::getInstance()) {
		if (auto src = manager->getSource(this->index)) {
			if (AudioIOList::getInstance()->isTask(src)) {
				this->error("Unavailable source status!");
				ACTION_RESULT(false);
			}
			if (src->isSynthRunning()) {
				this->error("Unavailable source status!");
				ACTION_RESULT(false);
			}
		}

		manager->saveSourceAsync(this->index, this->path);

		this->output("Create Save Source Data Task: " + this->path + "\n");
		return true;
	}
	return false;
}

ActionRenderNow::ActionRenderNow(
	const juce::String& path, const juce::String& name,
	const juce::String& extension, const juce::Array<int>& tracks)
	: path(path), name(name), extension(extension), tracks(tracks) {}

bool ActionRenderNow::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_SOURCE_IO_RUNNING(
		"Don't do this while source IO running.");

	if (AudioCore::getInstance()->renderNow(
		this->tracks, this->path, this->name, this->extension)) {
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

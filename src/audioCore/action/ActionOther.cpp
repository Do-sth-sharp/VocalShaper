#include "ActionOther.h"

#include "../AudioCore.h"
#include "../plugin/Plugin.h"
#include "../Utils.h"

ActionClearPlugin::ActionClearPlugin() {}

bool ActionClearPlugin::doAction() {
	if (Plugin::getInstance()->pluginSearchThreadIsRunning()) {
		this->output("Don't clear plugin list while searching plugin.");
		return false;
	}

	Plugin::getInstance()->clearPluginTemporary();

	this->output("Clear plugin list.");
	return true;
}

ActionSearchPlugin::ActionSearchPlugin() {}

bool ActionSearchPlugin::doAction() {
	Plugin::getInstance()->clearPluginList();
	Plugin::getInstance()->getPluginList();

	this->output("Searching Audio Plugin...");
	return true;
}

ActionPlay::ActionPlay() {}

bool ActionPlay::doAction() {
	auto pos = AudioCore::getInstance()->getPosition();
	AudioCore::getInstance()->play();

	this->output("Start play at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n");
	return true;
}

ActionPause::ActionPause() {}

bool ActionPause::doAction() {
	AudioCore::getInstance()->pause();
	auto pos = AudioCore::getInstance()->getPosition();

	this->output("Pause play at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n");
	return true;
}

ActionStop::ActionStop() {}

bool ActionStop::doAction() {
	auto pos = AudioCore::getInstance()->getPosition();
	AudioCore::getInstance()->stop();

	this->output("Stop play at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n");
	return true;
}

ActionRewind::ActionRewind() {}

bool ActionRewind::doAction() {
	AudioCore::getInstance()->rewind();

	this->output("Rewind play\n");
	return true;
}

ActionStartRecord::ActionStartRecord() {}

bool ActionStartRecord::doAction() {
	auto pos = AudioCore::getInstance()->getPosition();
	AudioCore::getInstance()->record(true);

	this->output("Start record at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n");
	return true;
}

ActionStopRecord::ActionStopRecord() {};

bool ActionStopRecord::doAction() {
	auto pos = AudioCore::getInstance()->getPosition();
	AudioCore::getInstance()->record(false);

	this->output("Stop record at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n");
	return true;
}

ActionSynthSource::ActionSynthSource(int index)
	: index(index) {}

bool ActionSynthSource::doAction() {
	if (CloneableSourceManager::getInstance()->synthSource(this->index)) {
		this->output("Start synth source: [" + juce::String(this->index) + "]\n");
		return true;
	}
	this->output("Can't start synth source: [" + juce::String(this->index) + "]\n");
	return false;
}

ActionCloneSource::ActionCloneSource(int index)
	: index(index) {}

bool ActionCloneSource::doAction() {
	if (auto manager = CloneableSourceManager::getInstance()) {
		if (CloneableSourceManager::getInstance()
			->cloneSource(this->index)) {
			juce::String result;

			result += "Clone Source: [" + juce::String(this->index) + "]\n";
			result += "Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n";

			this->output(result);
			return true;
		}
	}
	this->output("Can't Clone Source: [" + juce::String(this->index) + "]\n");
	return false;
}

ActionSaveSource::ActionSaveSource(
	int index, const juce::String& path)
	: index(index), path(path) {}

bool ActionSaveSource::doAction() {
	if (auto manager = CloneableSourceManager::getInstance()) {
		if (CloneableSourceManager::getInstance()
			->saveSource(this->index, this->path)) {
			this->output("Save Source Data: " + this->path + "\n");
			return true;
		}
	}
	this->output("Can't Save Source Data: " + this->path + "\n");
	return false;
}

ActionSaveSourceAsync::ActionSaveSourceAsync(
	int index, const juce::String& path)
	: index(index), path(path) {}

bool ActionSaveSourceAsync::doAction() {
	if (auto manager = CloneableSourceManager::getInstance()) {
		CloneableSourceManager::getInstance()
			->saveSourceAsync(this->index, this->path);

		this->output("Create Save Source Data Task: " + this->path + "\n");
		return true;
	}
	return false;
}

ActionExportSource::ActionExportSource(
	int index, const juce::String& path)
	: index(index), path(path) {}

bool ActionExportSource::doAction() {
	if (auto manager = CloneableSourceManager::getInstance()) {
		if (CloneableSourceManager::getInstance()
			->exportSource(this->index, this->path)) {
			this->output("Export Source Data: " + this->path + "\n");
			return true;
		}
	}
	this->output("Can't Export Source Data: " + this->path + "\n");
	return false;
}

ActionExportSourceAsync::ActionExportSourceAsync(
	int index, const juce::String& path)
	: index(index), path(path) {}

bool ActionExportSourceAsync::doAction() {
	if (auto manager = CloneableSourceManager::getInstance()) {
		CloneableSourceManager::getInstance()
			->exportSourceAsync(this->index, this->path);

		this->output("Create Export Source Data Task: " + this->path + "\n");
		return true;
	}
	return false;
}

ActionRenderNow::ActionRenderNow(
	const juce::String& path, const juce::String& name,
	const juce::String& extension, const juce::Array<int>& tracks)
	: path(path), name(name), extension(extension), tracks(tracks) {}

bool ActionRenderNow::doAction() {
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

	this->output("Can't start to render. Maybe rendering is already started!\n");
	return false;
}

ActionNewProject::ActionNewProject(const juce::String& path)
	: path(path) {}

bool ActionNewProject::doAction() {
	if (AudioCore::getInstance()->newProj(this->path)) {
		this->output("Create new project at: " + this->path + "\n");
		return true;
	}
	this->output("Can't create new project at: " + this->path + "\n");
	return false;
}

ActionSave::ActionSave(const juce::String& name)
	: name(name) {}

bool ActionSave::doAction() {
	if (AudioCore::getInstance()->save(this->name)) {
		this->output("Saved project data to: " + this->name + "\n");
		return true;
	}
	this->output("Can't save project data to: " + this->name + "\n");
	return false;
}

ActionLoad::ActionLoad(const juce::String& path)
	: path(path) {}

bool ActionLoad::doAction() {
	if (AudioCore::getInstance()->load(this->path)) {
		this->output("Load project data from: " + this->path + "\n");
		return true;
	}
	this->output("Can't load project data from: " + this->path + "\n");
	return false;
}

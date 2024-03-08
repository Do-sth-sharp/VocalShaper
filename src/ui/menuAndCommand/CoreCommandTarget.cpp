#include "CoreCommandTarget.h"
#include "GUICommandTarget.h"
#include "CommandTypes.h"
#include "../dataModel/TrackListBoxModel.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

juce::ApplicationCommandTarget* CoreCommandTarget::getNextCommandTarget() {
	return GUICommandTarget::getInstance();
}

void CoreCommandTarget::getAllCommands(
	juce::Array<juce::CommandID>& commands) {
	commands = {
		(juce::CommandID)(CoreCommandType::NewProject),
		(juce::CommandID)(CoreCommandType::OpenProject),
		(juce::CommandID)(CoreCommandType::SaveProject),
		(juce::CommandID)(CoreCommandType::Render),

		(juce::CommandID)(CoreCommandType::Undo),
		(juce::CommandID)(CoreCommandType::Redo),

		(juce::CommandID)(CoreCommandType::Play),
		(juce::CommandID)(CoreCommandType::Stop),
		(juce::CommandID)(CoreCommandType::Record),
		(juce::CommandID)(CoreCommandType::Rewind)
	};
}

void CoreCommandTarget::getCommandInfo(
	juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
	switch ((CoreCommandType)(commandID)) {
	case CoreCommandType::NewProject:
		result.setInfo(TRANS("New Project"), TRANS("Create an empty project."), TRANS("File"), 0);
		result.addDefaultKeypress('n', juce::ModifierKeys::ctrlModifier);
		result.setActive(!(quickAPI::checkRendering() || quickAPI::checkSourceIORunning()));
		break;
	case CoreCommandType::OpenProject:
		result.setInfo(TRANS("Open Project"), TRANS("Open a project from disk."), TRANS("File"), 0);
		result.addDefaultKeypress('o', juce::ModifierKeys::ctrlModifier);
		result.setActive(!(quickAPI::checkRendering() || quickAPI::checkSourceIORunning()
			|| quickAPI::checkPluginLoading() || quickAPI::checkPluginSearching()));
		break;
	case CoreCommandType::SaveProject:
		result.setInfo(TRANS("Save Project"), TRANS("Save current project to disk."), TRANS("File"), 0);
		result.addDefaultKeypress('s', juce::ModifierKeys::ctrlModifier);
		result.setActive(!(quickAPI::checkRendering() || quickAPI::checkSourceIORunning()));
		break;
	case CoreCommandType::Render:
		result.setInfo(TRANS("Render"), TRANS("Render the tracks in current project to audio files on disk."), TRANS("File"), 0);
		result.setActive(!(quickAPI::checkRendering() || quickAPI::checkSourceIORunning()));
		break;

	case CoreCommandType::Undo: {
		bool active = this->checkForUndo();
		juce::String name = TRANS("Undo");
		if (active) {
			name += " - " + this->getUndoName();
		}

		result.setInfo(name, TRANS("Undo the last action."), TRANS("Edit"), 0);
		result.addDefaultKeypress('z', juce::ModifierKeys::ctrlModifier);
		result.setActive(active);
		break;
	}
	case CoreCommandType::Redo: {
		bool active = this->checkForRedo();
		juce::String name = TRANS("Redo");
		if (active) {
			name += " - " + this->getRedoName();
		}

		result.setInfo(name, TRANS("Redo the last undo action."), TRANS("Edit"), 0);
		result.addDefaultKeypress('y', juce::ModifierKeys::ctrlModifier);
		result.addDefaultKeypress('z', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier);
		result.setActive(active);
		break;
	}

	case CoreCommandType::Play:
		result.setInfo(TRANS("Play"), TRANS("Play or pause."), TRANS("Control"), 0);
		result.addDefaultKeypress(juce::KeyPress::spaceKey, juce::ModifierKeys::noModifiers);
		result.setActive(true);
		result.setTicked(this->checkForPlaying());
		break;
	case CoreCommandType::Stop:
		result.setInfo(TRANS("Stop"), TRANS("Stop playing."), TRANS("Control"), 0);
		result.setActive(this->checkForPlaying());
		break;
	case CoreCommandType::Record:
		result.setInfo(TRANS("Record"), TRANS("Record the source."), TRANS("Control"), 0);
		result.setActive(true);
		result.setTicked(this->checkForRecording());
		break;
	case CoreCommandType::Rewind:
		result.setInfo(TRANS("Rewind"), TRANS("Back to start position."), TRANS("Control"), 0);
		result.setActive(true);
		break;
	}
}

bool CoreCommandTarget::perform(
	const juce::ApplicationCommandTarget::InvocationInfo& info) {
	switch ((CoreCommandType)(info.commandID)) {
	case CoreCommandType::NewProject:
		this->newProject();
		return true;
	case CoreCommandType::OpenProject:
		this->openProject();
		return true;
	case CoreCommandType::SaveProject:
		this->saveProject();
		return true;
	case CoreCommandType::Render:
		this->render();
		return true;

	case CoreCommandType::Undo:
		this->undo();
		return true;
	case CoreCommandType::Redo:
		this->redo();
		return true;

	case CoreCommandType::Play:
		this->play();
		return true;
	case CoreCommandType::Stop:
		this->stop();
		return true;
	case CoreCommandType::Record:
		this->record();
		return true;
	case CoreCommandType::Rewind:
		this->rewind();
		return true;
	}

	return false;
}

void CoreCommandTarget::systemRequestOpen(const juce::String& path) {
	CoreActions::loadProjectGUI(path);
}

void CoreCommandTarget::newProject() const {
	CoreActions::newProjectGUI();
}

void CoreCommandTarget::openProject() const {
	CoreActions::loadProjectGUI();
}

void CoreCommandTarget::saveProject() const {
	CoreActions::saveProjectGUI();
}

void CoreCommandTarget::render() const {
	CoreActions::renderGUI();
}

void CoreCommandTarget::undo() const {
	CoreActions::undo();
}

void CoreCommandTarget::redo() const {
	CoreActions::redo();
}

void CoreCommandTarget::play() const {
	bool isPlaying = this->checkForPlaying();

	if (isPlaying) { CoreActions::pause(); }
	else { CoreActions::play(); }
}

void CoreCommandTarget::stop() const {
	CoreActions::stop();
}

void CoreCommandTarget::record() const {
	bool isRecording = this->checkForRecording();

	CoreActions::record(!isRecording);
}

void CoreCommandTarget::rewind() const {
	CoreActions::rewind();
}

bool CoreCommandTarget::checkForUndo() const {
	return CoreActions::canUndo();
}

bool CoreCommandTarget::checkForRedo() const {
	return CoreActions::canRedo();
}

const juce::String CoreCommandTarget::getUndoName() const {
	return CoreActions::getUndoName();
}

const juce::String CoreCommandTarget::getRedoName() const {
	return CoreActions::getRedoName();
}

bool CoreCommandTarget::checkForPlaying() const {
	return quickAPI::isPlaying();
}

bool CoreCommandTarget::checkForRecording() const {
	return quickAPI::isRecording();
}

CoreCommandTarget* CoreCommandTarget::getInstance() {
	return CoreCommandTarget::instance ? CoreCommandTarget::instance
		: (CoreCommandTarget::instance = new CoreCommandTarget{});
}

void CoreCommandTarget::releaseInstance() {
	if (CoreCommandTarget::instance) {
		delete CoreCommandTarget::instance;
		CoreCommandTarget::instance = nullptr;
	}
}

CoreCommandTarget* CoreCommandTarget::instance = nullptr;

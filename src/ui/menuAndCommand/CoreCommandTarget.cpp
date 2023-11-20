﻿#include "CoreCommandTarget.h"
#include "GUICommandTarget.h"
#include "CommandTypes.h"
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
		(juce::CommandID)(CoreCommandType::LoadSource),
		(juce::CommandID)(CoreCommandType::SaveSource),
		(juce::CommandID)(CoreCommandType::ExportSource),
		(juce::CommandID)(CoreCommandType::Render)
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
	case CoreCommandType::LoadSource:
		result.setInfo(TRANS("Load Playing Source"), TRANS("Load a playing source from disk."), TRANS("File"), 0);
		result.setActive(!quickAPI::checkRendering());
		break;
	case CoreCommandType::SaveSource:
		result.setInfo(TRANS("Save Playing Source"), TRANS("Save a playing source to disk."), TRANS("File"), 0);
		result.setActive(!quickAPI::checkRendering());
		break;
	case CoreCommandType::ExportSource:
		result.setInfo(TRANS("Export Synth Source"), TRANS("Save the synth result of a synth source to disk."), TRANS("File"), 0);
		result.setActive(!quickAPI::checkRendering());
		break;
	case CoreCommandType::Render:
		result.setInfo(TRANS("Render"), TRANS("Render the tracks in current project to audio files on disk."), TRANS("File"), 0);
		result.setActive(!quickAPI::checkRendering());
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
	case CoreCommandType::LoadSource:
		/** TODO */
		return true;
	case CoreCommandType::SaveSource:
		/** TODO */
		return true;
	case CoreCommandType::ExportSource:
		/** TODO */
		return true;
	case CoreCommandType::Render:
		/** TODO */
		return true;
	}

	return false;
}

void CoreCommandTarget::newProject() const {
	if (!this->checkForSave()) { return; }

	juce::File defaultPath = quickAPI::getProjectDir();
	juce::FileChooser chooser(TRANS("New Project"), defaultPath);
	if (chooser.browseForDirectory()) {
		juce::File projDir = chooser.getResult();

		auto action = std::unique_ptr<ActionBase>(new ActionNewProject{
		projDir.getFullPathName() });
		ActionDispatcher::getInstance()->dispatch(std::move(action));
	}
}

void CoreCommandTarget::openProject() const {
	if (!this->checkForSave()) { return; }

	juce::File defaultPath = quickAPI::getProjectDir();
	juce::FileChooser chooser(TRANS("Open Project"), defaultPath, "*.vsp4");
	if (chooser.browseForFileToOpen()) {
		juce::File projFile = chooser.getResult();

		auto action = std::unique_ptr<ActionBase>(new ActionLoad{
		projFile.getFullPathName() });
		ActionDispatcher::getInstance()->dispatch(std::move(action));
	}
}

void CoreCommandTarget::saveProject() const {
	juce::File defaultPath = quickAPI::getProjectDir();
	juce::FileChooser chooser(TRANS("Save Project"), defaultPath, "*.vsp4");
	if (chooser.browseForFileToSave(true)) {
		juce::File projFile = chooser.getResult();

		if (projFile.getParentDirectory() != defaultPath) {
			juce::AlertWindow::showMessageBox(
				juce::MessageBoxIconType::WarningIcon, TRANS("Save Project"),
				TRANS("The project file must be in the root of working directory!"));
			return;
		}

		auto action = std::unique_ptr<ActionBase>(new ActionSave{
		projFile.getFileNameWithoutExtension() });
		ActionDispatcher::getInstance()->dispatch(std::move(action));
	}
}

bool CoreCommandTarget::checkForSave() const {
	if (quickAPI::checkProjectSaved() && quickAPI::checkSourcesSaved()) {
		return true;
	}

	return juce::AlertWindow::showOkCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Project Warning"),
		TRANS("Discard unsaved changes and continue?"));
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

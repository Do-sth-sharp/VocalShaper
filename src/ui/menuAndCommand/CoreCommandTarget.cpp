#include "CoreCommandTarget.h"
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
		(juce::CommandID)(CoreCommandType::LoadSynthSource),
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
	case CoreCommandType::LoadSynthSource:
		result.setInfo(TRANS("Load Playing Source (Synth)"), TRANS("Load a synth source from disk."), TRANS("File"), 0);
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
		this->loadSource();
		return true;
	case CoreCommandType::LoadSynthSource:
		this->loadSynthSource();
		return true;
	case CoreCommandType::SaveSource:
		this->saveSource();
		return true;
	case CoreCommandType::ExportSource:
		this->exportSource();
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

void CoreCommandTarget::loadSource() const {
	juce::StringArray audioFormats{ "*.wav", "*.bwf", "*.flac", "*.mp3", "*.ogg", "*.aiff", "*.aif" };
#if JUCE_WINDOWS
	audioFormats.addArray({ "*.wmv", "*.asf", "*.wm", "*.wma" });
#endif
#if JUCE_MAC
	audioFormats.addArray({ "*.aac", "*.m4a", "*.3gp" });
#endif
	juce::StringArray midiFormats{ "*.mid" };

	juce::File defaultPath = quickAPI::getProjectDir();
	juce::FileChooser chooser(TRANS("Load Playing Source"), defaultPath,
		audioFormats.joinIntoString(",") + ";" + midiFormats.joinIntoString(","));
	if (chooser.browseForMultipleFilesToOpen()) {
		int shouldCopy = juce::AlertWindow::showYesNoCancelBox(
			juce::MessageBoxIconType::QuestionIcon, TRANS("Load Playing Source"),
			TRANS("Copy source files to working directory?"));
		if (shouldCopy == 0) { return; }

		auto files = chooser.getResults();
		for (auto& i : files) {
			if (midiFormats.contains("*" + i.getFileExtension())) {
				auto action = std::unique_ptr<ActionBase>(new ActionAddMidiSourceThenLoad{
					i.getFullPathName(), shouldCopy == 1 });
				ActionDispatcher::getInstance()->dispatch(std::move(action));
			}
			else {
				auto action = std::unique_ptr<ActionBase>(new ActionAddAudioSourceThenLoad{
					i.getFullPathName(), shouldCopy == 1 });
				ActionDispatcher::getInstance()->dispatch(std::move(action));
			}
		}
	}
}

void CoreCommandTarget::loadSynthSource() const {
	juce::StringArray midiFormats{ "*.mid" };

	juce::File defaultPath = quickAPI::getProjectDir();
	juce::FileChooser chooser(TRANS("Load Playing Source (Synth)"), defaultPath,
		midiFormats.joinIntoString(","));
	if (chooser.browseForMultipleFilesToOpen()) {
		int shouldCopy = juce::AlertWindow::showYesNoCancelBox(
			juce::MessageBoxIconType::QuestionIcon, TRANS("Load Playing Source (Synth)"),
			TRANS("Copy source files to working directory?"));
		if (shouldCopy == 0) { return; }

		auto files = chooser.getResults();
		for (auto& i : files) {
			auto action = std::unique_ptr<ActionBase>(new ActionAddMidiSourceThenLoad{
					i.getFullPathName(), shouldCopy == 1 });
			ActionDispatcher::getInstance()->dispatch(std::move(action));
		}
	}
}

void CoreCommandTarget::saveSource() const {
	auto callback = [](int index) {
		if (index == -1) { return; }

		juce::StringArray audioFormats{ "*.wav", "*.bwf", "*.flac", "*.mp3", "*.ogg", "*.aiff", "*.aif" };
#if JUCE_WINDOWS
		audioFormats.addArray({ "*.wmv", "*.asf", "*.wm", "*.wma" });
#endif
#if JUCE_MAC
		audioFormats.addArray({ "*.aac", "*.m4a", "*.3gp" });
#endif
		juce::StringArray midiFormats{ "*.mid" };

		auto& formats = quickAPI::checkForAudioSource(index)
			? audioFormats : midiFormats;

		juce::File defaultPath = quickAPI::getProjectDir();
		juce::FileChooser chooser(TRANS("Save Playing Source"), defaultPath,
			formats.joinIntoString(","));
		if (chooser.browseForFileToSave(true)) {
			auto file = chooser.getResult();

			if (!file.isAChildOf(defaultPath)) {
				if (!juce::AlertWindow::showOkCancelBox(
					juce::MessageBoxIconType::QuestionIcon, TRANS("Save Playing Source"),
					TRANS("Saving the source outside of the working directory may cause dependency problem with project files. Continue?"))) {
					return;
				}
			}

			auto action = std::unique_ptr<ActionBase>(new ActionSaveSourceAsync{
			index, file.getFullPathName() });
			ActionDispatcher::getInstance()->dispatch(std::move(action));
		}
	};

	this->selectForSource(callback);
}

void CoreCommandTarget::exportSource() const {
	/** TODO */
}

bool CoreCommandTarget::checkForSave() const {
	if (quickAPI::checkProjectSaved() && quickAPI::checkSourcesSaved()) {
		return true;
	}

	return juce::AlertWindow::showOkCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Project Warning"),
		TRANS("Discard unsaved changes and continue?"));
}

void CoreCommandTarget::selectForSource(const std::function<void(int)>& callback) const {
	/** Get Source List */
	auto sourceList = quickAPI::getSourceNames();
	if (sourceList.isEmpty()) {
		juce::AlertWindow::showMessageBox(
			juce::MessageBoxIconType::WarningIcon, TRANS("Source Selector"),
			TRANS("The source list is empty!"));
		callback(-1);
		return;
	}
	for (int i = 0; i < sourceList.size(); i++) {
		auto& str = sourceList.getReference(i);
		str = "[" + juce::String(i) + "] " + str;
	}

	/** Show Source Chooser */
	auto chooserWindow = new juce::AlertWindow{
		TRANS("Source Selector"), TRANS("Select a source in the list:"),
		juce::MessageBoxIconType::QuestionIcon };
	chooserWindow->addButton(TRANS("OK"), 1);
	chooserWindow->addButton(TRANS("Cancel"), 0);
	chooserWindow->addComboBox(TRANS("Selector"), sourceList);

	auto combo = chooserWindow->getComboBoxComponent(TRANS("Selector"));
	chooserWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[combo, callback, size = sourceList.size()](int result) {
			if (result != 1) { callback(-1); return; }

			int index = combo->getSelectedItemIndex();
			if (index < 0 || index >= size) { callback(-1); return; }

			callback(index);
		}
	), true);
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

#include "CoreActions.h"
#include "../dataModel/TrackListBoxModel.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

void CoreActions::loadProject(const juce::String& filePath) {
	auto action = std::unique_ptr<ActionBase>(new ActionLoad{ filePath });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::newProject(const juce::String& dirPath) {
	auto action = std::unique_ptr<ActionBase>(new ActionNewProject{ dirPath });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::saveProject(const juce::String& fileName) {
	auto action = std::unique_ptr<ActionBase>(new ActionSave{ fileName });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::rescanPlugins() {
	auto clearAction = std::unique_ptr<ActionBase>(new ActionClearPlugin);
	auto searchAction = std::unique_ptr<ActionBase>(new ActionSearchPlugin);
	ActionDispatcher::getInstance()->dispatch(std::move(clearAction));
	ActionDispatcher::getInstance()->dispatch(std::move(searchAction));
}

void CoreActions::loadMIDISource(const juce::String& filePath, bool copy) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionAddMidiSourceThenLoad{ filePath, copy });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::loadAudioSource(const juce::String& filePath, bool copy) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionAddAudioSourceThenLoad{ filePath, copy });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::loadSynthSource(const juce::String& filePath, bool copy) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionAddSynthSourceThenLoad{ filePath, copy });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::saveSource(int index, const juce::String& filePath) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSaveSourceAsync{ index, filePath });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::exportSource(int index, const juce::String& filePath) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionExportSourceAsync{ index, filePath });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::render(const juce::String& dirPath, const juce::String& fileName,
	const juce::String& fileExtension, const juce::Array<int>& tracks) {
	auto action = std::unique_ptr<ActionBase>(new ActionRenderNow{
				dirPath, fileName, fileExtension, tracks });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::undo() {
	ActionDispatcher::getInstance()->performUndo();
}

void CoreActions::redo() {
	ActionDispatcher::getInstance()->performRedo();
}

bool CoreActions::canUndo() {
	auto& manager = ActionDispatcher::getInstance()->getActionManager();
	return manager.canUndo();
}

bool CoreActions::canRedo() {
	auto& manager = ActionDispatcher::getInstance()->getActionManager();
	return manager.canRedo();
}

const juce::String CoreActions::getUndoName() {
	auto& manager = ActionDispatcher::getInstance()->getActionManager();
	return manager.getUndoDescription();
}

const juce::String CoreActions::getRedoName() {
	auto& manager = ActionDispatcher::getInstance()->getActionManager();
	return manager.getRedoDescription();
}

void CoreActions::play() {
	auto action = std::unique_ptr<ActionBase>(new ActionPlay);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::pause() {
	auto action = std::unique_ptr<ActionBase>(new ActionPause);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::stop() {
	auto action = std::unique_ptr<ActionBase>(new ActionStop);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::rewind() {
	auto action = std::unique_ptr<ActionBase>(new ActionRewind);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::record(bool start) {
	auto action = start
		? std::unique_ptr<ActionBase>(new ActionStartRecord)
		: std::unique_ptr<ActionBase>(new ActionStopRecord);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::loadProjectGUI(const juce::String& filePath) {
	if (!CoreActions::askForSaveGUI()) { return; }

	juce::File projFile = utils::getAppRootDir().getChildFile(filePath);
	auto formatsSupport = quickAPI::getProjectFormatsSupported(false);

	if (projFile.existsAsFile() && formatsSupport.contains("*" + projFile.getFileExtension())) {
		CoreActions::loadProject(projFile.getFullPathName());
	}
}

void CoreActions::loadProjectGUI() {
	if (!CoreActions::askForSaveGUI()) { return; }

	auto projectFormats = quickAPI::getProjectFormatsSupported(false);

	juce::File defaultPath = quickAPI::getProjectDir();
	juce::FileChooser chooser(TRANS("Open Project"), defaultPath,
		projectFormats.joinIntoString(","));
	if (chooser.browseForFileToOpen()) {
		juce::File projFile = chooser.getResult();
		CoreActions::loadProject(projFile.getFullPathName());
	}
}

void CoreActions::newProjectGUI() {
	if (!CoreActions::askForSaveGUI()) { return; }

	juce::File defaultPath = quickAPI::getProjectDir();
	juce::FileChooser chooser(TRANS("New Project"), defaultPath);
	if (chooser.browseForDirectory()) {
		juce::File projDir = chooser.getResult();
		CoreActions::newProject(projDir.getFullPathName());
	}
}

void CoreActions::saveProjectGUI() {
	auto projectFormats = quickAPI::getProjectFormatsSupported(true);

	juce::File defaultPath = quickAPI::getProjectDir();
	juce::FileChooser chooser(TRANS("Save Project"), defaultPath,
		projectFormats.joinIntoString(","));
	if (chooser.browseForFileToSave(true)) {
		juce::File projFile = chooser.getResult();
		if (projFile.getFileExtension().isEmpty()) {
			projFile = projFile.withFileExtension(
				projectFormats[0].trimCharactersAtStart("*."));
		}

		if (projFile.getParentDirectory() != defaultPath) {
			juce::AlertWindow::showMessageBox(
				juce::MessageBoxIconType::WarningIcon, TRANS("Save Project"),
				TRANS("The project file must be in the root of working directory!"));
			return;
		}

		CoreActions::saveProject(projFile.getFileNameWithoutExtension());
	}
}

void CoreActions::loadSourceGUI() {
	juce::StringArray audioFormats = quickAPI::getAudioFormatsSupported(false);
	juce::StringArray midiFormats = quickAPI::getMidiFormatsSupported(false);

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
				CoreActions::loadMIDISource(i.getFullPathName(), shouldCopy == 1);
			}
			else {
				CoreActions::loadAudioSource(i.getFullPathName(), shouldCopy == 1);
			}
		}
	}
}

void CoreActions::loadSynthSourceGUI() {
	juce::StringArray midiFormats = quickAPI::getMidiFormatsSupported(false);

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
			CoreActions::loadSynthSource(i.getFullPathName(), shouldCopy == 1);
		}
	}
}

void CoreActions::saveSourceGUI(int index) {
	if (index == -1) { return; }

	juce::StringArray audioFormats = quickAPI::getAudioFormatsSupported(true);
	juce::StringArray midiFormats = quickAPI::getMidiFormatsSupported(true);

	bool isAudio = quickAPI::checkForAudioSource(index);
	auto& formats = isAudio ? audioFormats : midiFormats;

	juce::File defaultPath = quickAPI::getProjectDir();
	juce::FileChooser chooser(TRANS("Save Playing Source"), defaultPath,
		formats.joinIntoString(","));
	if (chooser.browseForFileToSave(true)) {
		auto file = chooser.getResult();
		if (file.getFileExtension().isEmpty()) {
			file = file.withFileExtension(isAudio
				? audioFormats[0].trimCharactersAtStart("*.")
				: midiFormats[0].trimCharactersAtStart("*."));
		}

		if (!file.isAChildOf(defaultPath)) {
			if (!juce::AlertWindow::showOkCancelBox(
				juce::MessageBoxIconType::QuestionIcon, TRANS("Save Playing Source"),
				TRANS("Saving the source outside of the working directory may cause dependency problem with project files. Continue?"))) {
				return;
			}
		}

		CoreActions::saveSource(index, file.getFullPathName());
	}
}

void CoreActions::saveSourceGUI() {
	auto callback = [](int index) {CoreActions::saveSourceGUI(index); };
	CoreActions::askForSourceIndexGUIAsync(callback);
}

void CoreActions::exportSourceGUI(int index) {
	if (index == -1) { return; }

	juce::StringArray audioFormats = quickAPI::getAudioFormatsSupported(true);

	if (!quickAPI::checkForSynthSource(index)) {
		juce::AlertWindow::showMessageBox(
			juce::MessageBoxIconType::WarningIcon, TRANS("Export Synth Source"),
			TRANS("The selected source is not a synth source!"));
		return;
	}

	juce::File defaultPath = quickAPI::getProjectDir();
	juce::FileChooser chooser(TRANS("Export Synth Source"), defaultPath,
		audioFormats.joinIntoString(","));
	if (chooser.browseForFileToSave(true)) {
		auto file = chooser.getResult();
		if (file.getFileExtension().isEmpty()) {
			file = file.withFileExtension(audioFormats[0].trimCharactersAtStart("*."));
		}

		CoreActions::exportSource(index, file.getFullPathName());
	}
}

void CoreActions::exportSourceGUI() {
	auto callback = [](int index) {CoreActions::exportSourceGUI(index); };
	CoreActions::askForSourceIndexGUIAsync(callback);
}

void CoreActions::renderGUI(const juce::Array<int>& tracks) {
	if (tracks.isEmpty()) { return; }

	juce::StringArray audioFormats = quickAPI::getAudioFormatsSupported(true);

	juce::File defaultPath = quickAPI::getProjectDir();
	juce::FileChooser chooser(TRANS("Render"), defaultPath,
		audioFormats.joinIntoString(","));
	if (chooser.browseForFileToSave(false)) {
		auto file = chooser.getResult();
		if (file.getFileExtension().isEmpty()) {
			file = file.withFileExtension(audioFormats[0].trimCharactersAtStart("*."));
		}

		auto dir = file.getParentDirectory();
		if (!dir.findChildFiles(juce::File::TypesOfFileToFind::findFilesAndDirectories, false).isEmpty()) {
			if (!juce::AlertWindow::showOkCancelBox(
				juce::MessageBoxIconType::QuestionIcon, TRANS("Render"),
				TRANS("The selected directory is not empty, which may cause existing files to be overwritten. Continue?"))) {
				return;
			}
		}

		CoreActions::render(dir.getFullPathName(),
			file.getFileNameWithoutExtension(), file.getFileExtension(), tracks);
	}
}

void CoreActions::renderGUI() {
	auto callback = [](const juce::Array<int>& tracks) { CoreActions::renderGUI(tracks); };
	CoreActions::askForMixerTracksListGUIAsync(callback);
}

bool CoreActions::askForSaveGUI() {
	if (quickAPI::checkProjectSaved() && quickAPI::checkSourcesSaved()) {
		return true;
	}

	return juce::AlertWindow::showOkCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Project Warning"),
		TRANS("Discard unsaved changes and continue?"));
}

void CoreActions::askForSourceIndexGUIAsync(
	const std::function<void(int)>& callback) {
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

class TrackListHelper final : private juce::DeletedAtShutdown {
	std::unique_ptr<juce::ListBox> trackListBox = nullptr;
	std::unique_ptr<TrackListBoxModel> trackListBoxModel = nullptr;

public:
	TrackListHelper() {
		/** Track List Box */
		this->trackListBoxModel = std::make_unique<TrackListBoxModel>();
		this->trackListBox = std::make_unique<juce::ListBox>(
			TRANS("Track List"), this->trackListBoxModel.get());
		this->trackListBox->setMultipleSelectionEnabled(true);
		//this->trackListBox->setClickingTogglesRowSelection(true);/**< For Touching */
	};

	~TrackListHelper() {
		this->trackListBox->setModel(nullptr);
	};

	void update(const juce::Array<TrackListBoxModel::TrackInfo>& items) {
		this->trackListBoxModel->setItems(items);
		this->trackListBox->updateContent();
		this->trackListBox->deselectAllRows();
		this->trackListBox->scrollToEnsureRowIsOnscreen(0);
	};

	void setBounds(const juce::Rectangle<int>& bounds) {
		this->trackListBox->setBounds(bounds);
	};

	juce::ListBox* getListBox() const {
		return this->trackListBox.get();
	};

public:
	static TrackListHelper* getInstance();
	static void releaseInstance();

private:
	static TrackListHelper* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListHelper)
};

TrackListHelper* TrackListHelper::getInstance() {
	return TrackListHelper::instance ? TrackListHelper::instance
		: (TrackListHelper::instance = new TrackListHelper{});
}

void TrackListHelper::releaseInstance() {
	if (TrackListHelper::instance) {
		delete TrackListHelper::instance;
		TrackListHelper::instance = nullptr;
	}
}

TrackListHelper* TrackListHelper::instance = nullptr;

void CoreActions::askForMixerTracksListGUIAsync(
	const std::function<void(const juce::Array<int>&)>& callback) {
	/** Get Track List */
	auto trackList = quickAPI::getMixerTrackInfos();
	if (trackList.isEmpty()) {
		juce::AlertWindow::showMessageBox(
			juce::MessageBoxIconType::WarningIcon, TRANS("Mixer Track Selector"),
			TRANS("The track list is empty!"));
		callback({});
		return;
	}

	/** Update ListBox */
	TrackListHelper::getInstance()->update(trackList);

	/** Show Track Chooser */
	auto chooserWindow = new juce::AlertWindow{
		TRANS("Mixer Track Selector"), TRANS("Select tracks in the list: (Use Ctrl and Shift keys to select multiple items)"),
		juce::MessageBoxIconType::QuestionIcon };
	chooserWindow->addButton(TRANS("OK"), 1);
	chooserWindow->addButton(TRANS("Cancel"), 0);

	auto chooserSize = chooserWindow->getLocalBounds();
	TrackListHelper::getInstance()->setBounds(chooserSize);
	chooserWindow->addCustomComponent(TrackListHelper::getInstance()->getListBox());

	chooserWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[callback, listBox = TrackListHelper::getInstance()->getListBox()](int result) {
			if (result != 1) { callback({}); return; }

			juce::Array<int> resList;
			auto resSet = listBox->getSelectedRows();
			auto& resRanges = resSet.getRanges();
			for (auto& i : resRanges) {
				for (int j = i.getStart(); j < i.getEnd(); j++) {
					resList.add(j);
				}
			}

			if (resList.isEmpty()) {
				juce::AlertWindow::showMessageBox(
					juce::MessageBoxIconType::WarningIcon, TRANS("Mixer Track Selector"),
					TRANS("No track selected!"));
				callback({});
				return;
			}

			callback(resList);
		}
	), true);
}

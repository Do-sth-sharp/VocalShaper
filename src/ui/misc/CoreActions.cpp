#include "CoreActions.h"
#include "../component/askfor/ChannelLinkView.h"
#include "../component/askfor/ColorEditor.h"
#include "../component/askfor/TempoEditor.h"
#include "../dataModel/askfor/TrackListBoxModel.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

void CoreActions::loadProject(const juce::String& filePath) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionLoad{ filePath });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::newProject() {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionNewProject{});
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::saveProject(const juce::String& filePath) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSave{ filePath });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::rescanPlugins() {
	auto clearAction = std::unique_ptr<ActionUndoableBase>(new ActionClearPlugin);
	auto searchAction = std::unique_ptr<ActionUndoableBase>(new ActionSearchPlugin);
	ActionDispatcher::getInstance()->dispatch(std::move(clearAction));
	ActionDispatcher::getInstance()->dispatch(std::move(searchAction));
}

void CoreActions::refreshPlugins() {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRefreshPlugin);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

bool CoreActions::addPluginBlackList(const juce::String& filePath) {
	return quickAPI::addToPluginBlackList(filePath);
}

bool CoreActions::removePluginBlackList(const juce::String& filePath) {
	return quickAPI::removeFromPluginBlackList(filePath);
}

bool CoreActions::addPluginSearchPath(const juce::String& path) {
	return quickAPI::addToPluginSearchPath(path);
}

bool CoreActions::removePluginSearchPath(const juce::String& path) {
	return quickAPI::removeFromPluginSearchPath(path);
}

void CoreActions::render(const juce::String& dirPath, const juce::String& fileName,
	const juce::String& fileExtension, const juce::Array<quickAPI::TrackIndex>& tracks,
	const juce::StringPairArray& metaData, int bitDepth, int quality) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRenderNow{
				dirPath, fileName, fileExtension, tracks,
				metaData, bitDepth, quality });
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
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionPlay);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::pause() {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionPause);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::stop() {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionStop);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::rewind() {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRewind);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::record(bool start) {
	auto action = start
		? std::unique_ptr<ActionUndoableBase>(new ActionStartRecord)
		: std::unique_ptr<ActionUndoableBase>(new ActionStopRecord);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::insertInstr(int index, const juce::String& pid, bool addARA) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionAddInstr{ index, pid, addARA });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::bypassInstr(int index, bool bypass) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetInstrBypass{ index, bypass });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::offlineInstr(int index, bool offline) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetInstrOffline{ index, offline });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setInstrMIDIChannel(int index, int channel) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetInstrMidiChannel{ index, channel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setInstrMIDICCIntercept(int index, bool intercept) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetInstrMidiCCIntercept{ index, intercept });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setInstrMIDIOutput(int index, bool output) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetInstrMidiOutput{ index, output });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setInstrParamCCLink(int index, int paramIndex, int ccChannel) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetInstrParamConnectToCC{ index, paramIndex, ccChannel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeInstrParamCCLink(int index, int ccChannel) {
	CoreActions::setInstrParamCCLink(index, -1, ccChannel);
}

void CoreActions::loadInstrPreset(int index, const juce::String& path) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionLoadInstrState{ index, path });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::saveInstrPreset(int index, const juce::String& path) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSaveInstrState{ index, path });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeInstr(int index) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionRemoveInstr{ index });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::insertEffect(
	quickAPI::TrackType type, int track, int index, const juce::String& pid) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionAddEffect{ { type, track }, index, pid });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::bypassEffect(
	quickAPI::TrackType type, int track, int index, bool bypass) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetEffectBypass{ { type, track }, index, bypass });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setEffectMIDIChannel(
	quickAPI::TrackType type, int track, int index, int channel) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetEffectMidiChannel{ { type, track }, index, channel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setEffectMIDICCIntercept(
	quickAPI::TrackType type, int track, int index, bool intercept) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetEffectMidiCCIntercept{ { type, track }, index, intercept });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setEffectMIDIOutput(
	quickAPI::TrackType type, int track, int index, bool output) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetEffectMidiOutput{ { type, track }, index, output });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setEffectParamCCLink(
	quickAPI::TrackType type, int track, int index, int paramIndex, int ccChannel) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetEffectParamConnectToCC{ { type, track }, index, paramIndex, ccChannel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeEffectParamCCLink(
	quickAPI::TrackType type, int track, int index, int ccChannel) {
	CoreActions::setEffectParamCCLink(type, track, index, -1, ccChannel);
}

void CoreActions::loadEffectPreset(
	quickAPI::TrackType type, int track, int index, const juce::String& path) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionLoadEffectState{ { type, track }, index, path });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::saveEffectPreset(
	quickAPI::TrackType type, int track, int index, const juce::String& path) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSaveEffectState{ { type, track }, index, path });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeEffect(
	quickAPI::TrackType type, int track, int index) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionRemoveEffect{ { type, track }, index });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setEffectIndex(
	quickAPI::TrackType type, int track, int oldIndex, int newIndex) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetEffectIndex{ { type, track }, oldIndex, newIndex });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::replaceEffect(
	quickAPI::TrackType type, int track, int index, const juce::String& pid) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetEffect{ { type, track }, index, pid });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::insertTrack(quickAPI::TrackType type, int index, int bus) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionAddTrack{ { type, index }, bus });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackColor(quickAPI::TrackType type, int index, const juce::Colour& color) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetTrackColor{ { type, index }, color });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackName(quickAPI::TrackType type, int index, const juce::String& name) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetTrackName{ { type, index }, name });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::addTrackSideChain(quickAPI::TrackType type, int index) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionAddTrackSideChainBus{ { type, index } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeTrackSideChain(quickAPI::TrackType type, int index) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionRemoveTrackSideChainBus{ { type, index } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::addTrackMIDIInput(quickAPI::TrackType type, int index) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionAddTrackMIDIInput{ { type, index } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::addTrackAudioInput(quickAPI::TrackType type, int index, int srcc, int dstc) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionAddTrackAudioInput{ { type, index }, srcc, dstc });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::addTrackMIDISend(quickAPI::TrackType type, int index, int slot,
	quickAPI::SendDst dst) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionAddTrackMIDISend{ { type, index }, slot, dst });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::addTrackAudioSend(quickAPI::TrackType type, int index, int slot,
	quickAPI::SendDst dst, int srcc, int dstc) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionAddTrackAudioSend{ { type, index }, slot, dst, srcc, dstc });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeTrackMIDIInput(quickAPI::TrackType type, int index) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionRemoveTrackMIDIInput{ { type, index } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeTrackAudioInput(quickAPI::TrackType type, int index, int srcc, int dstc) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionRemoveTrackAudioInput{ { type, index }, srcc, dstc });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeTrackMIDISend(quickAPI::TrackType type, int index, int slot,
	quickAPI::SendDst dst) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionRemoveTrackMIDISend{ { type, index }, slot, dst });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeTrackMIDISendOnSlot(quickAPI::TrackType type, int index, int slot) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionRemoveTrackMIDISendOnSlot{ { type, index }, slot });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeTrackAudioSend(quickAPI::TrackType type, int index, int slot,
	quickAPI::SendDst dst, int srcc, int dstc) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionRemoveTrackAudioSend{ { type, index }, slot, dst, srcc, dstc });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeTrackAudioSendAllChannel(quickAPI::TrackType type, int index, int slot,
	quickAPI::SendDst dst) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionRemoveTrackAudioSendAllChannel{ { type, index }, slot, dst });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeTrackAudioSendOnSlot(quickAPI::TrackType type, int index, int slot) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionRemoveTrackAudioSendOnSlot{ { type, index }, slot });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackGain(quickAPI::TrackType type, int index, float value) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetTrackGain{ { type, index }, value });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackPan(quickAPI::TrackType type, int index, float value) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetTrackPan{ { type, index }, value });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackFader(quickAPI::TrackType type, int index, float value) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetTrackFader{ { type, index }, value });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackMute(quickAPI::TrackType type, int index, bool mute) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetTrackMute{ { type, index }, mute });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackSolo(quickAPI::TrackType type, int index, bool solo) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetTrackSolo{ { type, index }, solo });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackInputMonitoring(int index, bool inputMonitoring) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetTrackInputMonitoring{ index, inputMonitoring });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackRecording(int index, quickAPI::RecordState rec) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetTrackRecording{ index, rec });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackMIDITrack(int index, int midiTrack) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetCurrentMIDITrack{ index, midiTrack });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackAudioRef(int index, const juce::String& path,
	const std::function<void(uint64_t)>& callback) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionLoadAudioSource{ index, path, callback });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackMIDIRef(int index, const juce::String& path,
	bool getTempo, const std::function<void(uint64_t)>& callback) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionLoadMidiSource{ index, path, getTempo, callback });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::createTrackAudioSource(int index, const juce::String& name,
	double sampleRate, int channels, double length) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionInitAudioSource{ index, name, sampleRate, channels, length });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::createTrackMIDISource(int index, const juce::String& name) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionInitMidiSource{ index, name });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeTrack(quickAPI::TrackType type, int index) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionRemoveTrack{ { type, index } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::addTempoLabel(double time, double tempo) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionAddLabelTempo{ time, tempo });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::addBeatLabel(double time, int numerator, int denominator) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionAddLabelBeat{ time, numerator, denominator });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeLabel(int index) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionRemoveLabel{ index });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setLabelTime(int index, double time) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetLabelTime{ index, time });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setLabelTempo(int index, double tempo) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetLabelTempo{ index, tempo });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setLabelBeat(int index, int numerator, int denominator) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionSetLabelBeat{ index, numerator, denominator });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::insertSeqBlock(
	int track, double startTime, double endTime, double offset) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddSequencerBlock{
		track, startTime, endTime, offset });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::splitSeqBlock(int track, int index, double time) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSplitSequencerBlock{
		track, index, time });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setSeqBlock(int track, int index,
	double startTime, double endTime, double offset) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetSequencerBlockTime{
		track, index, { startTime, endTime, offset } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeSeqBlock(int track, int index) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveSequencerBlock{ track, index });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::midiAddNote(
	uint64_t ref, int track, double startTime, double endTime, uint8_t channel,
	uint8_t pitch, uint8_t vel, const juce::String& lyrics) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionMIDIAddNote{ ref, track, startTime, endTime,
		channel, pitch, vel, lyrics });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::midiSetNoteTime(
	uint64_t ref, int track, int index,
	double startTime, double endTime) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionMIDISetNoteTime{
			ref, track, index, startTime, endTime });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::midiSetNoteChannel(
	uint64_t ref, int track, int index,
	uint8_t channel) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionMIDISetNoteChannel{ ref, track, index, channel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::midiSetNotePitch(
	uint64_t ref, int track, int index,
	uint8_t pitch) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionMIDISetNotePitch{ ref, track, index, pitch });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::midiSetNoteVelocity(
	uint64_t ref, int track, int index,
	uint8_t velocity) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionMIDISetNoteVelocity{ ref, track, index, velocity });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::midiSetNoteLyrics(
	uint64_t ref, int track, int index,
	const juce::String& lyrics) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionMIDISetNoteLyrics{ ref, track, index, lyrics });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::midiRemoveNote(
	uint64_t ref, int track, int index) {
	auto action = std::unique_ptr<ActionUndoableBase>(
		new ActionMIDIRemoveNote{ ref, track, index });
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

	CoreActions::newProject();
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

		CoreActions::saveProject(projFile.getFullPathName());
	}
}

bool CoreActions::addPluginBlackListGUI(const juce::String& filePath) {
	if (!CoreActions::addPluginBlackList(filePath)) {
		juce::AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon,
			TRANS("Add Plugin Black List"), TRANS("Do not change plugin configs during plugin search!"));
		return false;
	}
	return true;
}

bool CoreActions::removePluginBlackListGUI(const juce::String& filePath) {
	if (!CoreActions::removePluginBlackList(filePath)) {
		juce::AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon,
			TRANS("Remove Plugin Black List"), TRANS("Do not change plugin configs during plugin search!"));
		return false;
	}
	return true;
}

bool CoreActions::addPluginSearchPathGUI(const juce::String& path) {
	if (!CoreActions::addPluginSearchPath(path)) {
		juce::AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon,
			TRANS("Add Plugin Search Path"), TRANS("Do not change plugin configs during plugin search!"));
		return false;
	}
	return true;
}

bool CoreActions::removePluginSearchPathGUI(const juce::String& path) {
	if (!CoreActions::removePluginSearchPath(path)) {
		juce::AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon,
			TRANS("Remove Plugin Search Path"), TRANS("Do not change plugin configs during plugin search!"));
		return false;
	}
	return true;
}

void CoreActions::renderGUI(const juce::String& dirPath, const juce::String& fileName,
	const juce::String& fileExtension, const juce::StringPairArray& metaData, int bitDepth, int quality) {
	auto callback = [dirPath, fileName, fileExtension, metaData, bitDepth, quality](const juce::Array<quickAPI::TrackIndex>& tracks) {
		CoreActions::render(
			dirPath, fileName, fileExtension, tracks,
			metaData, bitDepth, quality);
		};
	CoreActions::askForMixerTracksListGUIAsync(callback);
}

void CoreActions::renderGUI(const juce::String& dirPath, const juce::String& fileName,
	const juce::String& fileExtension) {
	/** Check Input */
	if (fileName.isEmpty() || fileExtension.isEmpty()) { return; }

	/** Callback */
	auto callback = [dirPath, fileName, fileExtension]
	(bool addMetaData, int bitDepth, int quality) {
		juce::StringPairArray metaData;
		if (addMetaData) {
			metaData = quickAPI::getFormatMetaData(fileExtension);
		}

		CoreActions::renderGUI(
			dirPath, fileName, fileExtension,
			metaData, bitDepth, quality);
		};

	/** Ask For Audio Save Format */
	CoreActions::askForAudioSaveFormatsAsync(callback, fileExtension);
}

void CoreActions::renderGUI(const juce::String& dirPath) {
	/** Check Input */
	if (dirPath.isEmpty()) { return; }
	
	/** Callback */
	auto callback = [dirPath](const juce::String& name, const juce::String& format) {
		CoreActions::renderGUI(dirPath, name, format);
		};

	/** Ask For Audio Save Format */
	CoreActions::askForAudioSaveNameGUIAsync(callback);
}

void CoreActions::renderGUI() {
	/** Choose Dir */
	juce::File defaultPath = quickAPI::getProjectDir();
	juce::FileChooser chooser(TRANS("Render"), defaultPath);
	if (chooser.browseForDirectory()) {
		auto dir = chooser.getResult();
		if (!dir.findChildFiles(juce::File::TypesOfFileToFind::findFilesAndDirectories, false).isEmpty()) {
			if (!juce::AlertWindow::showOkCancelBox(
				juce::MessageBoxIconType::QuestionIcon, TRANS("Render"),
				TRANS("The selected directory is not empty, which may cause existing files to be overwritten. Continue?"))) {
				return;
			}
		}

		/** Render */
		CoreActions::renderGUI(dir.getFullPathName());
	}
}

void CoreActions::insertInstrGUI(int index) {
	auto callback = [index](const juce::String& id, bool addARA) {
		CoreActions::insertInstr(index, id, addARA); };
	CoreActions::askForPluginGUIAsync(callback, true, true);
}

void CoreActions::editInstrParamCCLinkGUI(int index, int paramIndex, int defaultCC) {
	auto callback = [index, paramIndex](int cc) { CoreActions::setInstrParamCCLink(index, paramIndex, cc); };
	CoreActions::askForPluginMIDICCGUIAsync(
		callback, quickAPI::TrackType::Track, index, 0, PluginType::Instr, defaultCC);
}

void CoreActions::addInstrParamCCLinkGUI(int index) {
	auto callback = [index](int param) { CoreActions::editInstrParamCCLinkGUI(index, param); };
	CoreActions::askForPluginParamGUIAsync(
		callback, quickAPI::TrackType::Track, index, 0, PluginType::Instr);
}

void CoreActions::loadInstrPresetGUI(int index) {
	auto callback = [index](const juce::String& path) {
		CoreActions::loadInstrPreset(index, path);
		};

	auto identifier = quickAPI::getInstrIdentifier(index);

	CoreActions::askForPluginPresetAsync(callback, identifier, false);
}

void CoreActions::saveInstrPresetGUI(int index) {
	auto callback = [index](const juce::String& path) {
		CoreActions::saveInstrPreset(index, path);
		};

	auto identifier = quickAPI::getInstrIdentifier(index);

	CoreActions::askForPluginPresetAsync(callback, identifier, true);
}

void CoreActions::removeInstrGUI(int index) {
	if (index <= -1) { return; }

	if (!juce::AlertWindow::showOkCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Remove Instrument"),
		TRANS("Remove the instrument from sequencer track. Continue?"))) {
		return;
	}

	CoreActions::removeInstr(index);
}

void CoreActions::insertEffectGUI(
	quickAPI::TrackType type, int track, int index) {
	auto callback = [type, track, index](const juce::String& id, bool) {
		CoreActions::insertEffect(type, track, index, id); };
	CoreActions::askForPluginGUIAsync(callback, true, false);
}

void CoreActions::editEffectParamCCLinkGUI(
	quickAPI::TrackType type, int track, int index,
	int paramIndex, int defaultCC) {
	auto callback = [type, track, index, paramIndex](int cc) {
		CoreActions::setEffectParamCCLink(type, track, index, paramIndex, cc); };
	CoreActions::askForPluginMIDICCGUIAsync(
		callback, type, track, index, PluginType::Effect, defaultCC);
}

void CoreActions::addEffectParamCCLinkGUI(
	quickAPI::TrackType type, int track, int index) {
	auto callback = [type, track, index](int param) {
		CoreActions::editEffectParamCCLinkGUI(type, track, index, param); };
	CoreActions::askForPluginParamGUIAsync(
		callback, type, track, index, PluginType::Effect);
}

void CoreActions::loadEffectPresetGUI(quickAPI::TrackType type, int track, int index) {
	auto callback = [type, track, index](const juce::String& path) {
		CoreActions::loadEffectPreset(type, track, index, path);
		};

	auto identifier = quickAPI::getEffectIdentifier({ type, track }, index);

	CoreActions::askForPluginPresetAsync(callback, identifier, false);
}

void CoreActions::saveEffectPresetGUI(quickAPI::TrackType type, int track, int index) {
	auto callback = [type, track, index](const juce::String& path) {
		CoreActions::saveEffectPreset(type, track, index, path);
		};

	auto identifier = quickAPI::getEffectIdentifier({ type, track }, index);

	CoreActions::askForPluginPresetAsync(callback, identifier, true);
}

void CoreActions::removeEffectGUI(
	quickAPI::TrackType type, int track, int index) {
	if (track <= -1 || index <= -1) { return; }

	if (!juce::AlertWindow::showOkCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Remove Effect"),
		TRANS("Remove the effect from effect list. Continue?"))) {
		return;
	}

	CoreActions::removeEffect(type, track, index);
}

void CoreActions::insertTrackGUI(quickAPI::TrackType type, int index) {
	auto callback = [type, index](int bus) {
		CoreActions::insertTrack(type, index, bus); };
	CoreActions::askForBusTypeGUIAsync(callback);
}

void CoreActions::insertTrackGUI(quickAPI::TrackType type) {
	int num = quickAPI::getTrackNum(type);
	CoreActions::insertTrackGUI(type, num);
}

void CoreActions::insertTrackGUI() {
	auto callback = [](quickAPI::TrackType type) {
		CoreActions::insertTrackGUI(type); };
	CoreActions::askForTrackTypeGUIAsync(callback, false);
}

void CoreActions::setTrackColorGUI(quickAPI::TrackType type, int index) {
	/** Callback */
	auto callback = [type, index](const juce::Colour& color) {
		CoreActions::setTrackColor(type, index, color);
		};

	/** Get Default Color */
	juce::Colour defaultColor = quickAPI::getTrackColor({ type, index });

	/** Ask For Color */
	CoreActions::askForColorGUIAsync(callback, defaultColor);
}

void CoreActions::setTrackNameGUI(quickAPI::TrackType type, int index) {
	/** Callback */
	auto callback = [type, index](const juce::String& name) {
		CoreActions::setTrackName(type, index, name);
		};

	/** Get Default Name */
	juce::String defaultName = quickAPI::getTrackName({ type, index });

	/** Ask For Name */
	CoreActions::askForNameGUIAsync(callback, defaultName);
}

void CoreActions::setTrackAudioInputGUI(
	quickAPI::TrackType type, int index) {
	/** Callback */
	auto callback = [type, index](int srcc, int dstc, bool input) {
		if (input) {
			CoreActions::addTrackAudioInput(type, index, srcc, dstc);
		}
		else {
			CoreActions::removeTrackAudioInput(type, index, srcc, dstc);
		}
		};

	/** Name */
	juce::String deviceName = quickAPI::getAudioDeviceName(true);
	juce::String trackName = TRANS(quickAPI::getTrackTypeName(type)) + " #" + juce::String{ index } + " " + quickAPI::getTrackName({ type, index });

	/** Channels */
	int deviceTotalChannels = quickAPI::getAudioDeviceChannelNum(true);
	int trackTotalChannels = quickAPI::getTrackInputChannelNum({ type, index });
	auto deviceChannelSet = juce::AudioChannelSet::discreteChannels(deviceTotalChannels);
	auto trackChannelSet = quickAPI::getTrackChannelSet({ type, index });

	/** Init List */
	auto initList = quickAPI::getTrackAudioInputChannels({ type, index });

	/** Ask For Channels */
	CoreActions::askForAudioChannelLinkGUIAsync(callback, initList,
		deviceChannelSet, trackChannelSet, deviceTotalChannels, trackTotalChannels,
		deviceName, trackName, true);
}

void CoreActions::setTrackAudioSendGUI(quickAPI::TrackType type, int index, int slot, quickAPI::SendDst dst) {
	/** Check Current */
	auto currentSend = quickAPI::getTrackAudioSendDst({ type, index }, slot);
	if (currentSend != dst && currentSend.second >= 0) {
		CoreActions::removeTrackAudioSendOnSlot(type, index, slot);
	}

	/** Callback */
	auto callback = [type, index, slot, dst](int srcc, int dstc, bool input) {
		if (input) {
			CoreActions::addTrackAudioSend(type, index, slot, dst, srcc, dstc);
		}
		else {
			CoreActions::removeTrackAudioSend(type, index, slot, dst, srcc, dstc);
		}
		};

	/** Name */
	juce::String trackName = TRANS(quickAPI::getTrackTypeName(type)) + " #" + juce::String{ index } + " " + quickAPI::getTrackName({ type, index });
	juce::String dstName;
	switch (dst.first) {
	case quickAPI::SendDstType::ToDevice: {
		dstName = quickAPI::getAudioDeviceName(false);
		break;
	}
	case quickAPI::SendDstType::ToMaster: {
		dstName = TRANS(quickAPI::getTrackTypeName(quickAPI::TrackType::MasterTrack)) + " #" + juce::String{ 0 } + " " + quickAPI::getTrackName({ quickAPI::TrackType::MasterTrack, 0 });
		break;
	}
	case quickAPI::SendDstType::ToAUX: {
		dstName = TRANS(quickAPI::getTrackTypeName(quickAPI::TrackType::AuxTrack)) + " #" + juce::String{ dst.second } + " " + quickAPI::getTrackName({ quickAPI::TrackType::AuxTrack, dst.second });
		break;
	}
	default:
		break;
	}

	/** Channels */
	int trackTotalChannels = quickAPI::getTrackOutputChannelNum({ type, index });
	auto trackChannelSet = quickAPI::getTrackChannelSet({ type, index });

	int dstTotalChannels = 0;
	switch (dst.first) {
	case quickAPI::SendDstType::ToDevice: {
		dstTotalChannels = quickAPI::getAudioDeviceChannelNum(false);
		break;
	}
	case quickAPI::SendDstType::ToMaster: {
		dstTotalChannels = quickAPI::getTrackInputChannelNum({ quickAPI::TrackType::MasterTrack, 0 });
		break;
	}
	case quickAPI::SendDstType::ToAUX: {
		dstTotalChannels = quickAPI::getTrackInputChannelNum({ quickAPI::TrackType::AuxTrack, dst.second });
		break;
	}
	default:
		break;
	}

	juce::AudioChannelSet dstChannelSet;
	switch (dst.first) {
	case quickAPI::SendDstType::ToDevice: {
		dstChannelSet = juce::AudioChannelSet::discreteChannels(dstTotalChannels);
		break;
	}
	case quickAPI::SendDstType::ToMaster: {
		dstChannelSet = quickAPI::getTrackChannelSet({ quickAPI::TrackType::MasterTrack, 0 });
		break;
	}
	case quickAPI::SendDstType::ToAUX: {
		dstChannelSet = quickAPI::getTrackChannelSet({ quickAPI::TrackType::AuxTrack, dst.second });
		break;
	}
	default:
		break;
	}

	/** Init List */
	auto initList = quickAPI::getTrackAudioSendChannels({ type, index }, slot);

	/** Ask For Channels */
	CoreActions::askForAudioChannelLinkGUIAsync(callback, initList,
		trackChannelSet, dstChannelSet, trackTotalChannels, dstTotalChannels,
		trackName, dstName, true);
}

void CoreActions::setTrackMIDISendGUI(quickAPI::TrackType type, int index, int slot, quickAPI::SendDst dst) {
	/** Check Current */
	auto currentSend = quickAPI::getTrackMIDISendDst({ type, index }, slot);
	if (currentSend == dst) {
		return;
	}
	if (currentSend.second >= 0) {
		CoreActions::removeTrackMIDISendOnSlot(type, index, slot);
	}

	/** Add Send */
	CoreActions::addTrackMIDISend(type, index, slot, dst);
}

void CoreActions::setTrackMIDITrackGUI(int index) {
	/** Callback */
	auto callback = [index](int midiTrack) {
		CoreActions::setTrackMIDITrack(index, midiTrack);
		};

	/** Get Total Track */
	auto ref = quickAPI::getTrackMIDIRef({ quickAPI::TrackType::Track, index });
	int totalMIDITrack = quickAPI::getMIDISourceTrackNum(ref);
	int currentMIDITrack = quickAPI::getTrackCurrentMIDITrack({ quickAPI::TrackType::Track, index });

	/** Get Events Num */
	juce::Array<int> eventsNum;
	for (int i = 0; i < totalMIDITrack; i++) {
		eventsNum.add(quickAPI::getMIDISourceEventNum(ref, i));
	}

	/** Ask For MIDI Track */
	CoreActions::askForMIDITrackAsync(callback, totalMIDITrack, eventsNum, currentMIDITrack);
}

void CoreActions::setTrackAudioRefGUIThenAddBlock(int index, const juce::String& path) {
	/** Add Block When Track Empty */
	[index, path] {
		/** Check Already Has Block */
		if (quickAPI::getBlockNum({ quickAPI::TrackType::Track, index }) > 0) { return; }

		/** Get Audio Format */
		auto audioFormatData = quickAPI::getAudioFormatData(path);

		/** Check Audio Format */
		if (audioFormatData.sampleRate == 0 ||
			audioFormatData.lengthInSamples == 0) {
			return;
		}

		/** Get Length */
		double length = audioFormatData.lengthInSamples / audioFormatData.sampleRate;

		/** Add Block */
		CoreActions::insertSeqBlock(index, 0, length, 0);
		} ();

	/** Load Audio */
	CoreActions::setTrackAudioRef(index, path);
}

void CoreActions::setTrackAudioRefGUI(int index, const juce::String& path) {
	if (index <= -1) { return; }

	if (!juce::AlertWindow::showOkCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Load Audio"),
		TRANS("Load {FILENAME} onto the sequencer track {TRACKINDEX}. Continue?")
		.replace("{TRACKINDEX}", "#" + juce::String{ index })
		.replace("{FILENAME}", path))) {
		return;
	}

	CoreActions::setTrackAudioRefGUIThenAddBlock(index, path);
}

void CoreActions::setTrackMIDIRefGUIThenAddBlock(int index, const juce::String& path, bool getTempo) {
	/** Add Block When Track Empty */
	auto addBlockFunc = [](int index) {
		/** Check Already Has Block */
		if (quickAPI::getBlockNum({ quickAPI::TrackType::Track, index }) > 0) { return; }

		/** Get Length */
		double length = quickAPI::getMIDISourceLength(
			quickAPI::getTrackMIDIRef({ quickAPI::TrackType::Track, index }));

		/** Add Block */
		CoreActions::insertSeqBlock(index, 0, length, 0);
		};

	/** Set Current MIDI Track */
	auto setMIDITrackFunc = [](int index) {
		/** Get Source Ref */
		auto ref = quickAPI::getTrackMIDIRef({ quickAPI::TrackType::Track, index });

		/** Get MIDI Track Num */
		int trackNum = quickAPI::getMIDISourceTrackNum(ref);
		for (int i = 0; i < trackNum; i++) {
			if (!quickAPI::isMIDISourceTrackEmpty(ref, i)) {
				/** Set Current MIDI Track */
				CoreActions::setTrackMIDITrack(index, i);
				break;
			}
		}
		};

	/** Source Loading Callback */
	auto callback = [index, addBlockFunc, setMIDITrackFunc](uint64_t ref) {
		/** Check Index Track Ref Firstly */
		if (quickAPI::isTrackMIDIRef({ quickAPI::TrackType::Track, index }, ref)) {
			addBlockFunc(index);
			setMIDITrackFunc(index);
			return;
		}

		/** Check Each Track */
		int trackNum = quickAPI::getTrackNum(quickAPI::TrackType::Track);
		for (int i = 0; i < trackNum; i++) {
			if (quickAPI::isTrackMIDIRef({ quickAPI::TrackType::Track, i }, ref)) {
				addBlockFunc(i);
				setMIDITrackFunc(index);
				return;
			}
		}
		};

	/** Load MIDI */
	CoreActions::setTrackMIDIRef(index, path, getTempo, callback);
}

void CoreActions::setTrackMIDIRefGUI(int index, const juce::String& path) {
	if (index <= -1) { return; }

	if (!juce::AlertWindow::showOkCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Load MIDI"),
		TRANS("Load {FILENAME} onto the sequencer track {TRACKINDEX}. Continue?")
		.replace("{TRACKINDEX}", "#" + juce::String{ index })
		.replace("{FILENAME}", path))) {
		return;
	}

	int getTempo = juce::AlertWindow::showYesNoCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Load MIDI"),
		TRANS("Get tempo and time meta events from the MIDI file?"));
	if (getTempo == 0) { return; }

	CoreActions::setTrackMIDIRefGUIThenAddBlock(index, path, getTempo == 1);
}

void CoreActions::createTrackAudioSourceGUI(
	int index, const juce::String& name,
	const CreateAudioSourceCancelCallback& cancelCallback) {
	/** Callback */
	auto callback = [index, name](double sampleRate, int channels, double length) {
		CoreActions::createTrackAudioSource(index, name, sampleRate, channels, length);
		};
	auto canceled = [index, cancelCallback] {
		cancelCallback(index);
		};

	/** Ask For Audio Props */
	CoreActions::askForAudioPropGUIAsync(callback, canceled);
}

void CoreActions::createTrackMIDISourceGUI(
	int index, const juce::String& name,
	const CreateMIDISourceCancelCallback& /*cancelCallback*/) {
	CoreActions::createTrackMIDISource(index, name);
}

void CoreActions::createTrackAudioSourceGUI(
	int index, const CreateAudioSourceCancelCallback& cancelCallback) {
	/** Callback */
	auto callback = [index, cancelCallback](const juce::String& name) {
		CoreActions::createTrackAudioSourceGUI(index, name, cancelCallback);
		};
	auto canceled = [index, cancelCallback] {
		cancelCallback(index);
		};

	/** Ask For Source Name */
	CoreActions::askForNameGUIAsync(callback, "", canceled);
}

void CoreActions::createTrackMIDISourceGUI(
	int index, const CreateMIDISourceCancelCallback& cancelCallback) {
	/** Callback */
	auto callback = [index, cancelCallback](const juce::String& name) {
		CoreActions::createTrackMIDISourceGUI(index, name, cancelCallback);
		};
	auto canceled = [index, cancelCallback] {
		cancelCallback(index);
		};

	/** Ask For Source Name */
	CoreActions::askForNameGUIAsync(callback, "", canceled);
}

void CoreActions::removeTrackGUI(quickAPI::TrackType type, int index) {
	if (index <= -1) { return; }

	if (!juce::AlertWindow::showOkCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Remove Track"),
		TRANS("Remove the track from mixer. Continue?"))) {
		return;
	}

	CoreActions::removeTrack(type, index);
}

void CoreActions::addLabelGUI(double time) {
	/** Callback */
	auto callback = [time](bool isTempo, double tempo, int numerator, int denominator) {
		if (isTempo) {
			CoreActions::addTempoLabel(time, tempo);
		}
		else {
			CoreActions::addBeatLabel(time, numerator, denominator);
		}
		};

	/** Ask For Label Data */
	CoreActions::askForTempoGUIAsync(callback);
}

void CoreActions::setLabelTimeGUI(int index, double time) {
	CoreActions::setLabelTime(index, time);
}

void CoreActions::editLabelGUI(int index) {
	/** Callback */
	auto callback = [index](bool isTempo, double tempo, int numerator, int denominator) {
		if (isTempo) {
			CoreActions::setLabelTempo(index, tempo);
		}
		else {
			CoreActions::setLabelBeat(index, numerator, denominator);
		}
		};

	/** Get Current Value */
	bool isTempo = quickAPI::isLabelTempo(index);
	double tempo = 120.0;
	int numerator = 4, denominator = 4;
	if (isTempo) {
		tempo = quickAPI::getLabelTempo(index);
	}
	else {
		std::tie(numerator, denominator) = quickAPI::getLabelBeat(index);
	}

	/** Ask For Label Data */
	CoreActions::askForTempoGUIAsync(callback, isTempo,
		tempo, numerator, denominator, false);
}

void CoreActions::removeLabelGUI(int index) {
	if (!juce::AlertWindow::showOkCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Remove Label"),
		TRANS("Remove the label from time line. Continue?"))) {
		return;
	}

	CoreActions::removeLabel(index);
}

bool CoreActions::askForSaveGUI() {
	if (quickAPI::checkProjectSaved() && quickAPI::checkSourcesSaved()) {
		return true;
	}

	return juce::AlertWindow::showOkCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Project Warning"),
		TRANS("Discard unsaved changes and continue?"));
}

void CoreActions::askForAudioPropGUIAsync(
	const std::function<void(double, int, double)>& callback,
	const CancelCallback& cancelCallback) {
	/** Get Available Props */
	auto availableSampleRate = quickAPI::getSourceSampleRateSupported();
	double currentSampleRate = quickAPI::getCurrentSampleRate();
	juce::StringArray availableSampleRateList;
	for (auto i : availableSampleRate) {
		availableSampleRateList.add(juce::String{ i });
	}

	/** Create Config Window */
	auto configWindow = new juce::AlertWindow{
		TRANS("Audio Properties"), TRANS("Set audio properties:"),
		juce::MessageBoxIconType::QuestionIcon };
	configWindow->addButton(TRANS("OK"), 1);
	configWindow->addButton(TRANS("Cancel"), 0);

	/** Add Prop Comp */
	configWindow->addComboBox(TRANS("Sample Rate"), availableSampleRateList, TRANS("Sample Rate"));
	configWindow->addTextEditor(TRANS("Channels"), juce::String{ 2 }, TRANS("Channels"));
	configWindow->addTextEditor(TRANS("Length"), juce::String{ 60.0 }, TRANS("Length"));

	auto sampleRateSeletor = configWindow->getComboBoxComponent(TRANS("Sample Rate"));
	auto channelsEditor = configWindow->getTextEditor(TRANS("Channels"));
	auto lengthEditor = configWindow->getTextEditor(TRANS("Length"));

	sampleRateSeletor->setText(juce::String{ currentSampleRate });
	channelsEditor->setInputFilter(new utils::TextIntegerFilter{ 0, 1024 }, true);
	lengthEditor->setInputFilter(new utils::TextDoubleFilter{ 0, DBL_MAX, 2 }, true);

	/** Show Window */
	configWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[sampleRateSeletor, channelsEditor, lengthEditor, callback, cancelCallback](int result) {
			if (result != 1) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			double sampleRate = sampleRateSeletor->getText().getDoubleValue();
			int channels = channelsEditor->getText().getIntValue();
			double length = lengthEditor->getText().getDoubleValue();

			callback(sampleRate, channels, length);
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
	const std::function<void(const juce::Array<quickAPI::TrackIndex>&)>& callback,
	const CancelCallback& cancelCallback) {
	/** Get Track List */
	juce::Array<TrackListBoxModel::TrackInfo> trackList;

	auto masterListTemp = quickAPI::getTrackInfos(quickAPI::TrackType::MasterTrack);
	for (int i = 0; i < masterListTemp.size(); i++) {
		auto [name, bus] = masterListTemp[i];
		trackList.add({ (int)quickAPI::TrackType::MasterTrack, i, name, bus });
	}
	
	auto auxListTemp = quickAPI::getTrackInfos(quickAPI::TrackType::AuxTrack);
	for (int i = 0; i < auxListTemp.size(); i++) {
		auto [name, bus] = auxListTemp[i];
		trackList.add({ (int)quickAPI::TrackType::AuxTrack, i, name, bus });
	}

	auto trackListTemp = quickAPI::getTrackInfos(quickAPI::TrackType::Track);
	for (int i = 0; i < trackListTemp.size(); i++) {
		auto [name, bus] = trackListTemp[i];
		trackList.add({ (int)quickAPI::TrackType::Track, i, name, bus });
	}

	if (trackList.isEmpty()) {
		juce::AlertWindow::showMessageBox(
			juce::MessageBoxIconType::WarningIcon, TRANS("Mixer Track Selector"),
			TRANS("The track list is empty!"));
		if (cancelCallback) { cancelCallback(); }
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

	int masterTrackIndexCount = masterListTemp.size();
	int auxTrackIndexCount = auxListTemp.size();
	chooserWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[callback, cancelCallback,
		masterTrackIndexCount, auxTrackIndexCount,
		listBox = TrackListHelper::getInstance()->getListBox()](int result) {
			if (result != 1) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			juce::Array<quickAPI::TrackIndex> resList;
			auto resSet = listBox->getSelectedRows();
			auto& resRanges = resSet.getRanges();
			for (auto& i : resRanges) {
				for (int j = i.getStart(); j < i.getEnd(); j++) {
					if (j < masterTrackIndexCount) {
						resList.add({ quickAPI::TrackType::MasterTrack, j });
					}
					else if (j < masterTrackIndexCount + auxTrackIndexCount) {
						resList.add({ quickAPI::TrackType::AuxTrack, j - masterTrackIndexCount });
					}
					else {
						resList.add({ quickAPI::TrackType::Track, j - masterTrackIndexCount - auxTrackIndexCount });
					}
				}
			}

			if (resList.isEmpty()) {
				juce::AlertWindow::showMessageBox(
					juce::MessageBoxIconType::WarningIcon, TRANS("Mixer Track Selector"),
					TRANS("No track selected!"));
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			callback(resList);
		}
	), true);
}

void CoreActions::askForAudioSaveNameGUIAsync(
	const std::function<void(const juce::String&, const juce::String&)>& callback,
	const CancelCallback& cancelCallback) {
	/** Supported Formats */
	juce::StringArray audioFormats = quickAPI::getAudioFormatsSupported(true);
	for (auto& i : audioFormats) {
		i = i.trimCharactersAtStart("*");
	}

	/** Create Editor */
	auto editorWindow = new juce::AlertWindow{
		TRANS("Save Audio"), TRANS("Select audio saving format and input the file name in the editor:"),
		juce::MessageBoxIconType::QuestionIcon };
	editorWindow->addButton(TRANS("OK"), 1);
	editorWindow->addButton(TRANS("Cancel"), 0);
	editorWindow->addComboBox(TRANS("Audio Format"), audioFormats, TRANS("Audio Format"));
	editorWindow->addTextEditor(TRANS("Name"), "", TRANS("Name"));

	/** Get Editor */
	auto formatEditor = editorWindow->getComboBoxComponent(TRANS("Audio Format"));
	auto nameEditor = editorWindow->getTextEditor(TRANS("Name"));

	/** Show Async */
	editorWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[formatEditor, nameEditor, callback, cancelCallback](int result) {
			if (result != 1) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			juce::String format = formatEditor->getText();
			juce::String name = nameEditor->getText();

			if (name.isEmpty()) {
				juce::AlertWindow::showMessageBox(
					juce::MessageBoxIconType::WarningIcon, TRANS("Save Audio"),
					TRANS("The name must not be empty!"));
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			callback(name, format);
		}
	), true);
}

void CoreActions::askForNameGUIAsync(
	const std::function<void(const juce::String&)>& callback,
	const juce::String& defaultName, const CancelCallback& cancelCallback) {
	/** Create Name Editor */
	auto editorWindow = new juce::AlertWindow{
		TRANS("Name Editor"), TRANS("Input the name in the editor:"),
		juce::MessageBoxIconType::QuestionIcon };
	editorWindow->addButton(TRANS("OK"), 1);
	editorWindow->addButton(TRANS("Cancel"), 0);
	editorWindow->addTextEditor(TRANS("Name"), "", TRANS("Name"));

	auto editor = editorWindow->getTextEditor(TRANS("Name"));
	editor->setText(defaultName);

	/** Show Async */
	editorWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[editor, callback, cancelCallback, defaultName](int result) {
			if (result != 1) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			juce::String name = editor->getText();
			callback(name);
		}
	), true);
}

void CoreActions::askForPluginGUIAsync(
	const std::function<void(const juce::String&, bool)>& callback,
	bool filter, bool instr, const CancelCallback& cancelCallback) {
	/** Get Plugin List */
	auto [result, list] = quickAPI::getPluginList(filter, instr);
	juce::StringArray pluginNames;
	for (auto& i : list) {
		auto name = i.name;
		if (i.hasARAExtension) {
			name += "(ARA)";
		}
		pluginNames.add(name);
	}

	/** Create Selector */
	auto selectorWindow = new juce::AlertWindow{
		TRANS("Plugin Selector"), TRANS("Select a plugin in the list:"),
		juce::MessageBoxIconType::QuestionIcon };
	selectorWindow->addButton(TRANS("OK"), 1);
	selectorWindow->addButton(TRANS("Cancel"), 0);
	selectorWindow->addComboBox(TRANS("Plugin"), pluginNames, TRANS("Plugin"));

	/** Show Selector Async */
	auto combo = selectorWindow->getComboBoxComponent(TRANS("Plugin"));
	selectorWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[combo, callback, cancelCallback, list](int result) {
			if (result != 1) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			int index = combo->getSelectedItemIndex();
			juce::String name = combo->getText();

			auto& pluginDes = list.getReference(index);
			if(pluginDes.name != name){
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			callback(pluginDes.createIdentifierString(), pluginDes.hasARAExtension);
		}
	), true);
}

void CoreActions::askForBusTypeGUIAsync(
	const std::function<void(int)>& callback, int defaultType,
	const CancelCallback& cancelCallback) {
	/** Get Bus Type List */
	auto list = quickAPI::getAllBusTypeWithName();
	juce::StringArray typeNames;
	int defaultIndex = 0;
	for (int i = 0; i < list.size(); i++) {
		auto& [id, name] = list.getReference(i);

		typeNames.add("[" + juce::String{ id } + "] " + name);
		if (id == defaultType) { defaultIndex = i; }
	}

	/** Create Selector */
	auto selectorWindow = new juce::AlertWindow{
		TRANS("Bus Type Selector"), TRANS("Select a bus type in the list:"),
		juce::MessageBoxIconType::QuestionIcon };
	selectorWindow->addButton(TRANS("OK"), 1);
	selectorWindow->addButton(TRANS("Cancel"), 0);
	selectorWindow->addComboBox(TRANS("Bus"), typeNames, TRANS("Bus"));

	/** Set Default Type */
	auto combo = selectorWindow->getComboBoxComponent(TRANS("Bus"));
	combo->setSelectedItemIndex(defaultIndex);

	/** Show Selector Async */
	selectorWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[combo, callback, cancelCallback, list](int result) {
			if (result != 1) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			int index = combo->getSelectedItemIndex();
			auto& [id, name] = list.getReference(index);

			callback(id);
		}
	), true);
}

void CoreActions::askForTrackTypeGUIAsync(
	const std::function<void(quickAPI::TrackType)>& callback,
	bool allowMasterTrack, quickAPI::TrackType defaultType,
	const CancelCallback& cancelCallback) {
	/** Get Track Type List */
	juce::StringArray typeNames;
	typeNames.add(TRANS("Track"));
	typeNames.add(TRANS("AUX Track"));
	typeNames.add(TRANS("Master Track"));

	/** Create Selector */
	auto selectorWindow = new juce::AlertWindow{
		TRANS("Track Type Selector"), TRANS("Select a track type in the list:"),
		juce::MessageBoxIconType::QuestionIcon };
	selectorWindow->addButton(TRANS("OK"), 1);
	selectorWindow->addButton(TRANS("Cancel"), 0);
	selectorWindow->addComboBox(TRANS("Type"), typeNames, TRANS("Type"));

	/** Set Default Type */
	auto combo = selectorWindow->getComboBoxComponent(TRANS("Type"));
	combo->setSelectedItemIndex((int)defaultType);
	combo->setItemEnabled((int)quickAPI::TrackType::MasterTrack + 1, allowMasterTrack);

	/** Show Selector Async */
	selectorWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[combo, callback, cancelCallback, typeNames](int result) {
			if (result != 1) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			int index = combo->getSelectedItemIndex();
			if (index < 0 || index >= typeNames.size()) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			callback((quickAPI::TrackType)index);
		}
	), true);
}

void CoreActions::askForPluginParamGUIAsync(
	const std::function<void(int)>& callback,
	quickAPI::TrackType trackType, int track, int index, PluginType type,
	const CancelCallback& cancelCallback) {
	/** Get Param List */
	juce::StringArray paramList;
	switch (type) {
	case PluginType::Instr:
		paramList = quickAPI::getInstrParamList(track);
		break;
	case PluginType::Effect:
		paramList = quickAPI::getEffectParamList({ trackType, track }, index);
		break;
	}

	juce::StringArray paramItemList;
	for (int i = 0; i < paramList.size(); i++) {
		paramItemList.add("#" + juce::String{ i } + " " + paramList[i]);
	}

	/** Create Selector */
	auto selectorWindow = new juce::AlertWindow{
		TRANS("Plugin Param Selector"), TRANS("Select a parameter in the list:"),
		juce::MessageBoxIconType::QuestionIcon };
	selectorWindow->addButton(TRANS("OK"), 1);
	selectorWindow->addButton(TRANS("Cancel"), 0);
	selectorWindow->addComboBox(TRANS("Parameter"), paramItemList, TRANS("Parameter"));

	/** Show Selector Async */
	auto combo = selectorWindow->getComboBoxComponent(TRANS("Parameter"));
	selectorWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[combo, callback, cancelCallback, size = paramList.size()](int result) {
			if (result != 1) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			int index = combo->getSelectedItemIndex();
			if (index < 0 || index >= size) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			callback(index);
		}
	), true);
}

class ListenableMIDICCCombo final : public juce::ComboBox {
public:
	ListenableMIDICCCombo() = delete;
	ListenableMIDICCCombo(const juce::String& name)
		: ComboBox(name) {
		/** Add Listener */
		auto listener = [comp = juce::Component::SafePointer(this)](int index) {
			if (comp) {
				comp->setSelectedItemIndex(index);
			}
			};
		quickAPI::setMainMIDICCListener(listener);
	};
	~ListenableMIDICCCombo() {
		/** Remove Listener */
		quickAPI::clearMainMIDICCListener();
	};

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListenableMIDICCCombo)
};

void CoreActions::askForPluginMIDICCGUIAsync(
	const std::function<void(int)>& callback,
	quickAPI::TrackType trackType, int track, int index, PluginType type,
	int defaultCCChannel, const CancelCallback& cancelCallback) {
	/** Get MIDI CC List */
	auto ccList = quickAPI::getMIDICCChannelNameList();

	juce::StringArray ccItemList;
	for (int i = 0; i < ccList.size(); i++) {
		juce::String item = "MIDI CC #" + juce::String{ i };
		juce::String name = ccList[i];
		if (name.isNotEmpty()) {
			item += (" (" + name + ")");
		}
		ccItemList.add(item);
	}

	/** Create Selector */
	auto selectorWindow = new juce::AlertWindow{
		TRANS("MIDI CC Controller Selector"), TRANS("Select a MIDI CC controller in the list:"),
		juce::MessageBoxIconType::QuestionIcon };
	selectorWindow->addButton(TRANS("OK"), 1);
	selectorWindow->addButton(TRANS("Cancel"), 0);

	/** Create Combo */
	auto combo = std::make_unique<ListenableMIDICCCombo>(TRANS("CC Controller"));
	combo->addItemList(ccItemList, 1);
	if (defaultCCChannel > -1) {
		combo->setSelectedItemIndex(defaultCCChannel);
	}
	auto chooserSize = selectorWindow->getLocalBounds();
	combo->setBounds(0, 0,
		chooserSize.getWidth() / 6 * 5, chooserSize.getHeight() / 5);
	selectorWindow->addCustomComponent(combo.get());

	/** Show Selector Async */
	selectorWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[combo = std::move(combo), callback, cancelCallback, size = ccList.size()](int result) {
			if (result != 1) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			int index = combo->getSelectedItemIndex();
			if (index < 0 || index >= size) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			callback(index);
		}
	), true);
}

void CoreActions::askForAudioChannelLinkGUIAsync(
	const std::function<void(int, int, bool)>& callback,
	const std::set<std::pair<int, int>>& initList,
	const juce::AudioChannelSet& srcChannels, const juce::AudioChannelSet& dstChannels,
	int srcChannelNum, int dstChannelNum, const juce::String& srcName, const juce::String& dstName,
	bool initIfEmpty, const CancelCallback& /*cancelCallback*/) {
	/** Create Editor */
	auto editor = new ChannelLinkView{
		callback, initList, srcChannels, dstChannels,
		srcChannelNum, dstChannelNum, srcName, dstName, initIfEmpty };

	/** Show Selector Async */
	editor->enterModalState(true, juce::ModalCallbackFunction::create(
		[](int /*result*/) {}), true);
}

void CoreActions::askForColorGUIAsync(
	const std::function<void(const juce::Colour&)>& callback,
	const juce::Colour& defaultColor, const CancelCallback& /*cancelCallback*/) {
	/** Create Editor */
	auto editor = new ColorEditor{ callback, defaultColor };

	/** Show Selector Async */
	editor->enterModalState(true, juce::ModalCallbackFunction::create(
		[](int /*result*/) {}), true);
}

void CoreActions::askForTempoGUIAsync(
	const std::function<void(bool, double, int, int)>& callback,
	bool defaultIsTempo,
	double defaultTempo, int defaultNumerator, int defaultDenominator,
	bool switchable, const CancelCallback& /*cancelCallback*/) {
	/** Create Editor */
	auto editor = new TempoEditor{ defaultIsTempo,
		defaultTempo, defaultNumerator, defaultDenominator, switchable };

	/** Show Selector Async */
	editor->enterModalState(true, juce::ModalCallbackFunction::create(
		[editor, callback](int /*result*/) {
			auto [isTempo, tempo, numerator, denominator] = editor->getResult();
			callback(isTempo, tempo, numerator, denominator);
		}), true);
}

void CoreActions::askForMIDITrackAsync(
	const std::function<void(int)>& callback,
	int totalNum, const juce::Array<int>& eventNum,
	int defaltTrack, const CancelCallback& cancelCallback) {
	/** Get Index List */
	juce::StringArray indexItemList;
	for (int i = 0; i < totalNum; i++) {
		int num = i < eventNum.size() ? eventNum[i] : 0;
		indexItemList.add(juce::String{ i } + " - " + juce::String{ num } + " Events");
	}

	/** Create Selector */
	auto selectorWindow = new juce::AlertWindow{
		TRANS("MIDI Track Selector"), TRANS("Select a MIDI track in the list:"),
		juce::MessageBoxIconType::QuestionIcon };
	selectorWindow->addButton(TRANS("OK"), 1);
	selectorWindow->addButton(TRANS("Cancel"), 0);
	selectorWindow->addComboBox(TRANS("Track"), indexItemList);

	/** Show Selector Async */
	auto combo = selectorWindow->getComboBoxComponent(TRANS("Track"));
	combo->setSelectedItemIndex(defaltTrack);
	selectorWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[combo, callback, cancelCallback, totalNum](int result) {
			if (result != 1) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			int index = combo->getSelectedItemIndex();
			if (index < 0 || index >= totalNum) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			callback(index);
		}
	), true);
}

void CoreActions::askForAudioSaveFormatsAsync(
	const std::function<void(bool, int, int)>& callback,
	const juce::String& format, const CancelCallback& cancelCallback) {
	/** Supported Formats */
	auto bitDepthList = quickAPI::getFormatPossibleBitDepthsForExtension(format);
	auto qualityList = quickAPI::getFormatQualityOptionsForExtension(format);

	juce::StringArray possibleBitDepth;
	for (auto i : bitDepthList) {
		possibleBitDepth.add(juce::String{ i });
	}

	/** Default Formats */
	int bitDepth = quickAPI::getFormatBitsPerSample(format);
	int quality = quickAPI::getFormatQualityOptionIndex(format);

	/** Config Window */
	auto configWindow = new juce::AlertWindow{
		TRANS("Audio Saving Config"), TRANS("Configure audio saving options:"),
		juce::MessageBoxIconType::QuestionIcon };
	configWindow->addButton(TRANS("OK"), 1);
	configWindow->addButton(TRANS("Cancel"), 0);
	configWindow->addComboBox(TRANS("bit-depth"), possibleBitDepth, TRANS("bit-depth"));
	configWindow->addComboBox(TRANS("quality"), qualityList, TRANS("quality"));
	configWindow->addComboBox(TRANS("Meta Data"),
		juce::StringArray{ TRANS("Empty"), TRANS("From Output Config") }, TRANS("Meta Data"));

	/** Default */
	auto bitDepthCombo = configWindow->getComboBoxComponent(TRANS("bit-depth"));
	bitDepthCombo->setSelectedItemIndex(bitDepthList.indexOf(bitDepth));

	auto qualityCombo = configWindow->getComboBoxComponent(TRANS("quality"));
	qualityCombo->setSelectedItemIndex(quality);

	auto metaDataCombo = configWindow->getComboBoxComponent(TRANS("Meta Data"));
	metaDataCombo->setSelectedItemIndex(1);

	/** Show Async */
	configWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[bitDepthCombo, qualityCombo, metaDataCombo, callback, cancelCallback, bitDepthList](int result) {
			if (result != 1) {
				if (cancelCallback) { cancelCallback(); }
				return;
			}

			int bitDepthIndex = bitDepthCombo->getSelectedItemIndex();
			if (bitDepthIndex < 0 || bitDepthIndex >= bitDepthList.size()) { return; }
			int bitDepth = bitDepthList.getUnchecked(bitDepthIndex);

			int quality = qualityCombo->getSelectedItemIndex();
			if (quality < 0) { quality = 0; }

			bool addMetaData = metaDataCombo->getSelectedItemIndex() == 1;

			callback(addMetaData, bitDepth, quality);
		}
	), true);
}

void CoreActions::askForPluginPresetAsync(
	const std::function<void(const juce::String&)>& callback,
	const juce::String& identifier, bool saveMode,
	const CancelCallback& cancelCallback) {
	if (identifier.isEmpty()) {
		if (cancelCallback) { cancelCallback(); }
		return;
	}

	auto presetDir = utils::getPluginPresetDir();
	if (!presetDir.exists()) {
		presetDir.createDirectory();
	}

	auto pluginPresetDir = presetDir.getChildFile("./" + identifier + "/");
	if (!pluginPresetDir.exists()) {
		pluginPresetDir.createDirectory();
	}

	juce::String title = TRANS(saveMode ? "Save Plugin Preset" : "Load Plugin Preset");
	juce::FileChooser chooser(title, pluginPresetDir, "*.pre");
	if (saveMode ? chooser.browseForFileToSave(true) : chooser.browseForFileToOpen()) {
		juce::File presetFile = chooser.getResult();
		if (saveMode && presetFile.getFileExtension().isEmpty()) {
			presetFile = presetFile.withFileExtension("pre");
		}
		callback(presetFile.getFullPathName());
	}
	else {
		if (cancelCallback) { cancelCallback(); }
	}
}

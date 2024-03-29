﻿#include "CoreActions.h"
#include "../component/ChannelLinkView.h"
#include "../component/ColorEditor.h"
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

void CoreActions::insertInstr(int index, const juce::String& pid) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionAddInstr{ index, pid });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::bypassInstr(int index, bool bypass) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetInstrBypass{ index, bypass });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::bypassInstr(quickAPI::PluginHolder instr, bool bypass) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetInstrBypassByPtr{ instr, bypass });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setInstrMIDIChannel(quickAPI::PluginHolder instr, int channel) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetInstrMidiChannelByPtr{ instr, channel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setInstrMIDICCIntercept(quickAPI::PluginHolder instr, bool intercept) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetInstrMidiCCInterceptByPtr{ instr, intercept });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setInstrMIDIOutput(quickAPI::PluginHolder instr, bool output) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetInstrMidiOutputByPtr{ instr, output });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setInstrParamCCLink(quickAPI::PluginHolder instr, int paramIndex, int ccChannel) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetInstrParamConnectToCCByPtr{ instr, paramIndex, ccChannel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeInstrParamCCLink(quickAPI::PluginHolder instr, int ccChannel) {
	CoreActions::setInstrParamCCLink(instr, -1, ccChannel);
}

void CoreActions::removeInstr(int index) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionRemoveInstr{ index });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::insertEffect(int track, int index, const juce::String& pid) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionAddEffect{ track, index, pid });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::bypassEffect(int track, int index, bool bypass) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetEffectBypass{ track, index, bypass });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::bypassEffect(quickAPI::PluginHolder effect, bool bypass) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetEffectBypassByPtr{ effect, bypass });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setEffectMIDIChannel(quickAPI::PluginHolder effect, int channel) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetEffectMidiChannelByPtr{ effect, channel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setEffectMIDICCIntercept(quickAPI::PluginHolder effect, bool intercept) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetEffectMidiCCInterceptByPtr{ effect, intercept });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setEffectMIDIOutput(quickAPI::PluginHolder effect, bool output) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetEffectMidiOutputByPtr{ effect, output });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setEffectParamCCLink(quickAPI::PluginHolder effect, int paramIndex, int ccChannel) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetEffectParamConnectToCCByPtr{ effect, paramIndex, ccChannel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeEffectParamCCLink(quickAPI::PluginHolder effect, int ccChannel) {
	CoreActions::setEffectParamCCLink(effect, -1, ccChannel);
}

void CoreActions::removeEffect(int track, int index) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionRemoveEffect{ track, index });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setEffectIndex(int track, int oldIndex, int newIndex) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetEffectIndex{ track, oldIndex, newIndex });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::insertTrack(int index, int type) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionAddMixerTrack{ index, type });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackColor(int index, const juce::Colour& color) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetMixerTrackColor{ index, color });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackName(int index, const juce::String& name) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionSetMixerTrackName{ index, name });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::addTrackSideChain(int index) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionAddMixerTrackSideChainBus{ index });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::removeTrackSideChain(int index) {
	auto action = std::unique_ptr<ActionBase>(
		new ActionRemoveMixerTrackSideChainBus{ index });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackMIDIInputFromDevice(int index, bool input) {
	auto action = input
		? std::unique_ptr<ActionBase>(new ActionAddMixerTrackMidiInput{ index })
		: std::unique_ptr<ActionBase>(new ActionRemoveMixerTrackMidiInput{ index });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackMIDIInputFromSeqTrack(
	int index, int seqIndex, bool input) {
	auto action = input
		? std::unique_ptr<ActionBase>(new ActionAddSequencerTrackMidiOutputToMixer{ seqIndex, index })
		: std::unique_ptr<ActionBase>(new ActionRemoveSequencerTrackMidiOutputToMixer{ seqIndex, index });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackAudioInputFromDevice(
	int index, int channel, int srcChannel, bool input) {
	auto action = input
		? std::unique_ptr<ActionBase>(new ActionAddMixerTrackInputFromDevice{ srcChannel, index, channel })
		: std::unique_ptr<ActionBase>(new ActionRemoveMixerTrackInputFromDevice{ srcChannel, index, channel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackAudioInputFromSource(
	int index, int channel, int seqIndex, int srcChannel, bool input) {
	auto action = input
		? std::unique_ptr<ActionBase>(new ActionAddSequencerTrackOutput{ seqIndex, srcChannel, index, channel })
		: std::unique_ptr<ActionBase>(new ActionRemoveSequencerTrackOutput{ seqIndex, srcChannel, index, channel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackAudioInputFromSend(
	int index, int channel, int trackIndex, int srcChannel, bool input) {
	auto action = input
		? std::unique_ptr<ActionBase>(new ActionAddMixerTrackSend{ trackIndex, srcChannel, index, channel })
		: std::unique_ptr<ActionBase>(new ActionRemoveMixerTrackSend{ trackIndex, srcChannel, index, channel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackMIDIOutputToDevice(int index, bool output) {
	auto action = output
		? std::unique_ptr<ActionBase>(new ActionAddMixerTrackMidiOutput{ index })
		: std::unique_ptr<ActionBase>(new ActionRemoveMixerTrackMidiOutput{ index });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackAudioOutputToDevice(
	int index, int channel, int dstChannel, bool output) {
	auto action = output
		? std::unique_ptr<ActionBase>(new ActionAddMixerTrackOutput{ index, channel, dstChannel })
		: std::unique_ptr<ActionBase>(new ActionRemoveMixerTrackOutput{ index, channel, dstChannel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackAudioOutputToSend(
	int index, int channel, int trackIndex, int dstChannel, bool output) {
	auto action = output
		? std::unique_ptr<ActionBase>(new ActionAddMixerTrackSend{ index, channel, trackIndex, dstChannel })
		: std::unique_ptr<ActionBase>(new ActionRemoveMixerTrackSend{ index, channel, trackIndex, dstChannel });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackGain(int index, float value) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetMixerTrackGain{ index, value });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackPan(int index, float value) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetMixerTrackPan{ index, value });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackFader(int index, float value) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetMixerTrackSlider{ index, value });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackMute(int index, bool mute) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetMixerTrackMute{ index, mute });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
}

void CoreActions::setTrackSolo(int index) {
	int trackNum = quickAPI::getMixerTrackNum();
	for (int i = 0; i < trackNum; i++) {
		auto action = std::unique_ptr<ActionBase>(new ActionSetMixerTrackMute{ i, i != index });
		ActionDispatcher::getInstance()->dispatch(std::move(action));
	}
}

void CoreActions::setTrackMuteAll(bool mute) {
	int trackNum = quickAPI::getMixerTrackNum();
	for (int i = 0; i < trackNum; i++) {
		auto action = std::unique_ptr<ActionBase>(new ActionSetMixerTrackMute{ i, mute });
		ActionDispatcher::getInstance()->dispatch(std::move(action));
	}
}

void CoreActions::removeTrack(int index) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveMixerTrack{ index });
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

void CoreActions::insertInstrGUI(int index) {
	auto callback = [index](const juce::String& id) {
		CoreActions::insertInstr(index, id); };
	CoreActions::askForPluginGUIAsync(callback, true, true);
}

void CoreActions::editInstrParamCCLinkGUI(quickAPI::PluginHolder instr,
	int paramIndex, int defaultCC) {
	auto callback = [instr, paramIndex](int cc) { CoreActions::setInstrParamCCLink(instr, paramIndex, cc); };
	CoreActions::askForPluginMIDICCGUIAsync(callback, instr, PluginType::Instr, defaultCC);
}

void CoreActions::addInstrParamCCLinkGUI(quickAPI::PluginHolder instr) {
	auto callback = [instr](int param) { CoreActions::editInstrParamCCLinkGUI(instr, param); };
	CoreActions::askForPluginParamGUIAsync(callback, instr, PluginType::Instr);
}

void CoreActions::removeInstrGUI(int index) {
	if (index <= -1) { return; }

	if (!juce::AlertWindow::showOkCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Remove Instrument"),
		TRANS("Remove the instrument from instrument list. Continue?"))) {
		return;
	}

	CoreActions::removeInstr(index);
}

void CoreActions::insertEffectGUI(int track, int index) {
	auto callback = [track, index](const juce::String& id) {
		CoreActions::insertEffect(track, index, id); };
	CoreActions::askForPluginGUIAsync(callback, true, false);
}

void CoreActions::insertEffectGUI(int track) {
	CoreActions::insertEffectGUI(track, quickAPI::getEffectNum(track));
}

void CoreActions::editEffectParamCCLinkGUI(quickAPI::PluginHolder effect,
	int paramIndex, int defaultCC) {
	auto callback = [effect, paramIndex](int cc) { CoreActions::setEffectParamCCLink(effect, paramIndex, cc); };
	CoreActions::askForPluginMIDICCGUIAsync(callback, effect, PluginType::Effect, defaultCC);
}

void CoreActions::addEffectParamCCLinkGUI(quickAPI::PluginHolder effect) {
	auto callback = [effect](int param) { CoreActions::editEffectParamCCLinkGUI(effect, param); };
	CoreActions::askForPluginParamGUIAsync(callback, effect, PluginType::Effect);
}

void CoreActions::removeEffectGUI(int track, int index) {
	if (track <= -1 || index <= -1) { return; }

	if (!juce::AlertWindow::showOkCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Remove Effect"),
		TRANS("Remove the effect from effect list. Continue?"))) {
		return;
	}

	CoreActions::removeEffect(track, index);
}

void CoreActions::insertTrackGUI(int index) {
	auto callback = [index](int type) {
		CoreActions::insertTrack(index, type); };
	CoreActions::askForBusTypeGUIAsync(callback);
}

void CoreActions::insertTrackGUI() {
	int num = quickAPI::getMixerTrackNum();
	CoreActions::insertTrackGUI(num);
}

void CoreActions::setTrackColorGUI(int index) {
	/** Callback */
	auto callback = [index](const juce::Colour& color) {
		CoreActions::setTrackColor(index, color);
		};

	/** Get Default Color */
	juce::Colour defaultColor = quickAPI::getMixerTrackColor(index);

	/** Ask For Color */
	CoreActions::askForColorGUIAsync(callback, defaultColor);
}

void CoreActions::setTrackNameGUI(int index) {
	/** Callback */
	auto callback = [index](const juce::String& name) {
		CoreActions::setTrackName(index, name);
		};

	/** Get Default Name */
	juce::String defaultName = quickAPI::getMixerTrackName(index);

	/** Ask For Name */
	CoreActions::askForNameGUIAsync(callback, defaultName);
}

void CoreActions::setTrackAudioInputFromDeviceGUI(int index, bool input,
	const juce::Array<std::tuple<int, int>>& links) {
	/** Callback */
	auto callback = [index](int srcc, int dstc, bool input) {
		CoreActions::setTrackAudioInputFromDevice(index, dstc, srcc, input);
		};

	/** Remove */
	if (!input) {
		for (auto& [srcc, dstc] : links) {
			callback(srcc, dstc, false);
		}
		return;
	}

	/** Name */
	juce::String deviceName = quickAPI::getAudioDeviceName(true);
	juce::String trackName = TRANS("Mixer Track") + " #" + juce::String{ index } + " " + quickAPI::getMixerTrackName(index);

	/** Channels */
	int deviceTotalChannels = quickAPI::getAudioDeviceChannelNum(true);
	int trackTotalChannels = quickAPI::getMixerTrackInputChannelNum(index);
	auto deviceChannelSet = juce::AudioChannelSet::discreteChannels(deviceTotalChannels);
	auto trackChannelSet = quickAPI::getMixerTrackChannelSet(index);

	/** Ask For Channels */
	CoreActions::askForAudioChannelLinkGUIAsync(callback, links,
		deviceChannelSet, trackChannelSet, deviceTotalChannels, trackTotalChannels,
		deviceName, trackName, true);
}

void CoreActions::setTrackAudioInputFromSourceGUI(int index, int seqIndex, bool input,
	const juce::Array<std::tuple<int, int>>& links) {
	/** Callback */
	auto callback = [index, seqIndex](int srcc, int dstc, bool input) {
		CoreActions::setTrackAudioInputFromSource(index, dstc, seqIndex, srcc, input);
		};

	/** Remove */
	if (!input) {
		for (auto& [srcc, dstc] : links) {
			callback(srcc, dstc, false);
		}
		return;
	}

	/** Name */
	juce::String seqName = TRANS("Sequencer Track") + " #" + juce::String{ seqIndex } + " " + quickAPI::getSeqTrackName(seqIndex);
	juce::String trackName = TRANS("Mixer Track") + " #" + juce::String{ index } + " " + quickAPI::getMixerTrackName(index);

	/** Channels */
	auto seqChannelSet = quickAPI::getSeqTrackChannelSet(seqIndex);
	auto trackChannelSet = quickAPI::getMixerTrackChannelSet(index);
	int seqTotalChannels = quickAPI::getSeqTrackOutputChannelNum(seqIndex);
	int trackTotalChannels = quickAPI::getMixerTrackInputChannelNum(index);

	/** Ask For Channels */
	CoreActions::askForAudioChannelLinkGUIAsync(callback, links,
		seqChannelSet, trackChannelSet, seqTotalChannels, trackTotalChannels,
		seqName, trackName, true);
}

void CoreActions::setTrackAudioInputFromSendGUI(int index, int trackIndex, bool input,
	const juce::Array<std::tuple<int, int>>& links) {
	/** Callback */
	auto callback = [index, trackIndex](int srcc, int dstc, bool input) {
		CoreActions::setTrackAudioInputFromSend(index, dstc, trackIndex, srcc, input);
		};

	/** Remove */
	if (!input) {
		for (auto& [srcc, dstc] : links) {
			callback(srcc, dstc, false);
		}
		return;
	}

	/** Name */
	juce::String sendName = TRANS("Mixer Track") + " #" + juce::String{ trackIndex } + " " + quickAPI::getMixerTrackName(trackIndex);
	juce::String trackName = TRANS("Mixer Track") + " #" + juce::String{ index } + " " + quickAPI::getMixerTrackName(index);

	/** Channels */
	auto sendChannelSet = quickAPI::getMixerTrackChannelSet(trackIndex);
	auto trackChannelSet = quickAPI::getMixerTrackChannelSet(index);
	int sendTotalChannels = quickAPI::getMixerTrackOutputChannelNum(trackIndex);
	int trackTotalChannels = quickAPI::getMixerTrackInputChannelNum(index);

	/** Ask For Channels */
	CoreActions::askForAudioChannelLinkGUIAsync(callback, links,
		sendChannelSet, trackChannelSet, sendTotalChannels, trackTotalChannels,
		sendName, trackName, true);
}

void CoreActions::setTrackAudioOutputToDeviceGUI(int index, bool output,
	const juce::Array<std::tuple<int, int>>& links) {
	/** Callback */
	auto callback = [index](int srcc, int dstc, bool output) {
		CoreActions::setTrackAudioOutputToDevice(index, srcc, dstc, output);
		};

	/** Remove */
	if (!output) {
		for (auto& [srcc, dstc] : links) {
			callback(srcc, dstc, false);
		}
		return;
	}

	/** Name */
	juce::String trackName = TRANS("Mixer Track") + " #" + juce::String{ index } + " " + quickAPI::getMixerTrackName(index);
	juce::String deviceName = quickAPI::getAudioDeviceName(false);

	/** Channels */
	int trackTotalChannels = quickAPI::getMixerTrackOutputChannelNum(index);
	int deviceTotalChannels = quickAPI::getAudioDeviceChannelNum(false);
	auto trackChannelSet = quickAPI::getMixerTrackChannelSet(index);
	auto deviceChannelSet = juce::AudioChannelSet::discreteChannels(deviceTotalChannels);

	/** Ask For Channels */
	CoreActions::askForAudioChannelLinkGUIAsync(callback, links,
		trackChannelSet, deviceChannelSet, trackTotalChannels, deviceTotalChannels,
		trackName, deviceName, true);
}

void CoreActions::setTrackAudioOutputToSendGUI(int index, int trackIndex, bool output,
	const juce::Array<std::tuple<int, int>>& links) {
	/** Callback */
	auto callback = [index, trackIndex](int srcc, int dstc, bool output) {
		CoreActions::setTrackAudioOutputToSend(index, srcc, trackIndex, dstc, output);
		};

	/** Remove */
	if (!output) {
		for (auto& [srcc, dstc] : links) {
			callback(srcc, dstc, false);
		}
		return;
	}

	/** Name */
	juce::String trackName = TRANS("Mixer Track") + " #" + juce::String{ index } + " " + quickAPI::getMixerTrackName(index);
	juce::String sendName = TRANS("Mixer Track") + " #" + juce::String{ trackIndex } + " " + quickAPI::getMixerTrackName(trackIndex);

	/** Channels */
	auto trackChannelSet = quickAPI::getMixerTrackChannelSet(index);
	auto sendChannelSet = quickAPI::getMixerTrackChannelSet(trackIndex);
	int trackTotalChannels = quickAPI::getMixerTrackOutputChannelNum(index);
	int sendTotalChannels = quickAPI::getMixerTrackInputChannelNum(trackIndex);

	/** Ask For Channels */
	CoreActions::askForAudioChannelLinkGUIAsync(callback, links,
		trackChannelSet, sendChannelSet, trackTotalChannels, sendTotalChannels,
		trackName, sendName, true);
}

void CoreActions::removeTrackGUI(int index) {
	if (index <= -1) { return; }

	if (!juce::AlertWindow::showOkCancelBox(
		juce::MessageBoxIconType::QuestionIcon, TRANS("Remove Track"),
		TRANS("Remove the track from mixer. Continue?"))) {
		return;
	}

	CoreActions::removeTrack(index);
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
	const std::function<void(double, int, double)>& callback) {
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
	lengthEditor->setInputFilter(new utils::TextDoubleFilter{ 0, INT_MAX, 2 }, true);

	/** Show Window */
	configWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[sampleRateSeletor, channelsEditor, lengthEditor, callback](int result) {
			if (result != 1) { return; }

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
	const std::function<void(const juce::Array<int>&)>& callback) {
	/** Get Track List */
	auto trackList = quickAPI::getMixerTrackInfos();
	if (trackList.isEmpty()) {
		juce::AlertWindow::showMessageBox(
			juce::MessageBoxIconType::WarningIcon, TRANS("Mixer Track Selector"),
			TRANS("The track list is empty!"));
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
			if (result != 1) { return; }

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
				return;
			}

			callback(resList);
		}
	), true);
}

void CoreActions::askForNameGUIAsync(
	const std::function<void(const juce::String&)>& callback,
	const juce::String& defaultName) {
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
		[editor, callback, defaultName](int result) {
			if (result != 1) { return; }

			juce::String name = editor->getText();
			callback(name);
		}
	), true);
}

void CoreActions::askForPluginGUIAsync(
	const std::function<void(const juce::String&)>& callback,
	bool filter, bool instr) {
	/** Get Plugin List */
	auto [result, list] = quickAPI::getPluginList(filter, instr);
	juce::StringArray pluginNames;
	for (auto& i : list) {
		pluginNames.add(i.name);
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
		[combo, callback, list](int result) {
			if (result != 1) { return; }

			int index = combo->getSelectedItemIndex();
			juce::String name = combo->getText();

			auto& pluginDes = list.getReference(index);
			if(pluginDes.name != name){ return; }

			callback(pluginDes.createIdentifierString());
		}
	), true);
}

void CoreActions::askForBusTypeGUIAsync(
	const std::function<void(int)>& callback, int defaultType) {
	/** Get Track Type List */
	auto list = quickAPI::getAllTrackTypeWithName();
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
	selectorWindow->addComboBox(TRANS("Type"), typeNames, TRANS("Type"));

	/** Set Default Type */
	auto combo = selectorWindow->getComboBoxComponent(TRANS("Type"));
	combo->setSelectedItemIndex(defaultIndex);

	/** Show Selector Async */
	selectorWindow->enterModalState(true, juce::ModalCallbackFunction::create(
		[combo, callback, list](int result) {
			if (result != 1) { return; }

			int index = combo->getSelectedItemIndex();
			auto& [id, name] = list.getReference(index);

			callback(id);
		}
	), true);
}

void CoreActions::askForPluginParamGUIAsync(
	const std::function<void(int)>& callback,
	quickAPI::PluginHolder plugin, PluginType type) {
	/** Get Param List */
	juce::StringArray paramList;
	switch (type) {
	case PluginType::Instr:
		paramList = quickAPI::getInstrParamList(plugin);
		break;
	case PluginType::Effect:
		paramList = quickAPI::getEffectParamList(plugin);
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
		[combo, callback, size = paramList.size()](int result) {
			if (result != 1) { return; }

			int index = combo->getSelectedItemIndex();
			if (index < 0 || index >= size) { return; }

			callback(index);
		}
	), true);
}

class ListenableMIDICCCombo final : public juce::ComboBox {
public:
	ListenableMIDICCCombo() = delete;
	ListenableMIDICCCombo(const juce::String& name,
		quickAPI::PluginHolder plugin, PluginType type)
		: ComboBox(name), plugin(plugin), type(type) {
		/** Add Listener */
		switch (type) {
		case PluginType::Instr:
			quickAPI::setInstrMIDICCListener(plugin,
				[comp = juce::Component::SafePointer(this)](int index) {
					if (comp) {
						comp->setSelectedItemIndex(index);
					}
				}
			);
			break;
		case PluginType::Effect:
			quickAPI::setEffectMIDICCListener(plugin,
				[comp = juce::Component::SafePointer(this)](int index) {
					if (comp) {
						comp->setSelectedItemIndex(index);
					}
				}
			);
			break;
		}
	};
	~ListenableMIDICCCombo() {
		/** Remove Listener */
		switch (this->type) {
		case PluginType::Instr:
			quickAPI::clearInstrMIDICCListener(this->plugin);
			break;
		case PluginType::Effect:
			quickAPI::clearEffectMIDICCListener(this->plugin);
			break;
		}
	};

private:
	const quickAPI::PluginHolder plugin;
	const PluginType type;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListenableMIDICCCombo)
};

void CoreActions::askForPluginMIDICCGUIAsync(
	const std::function<void(int)>& callback,
	quickAPI::PluginHolder plugin, PluginType type,
	int defaultCCChannel) {
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
	auto combo = std::make_unique<ListenableMIDICCCombo>(
		TRANS("CC Controller"), plugin, type);
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
		[combo = std::move(combo), callback, size = ccList.size()](int result) {
			if (result != 1) { return; }

			int index = combo->getSelectedItemIndex();
			if (index < 0 || index >= size) { return; }

			callback(index);
		}
	), true);
}

void CoreActions::askForAudioChannelLinkGUIAsync(
	const std::function<void(int, int, bool)>& callback,
	const juce::Array<std::tuple<int, int>>& initList,
	const juce::AudioChannelSet& srcChannels, const juce::AudioChannelSet& dstChannels,
	int srcChannelNum, int dstChannelNum, const juce::String& srcName, const juce::String& dstName,
	bool initIfEmpty) {
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
	const juce::Colour& defaultColor) {
	/** Create Editor */
	auto editor = new ColorEditor{ callback, defaultColor };

	/** Show Selector Async */
	editor->enterModalState(true, juce::ModalCallbackFunction::create(
		[](int /*result*/) {}), true);
}

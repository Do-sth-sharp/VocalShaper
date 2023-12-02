#include "GUICommandTarget.h"
#include "CommandTypes.h"
#include "../../audioCore/AC_API.h"
#include "../Utils.h"
#include "../component/CompManager.h"
#include <FlowUI.h>

juce::ApplicationCommandTarget* GUICommandTarget::getNextCommandTarget() {
	return nullptr;
}

void GUICommandTarget::getAllCommands(
	juce::Array<juce::CommandID>& commands) {
	commands = {
		(juce::CommandID)(GUICommandType::CloseEditor),

		(juce::CommandID)(GUICommandType::Copy),
		(juce::CommandID)(GUICommandType::Cut),
		(juce::CommandID)(GUICommandType::Paste),
		(juce::CommandID)(GUICommandType::Clipboard),
		(juce::CommandID)(GUICommandType::SelectAll),
		(juce::CommandID)(GUICommandType::Delete),

		(juce::CommandID)(GUICommandType::LoadLayout),
		(juce::CommandID)(GUICommandType::SaveLayout),
		(juce::CommandID)(GUICommandType::PluginView),
		(juce::CommandID)(GUICommandType::SourceView),
		(juce::CommandID)(GUICommandType::TrackView),
		(juce::CommandID)(GUICommandType::InstrView),
		(juce::CommandID)(GUICommandType::MixerView),
		(juce::CommandID)(GUICommandType::SourceEditView),
		(juce::CommandID)(GUICommandType::SourceRecordView),
		(juce::CommandID)(GUICommandType::AudioDebugger),
		(juce::CommandID)(GUICommandType::MidiDebugger),

		(juce::CommandID)(GUICommandType::Help),
		(juce::CommandID)(GUICommandType::Update),
		(juce::CommandID)(GUICommandType::Bilibili),
		(juce::CommandID)(GUICommandType::Github),
		(juce::CommandID)(GUICommandType::Website),
		(juce::CommandID)(GUICommandType::License),
		(juce::CommandID)(GUICommandType::About)
	};
}

void GUICommandTarget::getCommandInfo(
	juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
	switch ((GUICommandType)(commandID)) {
	case GUICommandType::CloseEditor:
		result.setInfo(TRANS("Close Editor"), TRANS("Close and exit VocalShaper."), TRANS("File"), 0);
		result.setActive(!quickAPI::checkRendering());
		break;

	case GUICommandType::Copy:
		result.setInfo(TRANS("Copy"), TRANS("Copy selected items in the actived editor to clipboard."), TRANS("Edit"), 0);
		result.addDefaultKeypress('c', juce::ModifierKeys::ctrlModifier);
		result.setActive(false);
		break;
	case GUICommandType::Cut:
		result.setInfo(TRANS("Cut"), TRANS("Cut selected items in the actived editor to clipboard."), TRANS("Edit"), 0);
		result.addDefaultKeypress('x', juce::ModifierKeys::ctrlModifier);
		result.setActive(false);
		break;
	case GUICommandType::Paste:
		result.setInfo(TRANS("Paste"), TRANS("Paste items to actived editor from clipboard."), TRANS("Edit"), 0);
		result.addDefaultKeypress('v', juce::ModifierKeys::ctrlModifier);
		result.setActive(false);
		break;
	case GUICommandType::Clipboard:
		result.setInfo(TRANS("Clipboard"), TRANS("Show items in clipboard."), TRANS("Edit"), 0);
		result.setActive(false);
		break;
	case GUICommandType::SelectAll:
		result.setInfo(TRANS("Select All"), TRANS("Select all items in the actived editor."), TRANS("Edit"), 0);
		result.addDefaultKeypress('a', juce::ModifierKeys::ctrlModifier);
		result.setActive(false);
		break;
	case GUICommandType::Delete:
		result.setInfo(TRANS("Delete"), TRANS("Delete selected item in the activer editor."), TRANS("Edit"), 0);
		result.addDefaultKeypress(juce::KeyPress::deleteKey, juce::ModifierKeys::noModifiers);
		result.addDefaultKeypress(juce::KeyPress::backspaceKey, juce::ModifierKeys::noModifiers);
		result.setActive(false);
		break;

	case GUICommandType::LoadLayout:
		result.setInfo(TRANS("Load Layout"), TRANS("Load a workspace layout file."), TRANS("View"), 0);
		result.setActive(true);
		break;
	case GUICommandType::SaveLayout:
		result.setInfo(TRANS("Save Layout"), TRANS("Save workspace to layout file."), TRANS("View"), 0);
		result.setActive(true);
		break;
	case GUICommandType::PluginView:
		result.setInfo(TRANS("Plugin"), TRANS("Show plugin view component."), TRANS("View"), 0);
		result.setTicked(CompManager::getInstance()->isOpened(
			CompManager::CompType::PluginView));
		result.setActive(true);
		break;
	case GUICommandType::SourceView:
		result.setInfo(TRANS("Source"), TRANS("Show source view component."), TRANS("View"), 0);
		result.setTicked(CompManager::getInstance()->isOpened(
			CompManager::CompType::SourceView));
		result.setActive(true);
		break;
	case GUICommandType::TrackView:
		result.setInfo(TRANS("Track"), TRANS("Show track view component."), TRANS("View"), 0);
		result.setTicked(CompManager::getInstance()->isOpened(
			CompManager::CompType::TrackView));
		result.setActive(true);
		break;
	case GUICommandType::InstrView:
		result.setInfo(TRANS("Instrument"), TRANS("Show instrument view component."), TRANS("View"), 0);
		result.setTicked(CompManager::getInstance()->isOpened(
			CompManager::CompType::InstrView));
		result.setActive(true);
		break;
	case GUICommandType::MixerView:
		result.setInfo(TRANS("Mixer"), TRANS("Show mixer view component."), TRANS("View"), 0);
		result.setTicked(CompManager::getInstance()->isOpened(
			CompManager::CompType::MixerView));
		result.setActive(true);
		break;
	case GUICommandType::SourceEditView:
		result.setInfo(TRANS("Source Editor"), TRANS("Show source editor component."), TRANS("View"), 0);
		result.setTicked(CompManager::getInstance()->isOpened(
			CompManager::CompType::SourceEditView));
		result.setActive(true);
		break;
	case GUICommandType::SourceRecordView:
		result.setInfo(TRANS("Source Recorder"), TRANS("Show source recorder component."), TRANS("View"), 0);
		result.setTicked(CompManager::getInstance()->isOpened(
			CompManager::CompType::SourceRecordView));
		result.setActive(true);
		break;
	case GUICommandType::AudioDebugger:
		result.setInfo(TRANS("Audio Debugger"), TRANS("Show audio debugger component."), TRANS("View"), 0);
		result.setTicked(CompManager::getInstance()->isOpened(
			CompManager::CompType::AudioDebugger));
		result.setActive(true);
		break;
	case GUICommandType::MidiDebugger:
		result.setInfo(TRANS("MIDI Debugger"), TRANS("Show MIDI debugger component."), TRANS("View"), 0);
		result.setTicked(CompManager::getInstance()->isOpened(
			CompManager::CompType::MidiDebugger));
		result.setActive(true);
		break;

	case GUICommandType::Help:
		result.setInfo(TRANS("Help"), TRANS("Go to our help page."), TRANS("Misc"), 0);
		result.setActive(true);
		break;
	case GUICommandType::Update:
		result.setInfo(TRANS("Update"), TRANS("Check for update."), TRANS("Misc"), 0);
		result.setActive(true);
		break;
	case GUICommandType::Bilibili:
		result.setInfo(TRANS("Bilibili"), TRANS("Go to our bilibili space."), TRANS("Misc"), 0);
		result.setActive(true);
		break;
	case GUICommandType::Github:
		result.setInfo(TRANS("Github"), TRANS("Go to our Github repository."), TRANS("Misc"), 0);
		result.setActive(true);
		break;
	case GUICommandType::Website:
		result.setInfo(TRANS("Website"), TRANS("Go to our official website."), TRANS("Misc"), 0);
		result.setActive(true);
		break;
	case GUICommandType::License:
		result.setInfo(TRANS("License"), TRANS("Read open source licenses."), TRANS("Misc"), 0);
		result.setActive(true);
		break;
	case GUICommandType::About:
		result.setInfo(TRANS("About"), TRANS("About VocalShaper."), TRANS("Misc"), 0);
		result.setActive(true);
		break;
	}
}

bool GUICommandTarget::perform(
	const juce::ApplicationCommandTarget::InvocationInfo& info) {
	switch ((GUICommandType)(info.commandID)) {
	case GUICommandType::CloseEditor:
		this->closeEditor();
		return true;

	case GUICommandType::Copy:
		/** TODO */
		return true;
	case GUICommandType::Cut:
		/** TODO */
		return true;
	case GUICommandType::Paste:
		/** TODO */
		return true;
	case GUICommandType::Clipboard:
		/** TODO */
		return true;
	case GUICommandType::SelectAll:
		/** TODO */
		return true;
	case GUICommandType::Delete:
		/** TODO */
		return true;

	case GUICommandType::LoadLayout:
		this->loadLayout();
		return true;
	case GUICommandType::SaveLayout:
		this->saveLayout();
		return true;
	case GUICommandType::PluginView:
		this->changeOpened(CompManager::CompType::PluginView);
		return true;
	case GUICommandType::SourceView:
		this->changeOpened(CompManager::CompType::SourceView);
		return true;
	case GUICommandType::TrackView:
		this->changeOpened(CompManager::CompType::TrackView);
		return true;
	case GUICommandType::InstrView:
		this->changeOpened(CompManager::CompType::InstrView);
		return true;
	case GUICommandType::MixerView:
		this->changeOpened(CompManager::CompType::MixerView);
		return true;
	case GUICommandType::SourceEditView:
		this->changeOpened(CompManager::CompType::SourceEditView);
		return true;
	case GUICommandType::SourceRecordView:
		this->changeOpened(CompManager::CompType::SourceRecordView);
		return true;
	case GUICommandType::AudioDebugger:
		this->changeOpened(CompManager::CompType::AudioDebugger);
		return true;
	case GUICommandType::MidiDebugger:
		this->changeOpened(CompManager::CompType::MidiDebugger);
		return true;

	case GUICommandType::Help:
		this->help();
		return true;
	case GUICommandType::Update:
		this->update();
		return true;
	case GUICommandType::Bilibili:
		this->bilibili();
		return true;
	case GUICommandType::Github:
		this->github();
		return true;
	case GUICommandType::Website:
		this->website();
		return true;
	case GUICommandType::License:
		this->license();
		return true;
	case GUICommandType::About:
		this->about();
		return true;
	}

	return false;
}

void GUICommandTarget::closeEditor() const {
	if (flowUI::FlowWindowHub::getAppExitHook()()) {
		juce::JUCEApplication::getInstance()->systemRequestedQuit();
	}
}

void GUICommandTarget::loadLayout() const {
	juce::File defaultPath = utils::getLayoutDir();
	juce::FileChooser chooser(TRANS("Load Layout"), defaultPath, "*.json");
	if (chooser.browseForFileToOpen()) {
		juce::File layoutFile = chooser.getResult();
		CompManager::getInstance()->autoLayout(layoutFile.getFullPathName());
		CompManager::getInstance()->maxMainWindow();
	}
}

void GUICommandTarget::saveLayout() const {
	juce::File defaultPath = utils::getLayoutDir();
	juce::FileChooser chooser(TRANS("Save Layout"), defaultPath, "*.json");
	if (chooser.browseForFileToSave(true)) {
		juce::File layoutFile = chooser.getResult();
		if (layoutFile.getFileExtension().isEmpty()) {
			layoutFile = layoutFile.withFileExtension("json");
		}

		CompManager::getInstance()->saveLayout(layoutFile.getFullPathName());
	}
}

void GUICommandTarget::changeOpened(CompManager::CompType type) const {
	if (CompManager::getInstance()->isOpened(type)) {
		CompManager::getInstance()->close(type);
	}
	else {
		CompManager::getInstance()->open(type);
	}
}

void GUICommandTarget::help() const {
	utils::getHelpPage(utils::getAudioPlatformVersionString(),
		utils::getReleaseBranch()).launchInDefaultBrowser();
}

void GUICommandTarget::update() const {
	utils::getUpdatePage(utils::getAudioPlatformVersionString(),
		utils::getReleaseBranch()).launchInDefaultBrowser();
}

void GUICommandTarget::bilibili() const {
	utils::getBilibiliPage().launchInDefaultBrowser();
}

void GUICommandTarget::github() const {
	utils::getGithubPage().launchInDefaultBrowser();
}

void GUICommandTarget::website() const {
	utils::getWebsitePage().launchInDefaultBrowser();
}

void GUICommandTarget::license() const {
	/** TODO */
}

void GUICommandTarget::about() const {
	/** TODO */
}

GUICommandTarget* GUICommandTarget::getInstance() {
	return GUICommandTarget::instance ? GUICommandTarget::instance
		: (GUICommandTarget::instance = new GUICommandTarget{});
}

void GUICommandTarget::releaseInstance() {
	if (GUICommandTarget::instance) {
		delete GUICommandTarget::instance;
		GUICommandTarget::instance = nullptr;
	}
}

GUICommandTarget* GUICommandTarget::instance = nullptr;

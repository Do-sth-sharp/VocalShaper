﻿#include "GUICommandTarget.h"
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
		(juce::CommandID)(GUICommandType::MidiDebugger)
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
		result.setActive(false);
		break;
	case GUICommandType::SourceView:
		result.setInfo(TRANS("Source"), TRANS("Show source view component."), TRANS("View"), 0);
		result.setActive(false);
		break;
	case GUICommandType::TrackView:
		result.setInfo(TRANS("Track"), TRANS("Show track view component."), TRANS("View"), 0);
		result.setActive(false);
		break;
	case GUICommandType::InstrView:
		result.setInfo(TRANS("Instrument"), TRANS("Show instrument view component."), TRANS("View"), 0);
		result.setActive(false);
		break;
	case GUICommandType::MixerView:
		result.setInfo(TRANS("Mixer"), TRANS("Show mixer view component."), TRANS("View"), 0);
		result.setActive(false);
		break;
	case GUICommandType::SourceEditView:
		result.setInfo(TRANS("Source Editor"), TRANS("Show source editor component."), TRANS("View"), 0);
		result.setActive(false);
		break;
	case GUICommandType::SourceRecordView:
		result.setInfo(TRANS("Source Recorder"), TRANS("Show source recorder component."), TRANS("View"), 0);
		result.setActive(false);
		break;
	case GUICommandType::AudioDebugger:
		result.setInfo(TRANS("Audio Debugger"), TRANS("Show audio debugger component."), TRANS("View"), 0);
		result.setActive(false);
		break;
	case GUICommandType::MidiDebugger:
		result.setInfo(TRANS("MIDI Debugger"), TRANS("Show MIDI debugger component."), TRANS("View"), 0);
		result.setActive(false);
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
		/** TODO */
		return true;
	case GUICommandType::SourceView:
		/** TODO */
		return true;
	case GUICommandType::TrackView:
		/** TODO */
		return true;
	case GUICommandType::InstrView:
		/** TODO */
		return true;
	case GUICommandType::MixerView:
		/** TODO */
		return true;
	case GUICommandType::SourceEditView:
		/** TODO */
		return true;
	case GUICommandType::SourceRecordView:
		/** TODO */
		return true;
	case GUICommandType::AudioDebugger:
		/** TODO */
		return true;
	case GUICommandType::MidiDebugger:
		/** TODO */
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
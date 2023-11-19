#include "GUICommandTarget.h"
#include "CommandTypes.h"
#include "../../audioCore/AC_API.h"
#include <FlowUI.h>

juce::ApplicationCommandTarget* GUICommandTarget::getNextCommandTarget() {
	return nullptr;
}

void GUICommandTarget::getAllCommands(
	juce::Array<juce::CommandID>& commands) {
	commands = {
		(juce::CommandID)(GUICommandType::CloseEditor)
	};
}

void GUICommandTarget::getCommandInfo(
	juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
	switch ((GUICommandType)(commandID)) {
	case GUICommandType::CloseEditor:
		result.setInfo(TRANS("Close Editor"), TRANS("Close and exit VocalShaper."), TRANS("File"), 0);
		result.setActive(!quickAPI::checkRendering());
		break;
	}
}

bool GUICommandTarget::perform(
	const juce::ApplicationCommandTarget::InvocationInfo& info) {
	switch ((GUICommandType)(info.commandID)) {
	case GUICommandType::CloseEditor:
		if (flowUI::FlowWindowHub::getAppExitHook()()) {
			juce::JUCEApplication::getInstance()->systemRequestedQuit();
		}
		return true;
	}

	return false;
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

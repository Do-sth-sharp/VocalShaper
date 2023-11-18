#include "GUICommandTarget.h"
#include "CommandTypes.h"

juce::ApplicationCommandTarget* GUICommandTarget::getNextCommandTarget() {
	return nullptr;
}

void GUICommandTarget::getAllCommands(
	juce::Array<juce::CommandID>& commands) {
	/** TODO */
}

void GUICommandTarget::getCommandInfo(
	juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
	/** TODO */
}

bool GUICommandTarget::perform(
	const juce::ApplicationCommandTarget::InvocationInfo& info) {
	/** TODO */
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

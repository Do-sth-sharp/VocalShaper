#include "CoreCommandTarget.h"
#include "GUICommandTarget.h"
#include "CommandTypes.h"

juce::ApplicationCommandTarget* CoreCommandTarget::getNextCommandTarget() {
	return GUICommandTarget::getInstance();
}

void CoreCommandTarget::getAllCommands(
	juce::Array<juce::CommandID>& commands) {
	/** TODO */
}

void CoreCommandTarget::getCommandInfo(
	juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
	/** TODO */
}

bool CoreCommandTarget::perform(
	const juce::ApplicationCommandTarget::InvocationInfo& info) {
	/** TODO */
	return false;
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

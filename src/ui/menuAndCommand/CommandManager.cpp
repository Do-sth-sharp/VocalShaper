#include "CommandManager.h"
#include "CoreCommandTarget.h"
#include "GUICommandTarget.h"

void CommandManager::init() {
	/** First Command Target */
	this->setFirstCommandTarget(CoreCommandTarget::getInstance());

	/** Register Commands */
	this->registerAllCommandsForTarget(CoreCommandTarget::getInstance());
	this->registerAllCommandsForTarget(GUICommandTarget::getInstance());
}

CommandManager* CommandManager::getInstance() {
	return CommandManager::instance ? CommandManager::instance
		: (CommandManager::instance = new CommandManager{});
}

void CommandManager::releaseInstance() {
	if (CommandManager::instance) {
		delete CommandManager::instance;
		CommandManager::instance = nullptr;
	}
}

CommandManager* CommandManager::instance = nullptr;

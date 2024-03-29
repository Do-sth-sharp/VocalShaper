﻿#include "CommandManager.h"
#include "CoreCommandTarget.h"
#include "GUICommandTarget.h"
#include "../misc/CoreCallbacks.h"
#include "../Utils.h"

CommandManager::CommandManager() {
	CoreCallbacks::getInstance()->addPlayingStatus(
		[this](bool) {
			this->commandStatusChanged();
		});
	CoreCallbacks::getInstance()->addRecordingStatus(
		[this](bool) {
			this->commandStatusChanged();
		});
}

CommandManager::~CommandManager() {
	this->stopListening();
}

void CommandManager::init() {
	/** First Command Target */
	this->setFirstCommandTarget(CoreCommandTarget::getInstance());

	/** Register Commands */
	this->registerAllCommandsForTarget(CoreCommandTarget::getInstance());
	this->registerAllCommandsForTarget(GUICommandTarget::getInstance());
}

void CommandManager::startListening() {
	if (auto km = this->getKeyMappings()) {
		km->addChangeListener(this);
	}
}

void CommandManager::stopListening() {
	if (auto km = this->getKeyMappings()) {
		km->removeChangeListener(this);
	}
}

void CommandManager::changeListenerCallback(juce::ChangeBroadcaster* source) {
	/** Save Key Mapping */
	auto file = utils::getKeyMappingFile();
	
	if (auto km = this->getKeyMappings()) {
		auto data = km->createXml(true);
		utils::saveXml(file, data.get());
	}
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

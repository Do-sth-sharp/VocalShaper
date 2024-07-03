#include "SourceManager.h"

SourceManager* SourceManager::getInstance() {
	return SourceManager::instance ? SourceManager::instance 
		: (SourceManager::instance = new SourceManager{});
}

void SourceManager::releaseInstance() {
	if (SourceManager::instance) {
		delete SourceManager::instance;
		SourceManager::instance = nullptr;
	}
}

SourceManager* SourceManager::instance = nullptr;

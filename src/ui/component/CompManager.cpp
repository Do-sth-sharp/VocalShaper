#include "CompManager.h"

CompManager::CompManager() {
	for (int i = 0; i < static_cast<int>(CompType::CompMaxSize); i++) {
		this->compList.add(nullptr);
	}
}

void CompManager::set(CompType type, std::unique_ptr<flowUI::FlowComponent> comp) {
	this->compList.set(static_cast<int>(type), comp.release(), true);
}

flowUI::FlowComponent* CompManager::get(CompType type) const {
	return this->compList.getUnchecked(static_cast<int>(type));
}

CompManager* CompManager::getInstance() {
	return CompManager::instance ? CompManager::instance
		: (CompManager::instance = new CompManager{});
}

void CompManager::releaseInstance() {
	if (CompManager::instance) {
		delete CompManager::instance;
		CompManager::instance = nullptr;
	}
}

CompManager* CompManager::instance = nullptr;

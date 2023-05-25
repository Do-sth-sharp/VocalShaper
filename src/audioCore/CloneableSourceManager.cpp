#include "CloneableSourceManager.h"

void CloneableSourceManager::addSource(std::unique_ptr<CloneableSource> src) {
	this->sourceList.add(std::move(src));
}

void CloneableSourceManager::removeSource(CloneableSource* src) {
	this->sourceList.removeObject(src, true);
}

void CloneableSourceManager::removeSource(int index) {
	this->sourceList.remove(index, true);
}

CloneableSource* CloneableSourceManager::getSource(int index) const {
	return this->sourceList[index];
}

int CloneableSourceManager::getSourceNum() const {
	return this->sourceList.size();
}

CloneableSourceManager* CloneableSourceManager::getInstance() {
	return CloneableSourceManager::instance
		? CloneableSourceManager::instance
		: (CloneableSourceManager::instance = new CloneableSourceManager());
}

void CloneableSourceManager::releaseInstance() {
	if (CloneableSourceManager::instance) {
		delete CloneableSourceManager::instance;
		CloneableSourceManager::instance = nullptr;
	}
}

CloneableSourceManager* CloneableSourceManager::instance = nullptr;

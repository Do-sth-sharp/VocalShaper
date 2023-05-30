#include "CloneableSourceManager.h"

void CloneableSourceManager::addSource(std::unique_ptr<CloneableSource> src) {
	juce::GenericScopedLock locker(this->getLock());
	this->sourceList.add(std::move(src));
}

void CloneableSourceManager::removeSource(CloneableSource* src) {
	juce::GenericScopedLock locker(this->getLock());
	this->sourceList.removeObject(src, true);
}

void CloneableSourceManager::removeSource(int index) {
	juce::GenericScopedLock locker(this->getLock());
	this->sourceList.remove(index, true);
}

CloneableSource::SafePointer<> CloneableSourceManager::getSource(int index) const {
	juce::GenericScopedLock locker(this->getLock());
	return this->sourceList[index];
}

int CloneableSourceManager::getSourceNum() const {
	juce::GenericScopedLock locker(this->getLock());
	return this->sourceList.size();
}

const juce::CriticalSection& CloneableSourceManager::getLock() const {
	return this->sourceList.getLock();
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

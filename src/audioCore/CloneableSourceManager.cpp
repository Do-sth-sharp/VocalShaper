#include "CloneableSourceManager.h"
#include "AudioIOList.h"

bool CloneableSourceManager::addSource(std::unique_ptr<CloneableSource> src) {
	AUDIOCORE_ENSURE_IO_NOT_RUNNING(false);

	juce::GenericScopedLock locker(this->getLock());
	this->sourceList.add(std::move(src));
	return true;
}

bool CloneableSourceManager::removeSource(CloneableSource* src) {
	AUDIOCORE_ENSURE_IO_NOT_RUNNING(false);

	juce::GenericScopedLock locker(this->getLock());
	this->sourceList.removeObject(src, true);
	return true;
}

bool CloneableSourceManager::removeSource(int index) {
	AUDIOCORE_ENSURE_IO_NOT_RUNNING(false);

	juce::GenericScopedLock locker(this->getLock());
	this->sourceList.remove(index, true);
	return true;
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

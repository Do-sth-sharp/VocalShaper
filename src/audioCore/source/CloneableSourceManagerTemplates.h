#include "CloneableSourceManager.h"

template<IsCloneable T>
bool CloneableSourceManager::createNewSource(
	double sampleRate, int channelNum, double length) {
	/** Lock */
	juce::ScopedTryWriteLock locker(this->getLock());
	if (!locker.isLocked()) { return false; }

	/** Create */
	auto source = std::unique_ptr<CloneableSource>(new T);
	auto ptr = source.get();

	/** Add To List */
	if (!this->addSource(std::move(source))) { return false; }

	/** Init */
	ptr->initThis(sampleRate, channelNum, length * this->sampleRate);

	return true;
}

template<IsCloneable T>
bool CloneableSourceManager::createNewSourceThenLoad(
	const juce::String& path) {
	/** Lock */
	juce::ScopedTryWriteLock locker(this->getLock());
	if (!locker.isLocked()) { return false; }

	/** Create */
	auto source = std::unique_ptr<CloneableSource>(new T);
	auto ptr = source.get();

	/** Add To List */
	if (!this->addSource(std::move(source))) { return false; }

	/** Load Sync */
	if (!ptr->loadFrom(
		juce::File::getCurrentWorkingDirectory().getChildFile(path))) {
		this->removeSource(ptr);
		return false;
	}

	return true;
}

template<IsCloneable T>
bool CloneableSourceManager::createNewSourceThenLoadAsync(
	const juce::String& path) {
	/** Lock */
	juce::ScopedTryWriteLock locker(this->getLock());
	if (!locker.isLocked()) { return false; }

	/** Create */
	auto source = std::unique_ptr<CloneableSource>(new T);
	auto ptr = source.get();

	/** Add To List */
	if (!this->addSource(std::move(source))) { return false; }

	/** Load Async */
	AudioIOList::getInstance()->load(CloneableSource::SafePointer<>{ ptr }, path);

	return true;
}

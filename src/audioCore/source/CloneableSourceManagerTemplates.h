#include "CloneableSourceManager.h"

template<IsCloneable T>
bool CloneableSourceManager::createNewSource(
	double sampleRate, int channelNum, double length) {
	/** Lock */
	juce::ScopedWriteLock locker(this->getLock());

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
	const juce::String& path, bool copy) {
	/** Lock */
	juce::ScopedWriteLock locker(this->getLock());

	/** Create */
	auto source = std::unique_ptr<CloneableSource>(new T);
	auto ptr = source.get();

	/** Add To List */
	if (!this->addSource(std::move(source))) { return false; }

	/** Copy */
	juce::File srcFile = utils::getSourceFile(path);
	juce::String dstPath = copy ? ("./" + srcFile.getFileName()) : path;
	juce::File dstFile = utils::getSourceFile(dstPath);
	if (srcFile.getFullPathName() != dstFile.getFullPathName()) {
		if (!srcFile.copyFileTo(dstFile)) {
			this->removeSource(ptr);
			return false;
		}
	}

	/** Load Sync */
	if (!ptr->loadFrom(dstFile)) {
		this->removeSource(ptr);
		return false;
	}

	ptr->setPath(dstPath);
	return true;
}

template<IsCloneable T>
bool CloneableSourceManager::createNewSourceThenLoadAsync(
	const juce::String& path, bool copy) {
	/** Lock */
	juce::ScopedWriteLock locker(this->getLock());

	/** Create */
	auto source = std::unique_ptr<CloneableSource>(new T);
	auto ptr = source.get();

	/** Add To List */
	if (!this->addSource(std::move(source))) { return false; }

	/** Load Async */
	AudioIOList::getInstance()->load(CloneableSource::SafePointer<>{ ptr }, path, copy);

	return true;
}

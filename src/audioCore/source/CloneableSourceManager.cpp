#include "CloneableSourceManager.h"
#include "AudioIOList.h"
#include "../AudioCore.h"
#include "../plugin/PluginLoader.h"

bool CloneableSourceManager::addSource(std::unique_ptr<CloneableSource> src) {
	juce::ScopedTryWriteLock locker(this->getLock());
	if (locker.isLocked()) {
		src->prepareToPlay(this->sampleRate, this->bufferSize);
		this->sourceList.add(std::move(src));
		return true;
	}
	return false;
}

bool CloneableSourceManager::removeSource(CloneableSource* src) {
	juce::ScopedTryWriteLock locker(this->getLock());
	if (locker.isLocked()) {
		this->sourceList.removeObject(src, true);
		return true;
	}
	return false;
}

bool CloneableSourceManager::removeSource(int index) {
	juce::ScopedTryWriteLock locker(this->getLock());
	if (locker.isLocked()) {
		this->sourceList.remove(index, true);
		return true;
	}
	return false;
}

CloneableSource::SafePointer<> CloneableSourceManager::getSource(int index) const {
	juce::ScopedTryReadLock locker(this->getLock());
	if (locker.isLocked()) {
		return this->sourceList[index];
	}
	return nullptr;
}

int CloneableSourceManager::getSourceNum() const {
	juce::ScopedTryReadLock locker(this->getLock());
	if (locker.isLocked()) {
		return this->sourceList.size();
	}
	return 0;
}

juce::ReadWriteLock& CloneableSourceManager::getLock() const {
	return this->lock;
}

bool CloneableSourceManager::setSourceSynthesizer(
	int index, const juce::String& identifier) {
	juce::ScopedTryReadLock locker(this->getLock());
	if (locker.isLocked()) {
		auto source = this->getSource(index);
		if (auto src = dynamic_cast<CloneableSynthSource*>(source.getSource())) {
			if (auto des = AudioCore::getInstance()->findPlugin(identifier, true)) {
				PluginLoader::getInstance()->loadPlugin(*(des.get()),
					CloneableSource::SafePointer<CloneableSynthSource>(src));
				return true;
			}
		}
	}
	return false;
}

bool CloneableSourceManager::synthSource(int index) {
	juce::ScopedTryReadLock locker(this->getLock());
	if (locker.isLocked()) {
		auto source = this->getSource(index);
		if (auto src = dynamic_cast<CloneableSynthSource*>(source.getSource())) {
			src->synth();
			return true;
		}
	}
	return false;
}

bool CloneableSourceManager::cloneSource(int index) {
	/** Lock */
	juce::ScopedTryWriteLock locker(this->getLock());
	if (locker.isLocked()) {
		/** Clone */
		if (auto source = this->getSource(index)) {
			if (auto ptr = source->cloneThis()) {
				return this->addSource(std::move(ptr));
			}
		}
	}
	return false;
}

bool CloneableSourceManager::saveSource(
	int index, const juce::String& path) const {
	/** Lock */
	juce::ScopedTryReadLock locker(this->getLock());
	if (!locker.isLocked()) { return false; }

	/** Get Source */
	if (auto src = this->getSource(index)) {
		return src->saveAs(
			juce::File::getCurrentWorkingDirectory().getChildFile(path));
	}

	return false;
}

bool CloneableSourceManager::saveSourceAsync(
	int index, const juce::String& path) const {
	/** Lock */
	juce::ScopedTryReadLock locker(this->getLock());
	if (!locker.isLocked()) { return false; }

	/** Get Source */
	if (auto src = this->getSource(index)) {
		AudioIOList::getInstance()->save(src, path);
		return true;
	}

	return false;
}

bool CloneableSourceManager::exportSource(
	int index, const juce::String& path) const {
	/** Lock */
	juce::ScopedTryReadLock locker(this->getLock());
	if (!locker.isLocked()) { return false; }

	/** Get Source */
	if (auto src = this->getSource(index)) {
		return src->exportAs(
			juce::File::getCurrentWorkingDirectory().getChildFile(path));
	}

	return false;
}

bool CloneableSourceManager::exportSourceAsync(
	int index, const juce::String& path) const {
	/** Lock */
	juce::ScopedTryReadLock locker(this->getLock());
	if (!locker.isLocked()) { return false; }

	/** Get Source */
	if (auto src = this->getSource(index)) {
		AudioIOList::getInstance()->exportt(src, path);
		return true;
	}

	return false;
}

void CloneableSourceManager::prepareToPlay(double sampleRate, int bufferSize) {
	juce::ScopedWriteLock locker(this->getLock());
	this->sampleRate = sampleRate;
	this->bufferSize = bufferSize;
	for (auto i : this->sourceList) {
		i->prepareToPlay(sampleRate, bufferSize);
	}
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

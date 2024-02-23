#include "CloneableSourceManager.h"
#include "AudioIOList.h"
#include "../AudioCore.h"
#include "../plugin/Plugin.h"
#include "../plugin/PluginLoader.h"
#include "../uiCallback/UICallback.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

bool CloneableSourceManager::addSource(std::unique_ptr<CloneableSource> src) {
	juce::ScopedWriteLock locker(this->getLock());
	src->prepareToPlay(this->sampleRate, this->bufferSize);
	this->sourceList.add(std::move(src));

	juce::MessageManager::callAsync([index = this->sourceList.size() - 1] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
		});

	return true;
}

bool CloneableSourceManager::removeSource(CloneableSource* src) {
	juce::ScopedWriteLock locker(this->getLock());
	this->sourceList.removeObject(src, true);

	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});

	return true;
}

bool CloneableSourceManager::removeSource(int index) {
	juce::ScopedWriteLock locker(this->getLock());
	this->sourceList.remove(index, true);

	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});

	return true;
}

CloneableSource::SafePointer<> CloneableSourceManager::getSource(int index) const {
	juce::ScopedReadLock locker(this->getLock());
	return this->sourceList[index];
}

int CloneableSourceManager::getSourceIndex(const CloneableSource* src) const {
	juce::ScopedReadLock locker(this->getLock());
	return this->sourceList.indexOf(src);
}

int CloneableSourceManager::getSourceNum() const {
	juce::ScopedReadLock locker(this->getLock());
	return this->sourceList.size();
}

juce::ReadWriteLock& CloneableSourceManager::getLock() const {
	return this->lock;
}

bool CloneableSourceManager::setSourceSynthesizer(
	int index, const juce::String& identifier) {
	juce::ScopedReadLock locker(this->getLock());
	if (auto source = this->getSource(index)) {
		if (auto des = Plugin::getInstance()->findPlugin(identifier, true)) {
			PluginLoader::getInstance()->loadPlugin(*(des.get()), source);

			juce::MessageManager::callAsync([index] {
				UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
				});

			return true;
		}
	}
	return false;
}

bool CloneableSourceManager::synthSource(int index) {
	juce::ScopedReadLock locker(this->getLock());
	if (auto source = this->getSource(index)) {
		source->synth();

		juce::MessageManager::callAsync([index] {
			UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
			});

		return true;
	}
	return false;
}

bool CloneableSourceManager::cloneSource(int index) {
	/** Lock */
	juce::ScopedWriteLock locker(this->getLock());
	
	/** Clone */
	if (auto source = this->getSource(index)) {
		if (auto ptr = source->cloneThis()) {
			bool result = this->addSource(std::move(ptr));

			/** Callback */
			juce::MessageManager::callAsync([index = this->sourceList.size() - 1] {
				UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
				});

			return result;
		}
	}
	return false;
}

bool CloneableSourceManager::cloneSourceAsync(int index) {
	/** Lock */
	juce::ScopedReadLock locker(this->getLock());

	/** Get Source */
	if (auto src = this->getSource(index)) {
		/** Create */
		auto dst = src->createThisType();
		auto ptr = dst.get();

		/** Add To List */
		if (!this->addSource(std::move(dst))) { return false; }

		/** Load Async */
		AudioIOList::getInstance()->clone(src, CloneableSource::SafePointer<>{ ptr });

		/** Callback */
		juce::MessageManager::callAsync([index = this->sourceList.size() - 1] {
			UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
			});

		return true;
	}

	return false;
}

bool CloneableSourceManager::saveSource(
	int index, const juce::String& path) const {
	/** Lock */
	juce::ScopedReadLock locker(this->getLock());

	/** Get Source */
	if (auto src = this->getSource(index)) {
		if (src->saveAs(utils::getSourceFile(path))) {
			src->setPath(path);

			/** Callback */
			juce::MessageManager::callAsync([index] {
				UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
				});

			return true;
		}
	}

	return false;
}

bool CloneableSourceManager::saveSourceAsync(
	int index, const juce::String& path) const {
	/** Lock */
	juce::ScopedReadLock locker(this->getLock());

	/** Get Source */
	if (auto src = this->getSource(index)) {
		AudioIOList::getInstance()->save(src, path);

		/** Callback */
		juce::MessageManager::callAsync([index] {
			UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
			});

		return true;
	}

	return false;
}

bool CloneableSourceManager::reloadSourceAsync(
	int index, const juce::String& path, bool copy) {
	/** Lock */
	juce::ScopedWriteLock locker(this->getLock());

	/** Create */
	auto source = this->getSource(index);
	if (!source) { return false; }

	/** Load Async */
	AudioIOList::getInstance()->load(source, path, copy);

	/** Callback */
	juce::MessageManager::callAsync([index] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
		});

	return true;
}

void CloneableSourceManager::prepareToPlay(double sampleRate, int bufferSize) {
	juce::ScopedWriteLock locker(this->getLock());
	this->sampleRate = sampleRate;
	this->bufferSize = bufferSize;
	for (auto i : this->sourceList) {
		i->prepareToPlay(sampleRate, bufferSize);
	}

	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});
}

void CloneableSourceManager::clearGraph() {
	juce::ScopedWriteLock locker(this->getLock());
	this->sourceList.clear(true);
	CloneableSource::resetIdCounter();

	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});
}

bool CloneableSourceManager::parse(const google::protobuf::Message* data) {
	return true;
}

std::unique_ptr<google::protobuf::Message> CloneableSourceManager::serialize() const {
	return nullptr;
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

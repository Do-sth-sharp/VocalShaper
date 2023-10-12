#include "AudioIOList.h"

#include "CloneableSourceManager.h"
#include "CloneableAudioSource.h"
#include "../Utils.h"

AudioIOList::AudioIOList()
	: Thread("Audio Source IO List") {}

AudioIOList::~AudioIOList() {
	if (this->isThreadRunning()) {
		this->stopThread(100000);
	}
}

void AudioIOList::load(CloneableSource::SafePointer<> ptr, juce::String path, bool copy) {
	juce::GenericScopedLock locker(this->lock);

	if (!ptr) { return; }
	this->list.push(std::make_tuple(
		copy ? TaskType::CopyLoad : TaskType::Load, ptr, path));

	this->startThread();
}

void AudioIOList::save(CloneableSource::SafePointer<> ptr, juce::String path) {
	juce::GenericScopedLock locker(this->lock);

	this->list.push(std::make_tuple(TaskType::Save, ptr, path));

	this->startThread();
}

void AudioIOList::exportt(CloneableSource::SafePointer<> ptr, juce::String path) {
	juce::GenericScopedLock locker(this->lock);

	this->list.push(std::make_tuple(TaskType::Export, ptr, path));

	this->startThread();
}

bool AudioIOList::isTask(CloneableSource::SafePointer<> ptr) const {
	juce::GenericScopedLock locker(this->lock);
	if (!ptr) { return false; }
	if (this->currentTask == ptr) { return true; }
	
	int size = this->list.size();
	for (int i = 0; i < size; i++) {
		auto& task = this->list.front();

		if (std::get<1>(task) == ptr) { return true; }

		this->list.push(task);
		this->list.pop();
	}

	return false;
}

void AudioIOList::run() {
	while (!this->threadShouldExit()) {
		/** Lock Source List */
		juce::ScopedReadLock listLocker(
			CloneableSourceManager::getInstance()->getLock());

		/** Get Next Task */
		IOTask task;
		{
			juce::GenericScopedLock locker(this->lock);

			/** Check Empty */
			if (this->list.empty()) { break; }

			/** Dequque Task */
			task = this->list.front();
			this->list.pop();

			/** Get Current Task */
			this->currentTask = std::get<1>(task);
		}

		/** Process Task */
		{
			if (auto src = static_cast<CloneableSource*>(this->currentTask)) {
				juce::File file
					= utils::getSourceFile(std::get<2>(task));
				bool result = false;
				switch (std::get<0>(task)) {
				case TaskType::Load:
					/** Read */
					result = src->loadFrom(file);
					if (result) {
						src->setPath(std::get<2>(task));
					}
					break;
				case TaskType::Save:
					/** Write */
					result = src->saveAs(file);
					if (result) {
						src->setPath(std::get<2>(task));
					}
					break;
				case TaskType::Export:
					/** Export */
					result = src->exportAs(file);
					break;
				case TaskType::CopyLoad:
					/** Copy */
					juce::String dstPath = "./" + file.getFileName();
					juce::File dstFile = utils::getSourceFile(dstPath);
					if (file.getFullPathName() != dstFile.getFullPathName()) {
						file.copyFileTo(dstFile);
					}

					/** Read */
					result = src->loadFrom(dstFile);
					if (result) {
						src->setPath(dstPath);
					}
					break;
				}

				if (result) {
					//src->setPath(std::get<2>(task));
				}
			}
		}

		/** Task End */
		this->currentTask = nullptr;
	}
}

AudioIOList* AudioIOList::getInstance() {
	return AudioIOList::instance ? AudioIOList::instance : (AudioIOList::instance = new AudioIOList());
}

void AudioIOList::releaseInstance() {
	if (AudioIOList::instance) {
		delete AudioIOList::instance;
		AudioIOList::instance = nullptr;
	}
}

AudioIOList* AudioIOList::instance = nullptr;

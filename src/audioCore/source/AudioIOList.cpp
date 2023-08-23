#include "AudioIOList.h"

#include "CloneableSourceManager.h"
#include "CloneableAudioSource.h"

AudioIOList::AudioIOList()
	: Thread("Audio Source IO List") {}

AudioIOList::~AudioIOList() {
	if (this->isThreadRunning()) {
		this->stopThread(100000);
	}
}

void AudioIOList::load(int index, juce::String path) {
	juce::GenericScopedLock locker(this->lock);

	this->list.push(std::make_tuple(TaskType::Load, index, path));

	this->startThread();
}

void AudioIOList::save(int index, juce::String path) {
	juce::GenericScopedLock locker(this->lock);

	this->list.push(std::make_tuple(TaskType::Save, index, path));

	this->startThread();
}

void AudioIOList::exportt(int index, juce::String path) {
	juce::GenericScopedLock locker(this->lock);

	this->list.push(std::make_tuple(TaskType::Export, index, path));

	this->startThread();
}

bool AudioIOList::isTask(int index) const {
	juce::GenericScopedLock locker(this->lock);
	if (this->currentIndex == index) { return true; }
	
	int size = this->list.size();
	for (int i = 0; i < size; i++) {
		auto& task = this->list.front();

		if (std::get<1>(task) == index) { return true; }

		this->list.push(task);
		this->list.pop();
	}

	return false;
}

void AudioIOList::run() {
	while (!this->threadShouldExit()) {
		/** Get Next Task */
		IOTask task;
		{
			juce::GenericScopedLock locker(this->lock);

			/** Check Empty */
			if (this->list.empty()) { break; }

			/** Dequque Task */
			task = this->list.front();
			this->list.pop();

			/** Get Current Index */
			this->currentIndex = std::get<1>(task);
		}

		/** Process Task */
		if (auto src = CloneableSourceManager::getInstance()->getSource(this->currentIndex)) {
			juce::File file 
				= juce::File::getCurrentWorkingDirectory().getChildFile(std::get<2>(task));
			switch (std::get<0>(task)) {
			case TaskType::Load:
				/** Read */
				src->loadFrom(file);
				break;
			case TaskType::Save:
				/** Write */
				src->saveAs(file);
				break;
			case TaskType::Export:
				/** Export */
				src->exportAs(file);
				break;
			}
		}

		/** Task End */
		this->currentIndex = -1;
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

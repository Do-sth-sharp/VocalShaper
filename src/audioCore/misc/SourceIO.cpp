#include "SourceIO.h"
#include "../Utils.h"

SourceIO::SourceIO()
	: Thread("Source IO"),
	audioFormatsIn(SourceIO::trimFormat(utils::getAudioFormatsSupported(false))),
	midiFormatsIn(SourceIO::trimFormat(utils::getMidiFormatsSupported(false))),
	audioFormatsOut(SourceIO::trimFormat(utils::getAudioFormatsSupported(true))),
	midiFormatsOut(SourceIO::trimFormat(utils::getMidiFormatsSupported(true))) {}

SourceIO::~SourceIO() {
	this->stopThread(30000);
}

void SourceIO::addTask(const Task& task) {
	juce::GenericScopedLock locker(this->lock);
	this->list.push(task);
}

void SourceIO::run() {
	while (!this->threadShouldExit()) {
		/** Get Next Task */
		Task task;
		{
			juce::GenericScopedLock locker(this->lock);

			/** Check Empty */
			if (this->list.empty()) { break; }

			/** Dequque Task */
			task = this->list.front();
			this->list.pop();
		}

		/** Process Task */
		{
			/** Check Source Type */
			auto& [type, ptr, path] = task;
			if (!ptr) { continue; }

			juce::File file = utils::getProjectDir().getChildFile(path);
			juce::String extension = file.getFileExtension();
			
			auto& audioTypes = ((type == TaskType::Read) ? this->audioFormatsIn : this->audioFormatsOut);
			auto& midiTypes = ((type == TaskType::Read) ? this->midiFormatsIn : this->midiFormatsOut);

			if (audioTypes.contains(extension)) {
				/** TODO */
			}
			else if (midiTypes.contains(extension)) {
				/** TODO */
			}
		}
	}
}

const juce::StringArray SourceIO::trimFormat(const juce::StringArray& list) {
	juce::StringArray result;
	for (auto& s : list) {
		result.add(s.trimCharactersAtStart("*"));
	}
	return result;
}

SourceIO* SourceIO::getInstance() {
	return SourceIO::instance
		? SourceIO::instance : (SourceIO::instance = new SourceIO);
}

void SourceIO::releaseInstance() {
	if (SourceIO::instance) {
		delete SourceIO::instance;
		SourceIO::instance = nullptr;
	}
}

SourceIO* SourceIO::instance = nullptr;

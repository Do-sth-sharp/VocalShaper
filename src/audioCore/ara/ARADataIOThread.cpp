#include "ARADataIOThread.h"
#include "../Utils.h"

ARADataIOThread::ARADataIOThread()
	: Thread("ARA Data IO") {}

ARADataIOThread::~ARADataIOThread() {
	if (this->isThreadRunning()) {
		this->stopThread(30000);
	}
}

void ARADataIOThread::addTask(
	const juce::String& path, DstPointer document, bool isWrite) {
	JUCE_ASSERT_MESSAGE_THREAD

	/** Prepare Data To Write */
	juce::MemoryBlock data;
	if (isWrite) {
		juce::MemoryOutputStream stream(data, false);
		if (document) {
			document->storeToStream(stream);
		}
	}

	{
		/** Lock */
		juce::GenericScopedLock locker(this->lock);

		/** Push To Task List */
		this->list.push(std::make_tuple(path, document, isWrite, data));
	}

	this->startThread();
}

void ARADataIOThread::run() {
	while (!this->threadShouldExit()) {
		/** Get Next Task */
		ARADataIOTask task;
		{
			juce::GenericScopedLock locker(this->lock);

			/** Check Empty */
			if (this->list.empty()) { break; }

			/** Dequque Task */
			task = this->list.front();
			this->list.pop();
		}

		/** Prepare ARA Internal Data IO */
		auto& [path, document, isWrite, data] = task;

		/** Write Data */
		if (isWrite) {
			ARADataIOThread::writeToFile(data, path);
		}
		/** Read Data */
		else {
			ARADataIOThread::readFromFile(data, path);

			juce::MessageManager::callAsync([document, block = data] {
				if (document) {
					document->restoreFromBlock(block);
				}
				});
		}
	}
}

bool ARADataIOThread::writeToFile(const juce::MemoryBlock& data, const juce::String& path) {
	juce::File file = utils::getProjectDir().getChildFile(path);
	file.getParentDirectory().createDirectory();

	juce::FileOutputStream stream(file);
	if (!stream.openedOk()) { return false; }

	stream.setPosition(0);
	stream.truncate();

	return stream.write(data.getData(), data.getSize());
}

bool ARADataIOThread::readFromFile(juce::MemoryBlock& data, const juce::String& path) {
	juce::File file = utils::getProjectDir().getChildFile(path);;
	juce::FileInputStream stream(file);
	if (!stream.openedOk()) { return false; }

	return (stream.readIntoMemoryBlock(data) == file.getSize());
}

ARADataIOThread* ARADataIOThread::getInstance() {
	return ARADataIOThread::instance
		? ARADataIOThread::instance : (ARADataIOThread::instance = new ARADataIOThread);
}

void ARADataIOThread::releaseInstance() {
	if (ARADataIOThread::instance) {
		delete ARADataIOThread::instance;
		ARADataIOThread::instance = nullptr;
	}
}

ARADataIOThread* ARADataIOThread::instance = nullptr;
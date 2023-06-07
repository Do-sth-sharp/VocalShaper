#include "PlayPosition.h"

#include <chrono>

juce::Optional<juce::AudioPlayHead::PositionInfo> PlayPosition::getPosition() const {
	juce::ScopedReadLock locker(this->lock);

	/** Get System Clock Nanoseconds */
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::nanoseconds ns
		= std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
	this->position.setHostTimeNs((uint64_t)ns.count());

	return juce::makeOptional(this->position);
}

bool PlayPosition::canControlTransport() {
	//return juce::MessageManager::existsAndIsCurrentThread();
	return true;
}

void PlayPosition::transportPlay(bool shouldStartPlaying) {
	juce::ScopedWriteLock locker(this->lock);
	this->position.setIsPlaying(shouldStartPlaying);
}

void PlayPosition::transportRecord(bool shouldStartRecording) {
	juce::ScopedWriteLock locker(this->lock);
	this->position.setIsRecording(shouldStartRecording);
}

void PlayPosition::transportRewind() {
	juce::ScopedWriteLock locker(this->lock);
	this->position.setTimeInSamples(0);
	this->position.setTimeInSeconds(0);
	this->position.setBarCount(0);
	this->position.setPpqPositionOfLastBarStart(0);
	this->position.setPpqPosition(0);
}

PlayPosition* PlayPosition::getInstance() {
	juce::GenericScopedLock locker{ PlayPosition::instanceGuard };
	return PlayPosition::instance ? PlayPosition::instance : (PlayPosition::instance = new PlayPosition());
}

void PlayPosition::releaseInstance() {
	juce::GenericScopedLock locker{ PlayPosition::instanceGuard };
	if (PlayPosition::instance) {
		delete PlayPosition::instance;
		PlayPosition::instance = nullptr;
	}
}

PlayPosition* PlayPosition::instance = nullptr;
juce::CriticalSection PlayPosition::instanceGuard;

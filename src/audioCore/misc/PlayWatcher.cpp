#include "PlayWatcher.h"

#include "PlayPosition.h"
#include "../AudioCore.h"

PlayWatcher::~PlayWatcher() {
	this->stopTimer();
}

void PlayWatcher::timerCallback() {
	/** Play Auto Stop */
	auto playPosition = PlayPosition::getInstance()->getPosition();
	if (playPosition->getIsPlaying() && !playPosition->getIsRecording()) {
		if (auto mainGraph = AudioCore::getInstance()->getGraph()) {
			if ((playPosition->getTimeInSeconds() > mainGraph->getTailLengthSeconds())
				|| PlayPosition::getInstance()->checkOverflow()) {
				AudioCore::getInstance()->pause();
			}
		}
	}
}

PlayWatcher* PlayWatcher::getInstance() {
	return PlayWatcher::instance 
		? PlayWatcher::instance : (PlayWatcher::instance = new PlayWatcher());
}

void PlayWatcher::releaseInstance() {
	if (PlayWatcher::instance) {
		delete PlayWatcher::instance;
		PlayWatcher::instance = nullptr;
	}
}

PlayWatcher* PlayWatcher::instance = nullptr;

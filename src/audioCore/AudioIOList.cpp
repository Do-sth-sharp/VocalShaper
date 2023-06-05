#include "AudioIOList.h"

AudioIOList::AudioIOList()
	: Thread("Audio IO List") {}

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

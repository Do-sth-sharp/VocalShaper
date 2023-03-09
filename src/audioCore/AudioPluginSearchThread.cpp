#include "AudioPluginSearchThread.h"

AudioPluginSearchThread::AudioPluginSearchThread(juce::WeakReference<AudioPluginManagerHelper> manager)
	: Thread("Audio Plugin Search Thread"), manager(manager) {}

std::tuple<bool, juce::KnownPluginList&> AudioPluginSearchThread::getPluginList() {
	if (this->isThreadRunning()) {
		return std::tuple<bool, juce::KnownPluginList&>(false, this->pluginList);
	}

	if (!this->pluginListValidFlag) {
		this->startThread();
		return std::tuple<bool, juce::KnownPluginList&>(false, this->pluginList);
	}
	return std::tuple<bool, juce::KnownPluginList&>(true, this->pluginList);
}

void AudioPluginSearchThread::clearList() {
	if (this->isThreadRunning()) {
		this->stopThread(-1);
	}
	this->pluginListValidFlag = false;
}

void AudioPluginSearchThread::clearTemporary() {
	if (this->isThreadRunning()) {
		this->stopThread(-1);
	}
	this->clearTemporaryInternal();
	this->pluginListValidFlag = false;
}

void AudioPluginSearchThread::run() {
	//TODO Get Temporary File.
	//TODO Get Expect List And Plugin Path List Then Search Path If Temporary File Doesn't Exists.
}

void AudioPluginSearchThread::clearTemporaryInternal() {
	//TODO Remove Temporary File.
}

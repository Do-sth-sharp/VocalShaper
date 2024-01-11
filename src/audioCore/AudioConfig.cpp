#include "AudioConfig.h"

const juce::String AudioConfig::getPluginSearchPathListFilePath() {
	return AudioConfig::getInstance()->pluginSearchPathListFilePath;
}

const juce::String AudioConfig::getPluginListTemporaryFilePath() {
	return AudioConfig::getInstance()->pluginListTemporaryFilePath;
}

const juce::String AudioConfig::getPluginBlackListFilePath() {
	return AudioConfig::getInstance()->pluginBlackListFilePath;
}

const juce::String AudioConfig::getDeadPluginListPath() {
	return AudioConfig::getInstance()->deadPluginListPath;
}

void AudioConfig::setPluginSearchPathListFilePath(const juce::String& path) {
	AudioConfig::getInstance()->pluginSearchPathListFilePath = path;
}

void AudioConfig::setPluginListTemporaryFilePath(const juce::String& path) {
	AudioConfig::getInstance()->pluginListTemporaryFilePath = path;
}

void AudioConfig::setPluginBlackListFilePath(const juce::String& path) {
	AudioConfig::getInstance()->pluginBlackListFilePath = path;
}

void AudioConfig::setDeadPluginListPath(const juce::String& path) {
	AudioConfig::getInstance()->deadPluginListPath = path;
}

void AudioConfig::setAnonymous(bool anonymous) {
	AudioConfig::getInstance()->anonymous = anonymous;
}
bool AudioConfig::getAnonymous() {
	return AudioConfig::getInstance()->anonymous;
}

void AudioConfig::setMidiTail(double time) {
	AudioConfig::getInstance()->midiTailTime = time;
}

double AudioConfig::getMidiTail() {
	return AudioConfig::getInstance()->midiTailTime;
}

AudioConfig* AudioConfig::getInstance() {
	return AudioConfig::instance ? AudioConfig::instance : (AudioConfig::instance = new AudioConfig());
}

AudioConfig* AudioConfig::instance = nullptr;

const int AudioSaveConfig::getBitsPerSample(const juce::String& format) const {
	juce::ScopedReadLock locker(this->lock);

	auto it = this->bitsPerSample.find(format);
	return (it != this->bitsPerSample.end())
		? it->second : 24;
}

const juce::StringPairArray AudioSaveConfig::getMetaData(const juce::String& format) const {
	juce::ScopedReadLock locker(this->lock);

	auto it = this->metaData.find(format);
	return (it != this->metaData.end())
		? it->second : juce::StringPairArray{};
}

const int AudioSaveConfig::getQualityOptionIndex(const juce::String& format) const {
	juce::ScopedReadLock locker(this->lock);

	auto it = this->qualityOptionIndex.find(format);
	return (it != this->qualityOptionIndex.end())
		? it->second : 0;
}

void AudioSaveConfig::setBitsPerSample(const juce::String& format, int value) {
	juce::ScopedWriteLock locker(this->lock);
	this->bitsPerSample[format] = value;
}

void AudioSaveConfig::setMetaData(
	const juce::String& format, const juce::StringPairArray& data) {
	juce::ScopedWriteLock locker(this->lock);
	this->metaData[format] = data;
}

void AudioSaveConfig::setQualityOptionIndex(
	const juce::String& format, int value) {
	juce::ScopedWriteLock locker(this->lock);
	AudioSaveConfig::getInstance()->qualityOptionIndex[format] = value;
}

AudioSaveConfig* AudioSaveConfig::getInstance() {
	return AudioSaveConfig::instance
		? AudioSaveConfig::instance
		: (AudioSaveConfig::instance = new AudioSaveConfig);
}

AudioSaveConfig* AudioSaveConfig::instance = nullptr;


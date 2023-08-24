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

const int AudioConfig::getAudioSaveBitsPerSample() {
	return AudioConfig::getInstance()->audioSaveBitsPerSample;
}

const juce::StringPairArray AudioConfig::getAudioSaveMetaData() {
	return AudioConfig::getInstance()->audioSaveMetaData;
}

const int AudioConfig::getAudioSaveQualityOptionIndex() {
	return AudioConfig::getInstance()->audioSaveQualityOptionIndex;
}

void AudioConfig::setAudioSaveBitsPerSample(int value) {
	AudioConfig::getInstance()->audioSaveBitsPerSample = value;
}

void AudioConfig::setAudioSaveMetaData(const juce::StringPairArray& data) {
	AudioConfig::getInstance()->audioSaveMetaData = data;
}

void AudioConfig::setAudioSaveQualityOptionIndex(int value) {
	AudioConfig::getInstance()->audioSaveQualityOptionIndex = value;
}

AudioConfig* AudioConfig::getInstance() {
	return AudioConfig::instance ? AudioConfig::instance : (AudioConfig::instance = new AudioConfig());
}

AudioConfig* AudioConfig::instance = nullptr;

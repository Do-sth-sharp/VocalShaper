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

AudioConfig* AudioConfig::getInstance() {
	return AudioConfig::instance ? AudioConfig::instance : (AudioConfig::instance = new AudioConfig());
}

AudioConfig* AudioConfig::instance = nullptr;

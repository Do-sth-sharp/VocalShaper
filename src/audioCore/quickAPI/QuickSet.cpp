#include "QuickSet.h"
#include "../AudioConfig.h"
#include "../AudioCore.h"

namespace quickAPI {
	void setPluginSearchPathListFilePath(const juce::String& path) {
		AudioConfig::getInstance()->setPluginSearchPathListFilePath(path);
	}

	void setPluginListTemporaryFilePath(const juce::String& path) {
		AudioConfig::getInstance()->setPluginListTemporaryFilePath(path);
	}

	void setDeadPluginListPath(const juce::String& path) {
		AudioConfig::getInstance()->setDeadPluginListPath(path);
	}

	void setPluginBlackListFilePath(const juce::String& path) {
		AudioConfig::getInstance()->setPluginBlackListFilePath(path);
	}

	void setReturnToStartOnStop(bool value) {
		AudioCore::getInstance()->setReturnToPlayStartPosition(value);
	}

	void setAnonymousMode(bool value) {
		utils::AudioSaveConfig::getInstance()->setAnonymous(value);
	}
}
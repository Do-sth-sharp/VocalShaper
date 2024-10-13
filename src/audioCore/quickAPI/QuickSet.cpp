#include "QuickSet.h"
#include "../AudioConfig.h"
#include "../AudioCore.h"
#include "../plugin/Plugin.h"
#include "../misc/AudioLock.h"
#include "../misc/VMath.h"

namespace quickAPI {
	void setPluginSearchPathListFilePath(const juce::String& path) {
		AudioConfig::setPluginSearchPathListFilePath(path);
	}

	void setPluginListTemporaryFilePath(const juce::String& path) {
		AudioConfig::setPluginListTemporaryFilePath(path);
	}

	void setDeadPluginListPath(const juce::String& path) {
		AudioConfig::setDeadPluginListPath(path);
	}

	void setPluginBlackListFilePath(const juce::String& path) {
		AudioConfig::setPluginBlackListFilePath(path);
	}

	void setReturnToStartOnStop(bool value) {
		AudioCore::getInstance()->setReturnToPlayStartPosition(value);
	}

	void setAnonymousMode(bool value) {
		AudioConfig::setAnonymous(value);
	}

	void setFormatBitsPerSample(const juce::String& extension, int value) {
		AudioSaveConfig::getInstance()->setBitsPerSample(extension, value);
	}

	void setFormatMetaData(const juce::String& extension,
		const juce::StringPairArray& data) {
		AudioSaveConfig::getInstance()->setMetaData(extension, data);
	}

	void setFormatQualityOptionIndex(const juce::String& extension, int value) {
		AudioSaveConfig::getInstance()->setQualityOptionIndex(extension, value);
	}

	bool addToPluginBlackList(const juce::String& plugin) {
		if (Plugin::getInstance()->pluginSearchThreadIsRunning()) { return false; }
		Plugin::getInstance()->addToPluginBlackList(plugin);
		return true;
	}

	bool removeFromPluginBlackList(const juce::String& plugin) {
		if (Plugin::getInstance()->pluginSearchThreadIsRunning()) { return false; }
		Plugin::getInstance()->removeFromPluginBlackList(plugin);
		return true;
	}

	bool addToPluginSearchPath(const juce::String& path) {
		if (Plugin::getInstance()->pluginSearchThreadIsRunning()) { return false; }
		Plugin::getInstance()->addToPluginSearchPath(path);
		return true;
	}

	bool removeFromPluginSearchPath(const juce::String& path) {
		if (Plugin::getInstance()->pluginSearchThreadIsRunning()) { return false; }
		Plugin::getInstance()->removeFromPluginSearchPath(path);
		return true;
	}

	void setSIMDLevel(int level) {
		if (level >= (int)(vMath::InsType::MaxNum)) { level = (int)(vMath::InsType::MaxNum) - 1; }
		if (level < 0) { level = 0; }

		juce::ScopedWriteLock locker(audioLock::getAudioLock());
		vMath::setInsType((vMath::InsType)(level));
	}

	static void setPluginMIDICCListener(PluginHolder pointer, const MIDICCListener& listener) {
		if (pointer) {
			pointer->setMIDICCListener(listener);
		}
	}

	static void clearPluginMIDICCListener(PluginHolder pointer) {
		if (pointer) {
			pointer->clearMIDICCListener();
		}
	}

	void setInstrMIDICCListener(PluginHolder pointer, const MIDICCListener& listener) {
		setPluginMIDICCListener(pointer, listener);
	}

	void setEffectMIDICCListener(PluginHolder pointer, const MIDICCListener& listener) {
		setPluginMIDICCListener(pointer, listener);
	}

	void clearInstrMIDICCListener(PluginHolder pointer) {
		clearPluginMIDICCListener(pointer);
	}

	void clearEffectMIDICCListener(PluginHolder pointer) {
		clearPluginMIDICCListener(pointer);
	}

	void setMainMIDICCListener(const MIDICCListener& listener) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			graph->setMIDICCListener(listener);
		}
	}

	void clearMainMIDICCListener() {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			graph->clearMIDICCListener();
		}
	}

	void setPlayPosition(double timeSec) {
		AudioCore::getInstance()->setPositon(timeSec);
	}

	void setPlayLoop(double startSec, double endSec) {
		AudioCore::getInstance()->setLoop(startSec, endSec);
	}

	void setAudioDeviceCallback(const AudioDeviceCallback& callback) {
		Device::getInstance()->addChangeCallback(callback);
	}

	void setAudioDeviceInitState(std::unique_ptr<juce::XmlElement> state) {
		AudioStartupConfig::getInstance()->setConfig(std::move(state));
	}
}
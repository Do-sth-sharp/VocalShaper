#pragma once

#include <JuceHeader.h>
#include "QuickGet.h"
#include "../misc/Device.h"

namespace quickAPI {
	void setPluginSearchPathListFilePath(const juce::String& path);
	void setPluginListTemporaryFilePath(const juce::String& path);
	void setPluginBlackListFilePath(const juce::String& path);
	void setDeadPluginListPath(const juce::String& path);

	void setReturnToStartOnStop(bool value);
	void setAnonymousMode(bool value);

	void setFormatBitsPerSample(const juce::String& extension, int value);
	void setFormatMetaData(const juce::String& extension,
		const juce::StringPairArray& data);
	void setFormatQualityOptionIndex(const juce::String& extension, int value);

	bool addToPluginBlackList(const juce::String& plugin);
	bool removeFromPluginBlackList(const juce::String& plugin);
	bool addToPluginSearchPath(const juce::String& path);
	bool removeFromPluginSearchPath(const juce::String& path);

	void setSIMDLevel(int level);

	using MIDICCListener = std::function<void(int)>;
	void setInstrMIDICCListener(PluginHolder pointer, const MIDICCListener& listener);
	void setEffectMIDICCListener(PluginHolder pointer, const MIDICCListener& listener);
	void clearInstrMIDICCListener(PluginHolder pointer);
	void clearEffectMIDICCListener(PluginHolder pointer);
	void setMainMIDICCListener(const MIDICCListener& listener);
	void clearMainMIDICCListener();

	void setPlayPosition(double timeSec);
	void setPlayLoop(double startSec, double endSec);

	using AudioDeviceCallback = Device::AudioDeviceCallback;
	void setAudioDeviceCallback(const AudioDeviceCallback& callback);
	void setAudioDeviceInitState(std::unique_ptr<juce::XmlElement> state);
}
#include "QuickGet.h"
#include "../AudioCore.h"
#include "../source/CloneableSourceManager.h"
#include "../plugin/Plugin.h"
#include "../misc/Device.h"

namespace quickAPI {
	juce::Component* getAudioDebugger() {
		return AudioCore::getInstance()->getAudioDebugger();
	}

	juce::Component* getMidiDebugger() {
		return AudioCore::getInstance()->getMIDIDebugger();
	}

	const juce::File getProjectDir() {
		return utils::getProjectDir();
	}

	double getCPUUsage() {
		return Device::getInstance()->getCPUUsage();
	}

	bool getReturnToStartOnStop() {
		return AudioCore::getInstance()->getReturnToPlayStartPosition();
	}

	bool getAnonymousMode() {
		return utils::AudioSaveConfig::getInstance()->getAnonymous();
	}

	std::unique_ptr<juce::Component> createAudioDeviceSelector() {
		return std::unique_ptr<juce::Component>{
			Device::createDeviceSelector().release() };
	}

	const juce::StringArray getSourceNames() {
		juce::StringArray result;

		int size = CloneableSourceManager::getInstance()->getSourceNum();
		for (int i = 0; i < size; i++) {
			auto ptr = CloneableSourceManager::getInstance()->getSource(i);
			if (!ptr) { result.add("-"); continue; }

			juce::String name = ptr->getName();
			if (name.isEmpty()) {
				name = utils::getProjectDir().getChildFile(ptr->getPath()).getFileName();
			}
			result.add(name);
		}

		return result;
	}

	const juce::Array<TrackInfo> getMixerTrackInfos() {
		juce::Array<TrackInfo> result;

		if (auto graph = AudioCore::getInstance()->getGraph()) {
			int size = graph->getTrackNum();
			for (int i = 0; i < size; i++) {
				auto track = graph->getTrackProcessor(i);
				if (!track) {
					result.add({ "", "" });
					continue;
				}
				
				auto& trackType = track->getAudioChannelSet();
				auto trackName = track->getTrackName();
				if (trackName.isEmpty()) { trackName = "untitled"; }
				result.add({ trackName, trackType.getDescription() });
			}
		}

		return result;
	}

	const juce::StringArray getFormatQualityOptionsForExtension(const juce::String& extension) {
		return utils::getQualityOptionsForExtension(extension);
	}

	const juce::Array<int> getFormatPossibleBitDepthsForExtension(const juce::String& extension) {
		return utils::getPossibleBitDepthsForExtension(extension);
	}

	const juce::Array<int> getFormatPossibleSampleRatesForExtension(const juce::String& extension) {
		return utils::getPossibleSampleRatesForExtension(extension);
	}

	const juce::StringArray getFormatPossibleMetaKeyForExtension(const juce::String& extension) {
		return utils::getPossibleMetaKeyForExtension(extension);
	}

	const juce::StringArray getAudioFormatsSupported(bool isWrite) {
		return utils::getAudioFormatsSupported(isWrite);
	}

	const juce::StringArray getMidiFormatsSupported(bool isWrite) {
		return utils::getMidiFormatsSupported(isWrite);
	}

	const juce::StringArray getProjectFormatsSupported(bool isWrite) {
		return utils::getProjectFormatsSupported(isWrite);
	}

	int getFormatBitsPerSample(const juce::String& extension) {
		return utils::AudioSaveConfig::getInstance()->getBitsPerSample(extension);
	}

	const juce::StringPairArray getFormatMetaData(const juce::String& extension) {
		return utils::AudioSaveConfig::getInstance()->getMetaData(extension);
	}

	int getFormatQualityOptionIndex(const juce::String& extension) {
		return utils::AudioSaveConfig::getInstance()->getQualityOptionIndex(extension);
	}

	const juce::StringArray getPluginBlackList() {
		return Plugin::getInstance()->getPluginBlackList();
	}

	const juce::StringArray getPluginSearchPath() {
		return Plugin::getInstance()->getPluginSearchPath();
	}
}
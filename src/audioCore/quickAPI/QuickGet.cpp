﻿#include "QuickGet.h"
#include "../AudioCore.h"
#include "../source/CloneableSourceManager.h"
#include "../plugin/Plugin.h"
#include "../misc/Device.h"
#include "../misc/PlayPosition.h"
#include "../misc/VMath.h"

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
		return AudioConfig::getAnonymous();
	}

	std::unique_ptr<juce::Component> createAudioDeviceSelector() {
		return std::unique_ptr<juce::Component>{
			Device::createDeviceSelector().release() };
	}

	std::tuple<int64_t, double> getTimeInBeat() {
		auto pos = PlayPosition::getInstance()->getPosition();
		return { pos->getBarCount().orFallback(0),
			pos->getPpqPosition().orFallback(0) - pos->getPpqPositionOfLastBarStart().orFallback(0) };
	}

	double getTimeInSecond() {
		auto pos = PlayPosition::getInstance()->getPosition();
		return pos->getTimeInSeconds().orFallback(0);
	}

	const juce::Array<float> getAudioOutputLevel() {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getOutputLevels();
		}
		return {};
	}

	bool isPlaying() {
		auto pos = PlayPosition::getInstance()->getPosition();
		return pos->getIsPlaying();
	}

	bool isRecording() {
		auto pos = PlayPosition::getInstance()->getPosition();
		return pos->getIsRecording();
	}

	int getSourceNum() {
		return CloneableSourceManager::getInstance()->getSourceNum();
	}

	int getSourceId(int index) {
		if (auto ptr = CloneableSourceManager::getInstance()->getSource(index)) {
			return ptr->getId();
		}
		return -1;
	}

	const juce::String getSourceName(int index) {
		if (auto ptr = CloneableSourceManager::getInstance()->getSource(index)) {
			auto name = ptr->getName();
			if (name.isEmpty()) {
				auto path = ptr->getPath();
				if (path.isNotEmpty()) {
					name = utils::getProjectDir().getChildFile(path).getFileName();
				}
			}
			return name;
		}
		return "";
	}

	constexpr std::array<const char*, 4> sourceTypeNameList{
		"Unknown", "Audio", "MIDI" };

	SourceType getSourceType(int index) {
		if (auto ptr = CloneableSourceManager::getInstance()->getSource(index)) {
			if (auto p = dynamic_cast<CloneableAudioSource*>(ptr.getSource())) {
				return SourceType::AudioSource;
			}
			else if (auto p = dynamic_cast<CloneableMIDISource*>(ptr.getSource())) {
				return SourceType::MIDISource;
			}
		}
		return SourceType::UnknownSource;
	}

	const juce::String getSourceTypeName(int index) {
		return sourceTypeNameList[getSourceType(index)];
	}

	const juce::StringArray getAllSourceTypeName() {
		juce::StringArray result;
		for (auto s : sourceTypeNameList) {
			result.add(juce::String{ s });
		}
		return result;
	}

	double getSourceLength(int index) {
		if (auto ptr = CloneableSourceManager::getInstance()->getSource(index)) {
			return ptr->getSourceLength();
		}
		return 0;
	}

	int getSourceChannelNum(int index) {
		if (auto ptr = CloneableSourceManager::getInstance()->getSource(index)) {
			return ptr->getChannelNum();
		}
		return 0;
	}

	int getSourceTrackNum(int index) {
		if (auto ptr = CloneableSourceManager::getInstance()->getSource(index)) {
			return ptr->getTrackNum();
		}
		return 0;
	}
	
	const juce::String getSourceSynthesizerName(int index) {
		if (auto ptr = CloneableSourceManager::getInstance()->getSource(index)) {
			return ptr->getSynthesizerName();
		}
		return "";
	}

	int getSourceSynthDstIndex(int index) {
		if (auto ptr = CloneableSourceManager::getInstance()->getSource(index)) {
			return CloneableSourceManager::getInstance()->getSourceIndex(
				ptr->getDstSource());
		}
		return -1;
	}

	double getSourceSampleRate(int index) {
		if (auto ptr = CloneableSourceManager::getInstance()->getSource(index)) {
			return ptr->getSourceSampleRate();
		}
		return 0;
	}

	int getSourceEventNum(int index) {
		if (auto ptr = CloneableSourceManager::getInstance()->getSource(index)) {
			if (auto p = dynamic_cast<CloneableMIDISource*>(ptr.getSource())) {
				return p->getEventNum();
			}
		}
		return 0;
	}

	const juce::StringArray getSourceNames() {
		juce::StringArray result;

		int size = CloneableSourceManager::getInstance()->getSourceNum();
		for (int i = 0; i < size; i++) {
			auto ptr = CloneableSourceManager::getInstance()->getSource(i);
			if (!ptr) { result.add("-"); continue; }

			juce::String name = ptr->getName();
			if (name.isEmpty()) {
				auto path = ptr->getPath();
				if (path.isNotEmpty()) {
					name = utils::getProjectDir().getChildFile(path).getFileName();
				}
			}
			result.add(name);
		}

		return result;
	}

	const juce::StringArray getSourceNamesWithID() {
		juce::StringArray result;

		int size = CloneableSourceManager::getInstance()->getSourceNum();
		for (int i = 0; i < size; i++) {
			auto ptr = CloneableSourceManager::getInstance()->getSource(i);
			if (!ptr) { result.add("-"); continue; }

			juce::String name = ptr->getName();
			if (name.isEmpty()) {
				auto path = ptr->getPath();
				if (path.isNotEmpty()) {
					name = utils::getProjectDir().getChildFile(path).getFileName();
				}
			}
			result.add("#" + juce::String{ ptr->getId() } + " " + name);
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

	juce::StringArray getPluginFormatsSupported() {
		return utils::getPluginFormatsSupported();
	}

	int getFormatBitsPerSample(const juce::String& extension) {
		return AudioSaveConfig::getInstance()->getBitsPerSample(extension);
	}

	const juce::StringPairArray getFormatMetaData(const juce::String& extension) {
		return AudioSaveConfig::getInstance()->getMetaData(extension);
	}

	int getFormatQualityOptionIndex(const juce::String& extension) {
		return AudioSaveConfig::getInstance()->getQualityOptionIndex(extension);
	}

	const juce::Array<double> getSampleRateSupported() {
		return Device::getInstance()->getCurrentAvailableSampleRates();
	}

	const juce::StringArray getPluginBlackList() {
		return Plugin::getInstance()->getPluginBlackList();
	}

	const juce::StringArray getPluginSearchPath() {
		return Plugin::getInstance()->getPluginSearchPath();
	}

	int getSIMDLevel() {
		return (int)(vMath::getInsType());
	}

	const juce::String getSIMDInsName() {
		return vMath::getInsTypeName();
	}

	const juce::StringArray getAllSIMDInsName() {
		return vMath::getAllInsTypeName();
	}

	const std::tuple<bool, juce::Array<juce::PluginDescription>>
		getPluginList(bool filter, bool instr) {
		auto [result, list] = Plugin::getInstance()->getPluginList();

		if (!filter) { return { result, list.getTypes() }; }

		juce::Array<juce::PluginDescription> listRes;
		auto listAll = list.getTypes();
		for (auto& i : listAll) {
			if (i.isInstrument == instr) {
				listRes.add(i);
			}
		}

		return { result, listRes };
	}
}
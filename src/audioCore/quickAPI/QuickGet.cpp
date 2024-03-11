#include "QuickGet.h"
#include "../AudioCore.h"
#include "../Utils.h"
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

	const juce::Array<double> getSourceSampleRateSupported() {
		return utils::getSourceSampleRateSupported();
	}

	double getCurrentSampleRate() {
		return Device::getInstance()->getAudioSampleRate();
	}

	double getCurrentBufferSize() {
		return Device::getInstance()->getAudioBufferSize();
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

	const juce::Array<TrackType> createAllTrackTypeWithName() {
		auto list = utils::getAllTrackTypes();

		juce::Array<TrackType> result;
		for (auto i : list) {
			auto channelSet = utils::getChannelSet(i);
			result.add({ (int)i, channelSet.getDescription() });
		}
		return result;
	}

	const juce::Array<TrackType> getAllTrackTypeWithName() {
		static juce::Array<TrackType> list = createAllTrackTypeWithName();
		return list;
	}

	static const juce::String getPluginName(PluginHolder pointer) {
		if (pointer) {
			return pointer->getName();
		}
		return "";
	}

	static EditorPointer getPluginEditor(PluginHolder pointer) {
		if (pointer) {
			return pointer->createEditorIfNeeded();
		}
		return nullptr;
	}

	static int getPluginMIDIChannel(PluginHolder pointer) {
		if (pointer) {
			return pointer->getMIDIChannel();
		}
		return 0;
	}

	static bool getPluginMIDICCIntercept(PluginHolder pointer) {
		if (pointer) {
			return pointer->getMIDICCIntercept();
		}
		return false;
	}

	static bool getPluginMIDIOutput(PluginHolder pointer) {
		if (pointer) {
			return pointer->getMIDIOutput();
		}
		return false;
	}

	static const juce::Array<PluginParamLink> getPluginParamCCLink(PluginHolder pointer) {
		if (pointer) {
			juce::Array<PluginParamLink> result;

			for (int i = 0; i < 128; i++) {
				int param = pointer->getCCParamConnection(i);
				if (param > -1) {
					result.add({ param, i });
				}
			}

			return result;
		}
		return {};
	}

	static const juce::String getPluginParamName(PluginHolder pointer, int paramIndex) {
		if (pointer) {
			return pointer->getParamName(paramIndex);
		}
		return "";
	}

	static const juce::StringArray getPluginParamList(PluginHolder pointer) {
		if (pointer) {
			return pointer->getParamNameList();
		}
		return {};
	}

	PluginHolder getInstrPointer(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getSourceProcessor(index)) {
				if (auto instr = track->getInstrProcessor()) {
					return PluginHolder{ instr };
				}
			}
		}
		return PluginHolder{};
	}

	const juce::String getInstrName(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getSourceProcessor(index)) {
				if (auto instr = track->getInstrProcessor()) {
					return instr->getName();
				}
			}
		}
		return "";
	}

	bool getInstrBypass(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getSourceProcessor(index)) {
				return track->getInstrumentBypass();
			}
		}
		return false;
	}

	EditorPointer getInstrEditor(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getSourceProcessor(index)) {
				if (auto instr = track->getInstrProcessor()) {
					return instr->createEditorIfNeeded();
				}
			}
		}
		return nullptr;
	}

	const juce::String getInstrName(PluginHolder pointer) {
		return getPluginName(pointer);
	}

	bool getInstrBypass(PluginHolder pointer) {
		return SeqSourceProcessor::getInstrumentBypass(pointer);
	}

	EditorPointer getInstrEditor(PluginHolder pointer) {
		return getPluginEditor(pointer);
	}

	int getInstrMIDIChannel(PluginHolder pointer) {
		return getPluginMIDIChannel(pointer);
	}

	bool getInstrMIDICCIntercept(PluginHolder pointer) {
		return getPluginMIDICCIntercept(pointer);
	}

	bool getInstrMIDIOutput(PluginHolder pointer) {
		return getPluginMIDIOutput(pointer);
	}

	const juce::Array<PluginParamLink> getInstrParamCCLink(PluginHolder pointer) {
		return getPluginParamCCLink(pointer);
	}

	const juce::String getInstrParamName(PluginHolder pointer, int paramIndex) {
		return getPluginParamName(pointer, paramIndex);
	}

	const juce::StringArray getInstrParamList(PluginHolder pointer) {
		return getPluginParamList(pointer);
	}

	int getEffectNum(int trackIndex) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(trackIndex)) {
				if (auto pluginDock = track->getPluginDock()) {
					return pluginDock->getPluginNum();
				}
			}
		}
		return 0;
	}

	PluginHolder getEffectPointer(int trackIndex, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(trackIndex)) {
				if (auto pluginDock = track->getPluginDock()) {
					if (auto plugin = pluginDock->getPluginProcessor(index)) {
						return PluginHolder{ plugin };
					}
				}
			}
		}
		return PluginHolder{};
	}

	const juce::String getEffectName(int trackIndex, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(trackIndex)) {
				if (auto pluginDock = track->getPluginDock()) {
					if (auto plugin = pluginDock->getPluginProcessor(index)) {
						return plugin->getName();
					}
				}
			}
		}
		return "";
	}

	bool getEffectBypass(int trackIndex, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(trackIndex)) {
				if (auto pluginDock = track->getPluginDock()) {
					return pluginDock->getPluginBypass(index);
				}
			}
		}
		return false;
	}

	const juce::String getEffectName(PluginHolder pointer) {
		return getPluginName(pointer);
	}

	bool getEffectBypass(PluginHolder pointer) {
		return PluginDock::getPluginBypass(pointer);
	}

	EditorPointer getEffectEditor(PluginHolder pointer) {
		return getPluginEditor(pointer);
	}

	int getEffectMIDIChannel(PluginHolder pointer) {
		return getPluginMIDIChannel(pointer);
	}

	bool getEffectMIDICCIntercept(PluginHolder pointer) {
		return getPluginMIDICCIntercept(pointer);
	}

	bool getEffectMIDIOutput(PluginHolder pointer) {
		return getPluginMIDIOutput(pointer);
	}

	const juce::Array<PluginParamLink> getEffectParamCCLink(PluginHolder pointer) {
		return getPluginParamCCLink(pointer);
	}

	const juce::String getEffectParamName(PluginHolder pointer, int paramIndex) {
		return getPluginParamName(pointer, paramIndex);
	}

	const juce::StringArray getEffectParamList(PluginHolder pointer) {
		return getPluginParamList(pointer);
	}

	const juce::AudioChannelSet getEffectChannelSet(int trackIndex, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(trackIndex)) {
				if (auto pluginDock = track->getPluginDock()) {
					if (auto plugin = pluginDock->getPluginProcessor(index)) {
						return plugin->getAudioChannelSet();
					}
				}
			}
		}
		return {};
	}

	int getEffectInputChannelNum(int trackIndex, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(trackIndex)) {
				if (auto pluginDock = track->getPluginDock()) {
					if (auto plugin = pluginDock->getPluginProcessor(index)) {
						return plugin->getTotalNumInputChannels();
					}
				}
			}
		}
		return 0;
	}

	int getSeqTrackNum() {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getSourceNum();
		}
		return 0;
	}

	const juce::String getSeqTrackName(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto seqTrack = graph->getSourceProcessor(index)) {
				return seqTrack->getTrackName();
			}
		}
		return "";
	}

	const juce::Colour getSeqTrackColor(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto seqTrack = graph->getSourceProcessor(index)) {
				return seqTrack->getTrackColor();
			}
		}
		return utils::getDefaultColour();
	}

	const juce::StringArray getSeqTrackNameList() {
		int size = getSeqTrackNum();
		
		juce::StringArray result;
		for (int i = 0; i < size; i++) {
			result.add(getSeqTrackName(i));
		}

		return result;
	}

	const juce::AudioChannelSet getSeqTrackChannelSet(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getSourceProcessor(index)) {
				return track->getAudioChannelSet();
			}
		}
		return juce::AudioChannelSet{};
	}

	int getSeqTrackInputChannelNum(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getSourceProcessor(index)) {
				return track->getTotalNumInputChannels();
			}
		}
		return 0;
	}

	int getSeqTrackOutputChannelNum(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getSourceProcessor(index)) {
				return track->getTotalNumOutputChannels();
			}
		}
		return 0;
	}

	int getMixerTrackNum() {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackNum();
		}
		return 0;
	}

	const juce::String getMixerTrackName(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(index)) {
				return track->getTrackName();
			}
		}
		return "";
	}

	const juce::StringArray getMixerTrackNameList() {
		int size = getMixerTrackNum();

		juce::StringArray result;
		for (int i = 0; i < size; i++) {
			result.add(getMixerTrackName(i));
		}

		return result;
	}

	const juce::Colour getMixerTrackColor(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(index)) {
				return track->getTrackColor();
			}
		}
		return utils::getDefaultColour();
	}

	const juce::AudioChannelSet getMixerTrackChannelSet(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(index)) {
				return track->getAudioChannelSet();
			}
		}
		return juce::AudioChannelSet{};
	}

	int getMixerTrackInputChannelNum(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(index)) {
				return track->getTotalNumInputChannels();
			}
		}
		return 0;
	}

	int getMixerTrackOutputChannelNum(int index) {
		return getMixerTrackChannelSet(index).size();
	}

	int getMixerTrackSideChainBusNum(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(index)) {
				return track->getAdditionalAudioBusNum();
			}
		}
		return 0;
	}

	bool getMixerTrackMIDIInputFromDevice(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackMidiInputFromDeviceConnections(index).size() > 0;
		}
		return false;
	}

	const juce::Array<MIDILink> getMixerTrackMIDIInputFromSource(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackMidiInputFromSrcConnections(index);
		}
		return {};
	}

	const juce::Array<AudioLink> getMixerTrackAudioInputFromDevice(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackInputFromDeviceConnections(index);
		}
		return {};
	}

	const juce::Array<AudioLink> getMixerTrackAudioInputFromSource(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackInputFromSrcConnections(index);
		}
		return {};
	}

	const juce::Array<AudioLink> getMixerTrackAudioInputFromSend(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackInputFromTrackConnections(index);
		}
		return {};
	}

	bool getMixerTrackMIDIOutputToDevice(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackMidiOutputToDeviceConnections(index).size() > 0;
		}
		return false;
	}

	const juce::Array<AudioLink> getMixerTrackAudioOutputToDevice(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackOutputToDeviceConnections(index);
		}
		return {};
	}

	const juce::Array<AudioLink> getMixerTrackAudioOutputToSend(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackOutputToTrackConnections(index);
		}
		return {};
	}

	float getMixerTrackGain(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(index)) {
				return track->getGain();
			}
		}
		return 0;
	}

	float getMixerTrackPan(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(index)) {
				return track->getPan();
			}
		}
		return 0;
	}

	float getMixerTrackFader(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(index)) {
				return track->getSlider();
			}
		}
		return 1;
	}

	bool getMixerTrackMute(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(index)) {
				return track->getMute();
			}
		}
		return false;
	}

	bool isMixerTrackPanValid(int index) {
		return getMixerTrackChannelSet(index).size() == 2;
	}

	const juce::Array<float> getMixerTrackOutputLevel(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(index)) {
				return track->getOutputLevels();
			}
		}
		return {};
	}

	const juce::String getAudioDeviceName(bool isInput) {
		return isInput
			? Device::getInstance()->getAudioInputDeviceName()
			: Device::getInstance()->getAudioOutputDeviceName();
	}

	int getAudioDeviceChannelNum(bool isInput) {
		return isInput
			? Device::getInstance()->getAudioInputDeviceChannelNum()
			: Device::getInstance()->getAudioOutputDeviceChannelNum();
	}

	const juce::String getMIDICCChannelName(int channel) {
		return utils::getMIDICCChannelName(channel);
	}

	const juce::StringArray getMIDICCChannelNameList() {
		juce::StringArray result;
		for (int i = 0; i < 128; i++) {
			result.add(getMIDICCChannelName(i));
		}
		return result;
	}
}
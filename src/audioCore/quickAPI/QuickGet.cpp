#include "QuickGet.h"
#include "../AudioCore.h"
#include "../plugin/Plugin.h"
#include "../misc/Device.h"
#include "../misc/PlayPosition.h"
#include "../misc/VMath.h"
#include "../source/SourceManager.h"

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

	std::tuple<double, double> getLoopTimeSec() {
		if (!PlayPosition::getInstance()->getLooping()) {
			return { 0, 0 };
		}

		return PlayPosition::getInstance()->getLoopingTimeSec();
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

	double getTotalLength() {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTailLengthSeconds();
		}
		return 0;
	}

	int getTempoTempIndexBySec(double timeSec) {
		return PlayPosition::getInstance()->getTempoTempIndexBySec(timeSec);
	}

	const TempoData getTempoData(int tempIndex) {
		return PlayPosition::getInstance()->getTempoTempData(tempIndex);
	}

	double limitTimeSec(double timeSec, double limitLevel) {
		if (limitLevel <= 0) {
			return timeSec;
		}

		double timeQuarter = PlayPosition::getInstance()->toQuarter(timeSec);
		double timeQuarterLimited = std::round(timeQuarter / limitLevel) * limitLevel;
		return PlayPosition::getInstance()->toSecondQ(timeQuarterLimited);
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

	const AudioFormatInfo getAudioFormatData(const juce::String& path) {
		return utils::getAudioFormatData(
			utils::getProjectDir().getChildFile(path));
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

		juce::Array<juce::PluginDescription> listRes;
		auto listAll = list.getTypes();

		if (filter) {
			for (auto& i : listAll) {
				if (i.isInstrument == instr) {
					auto temp = i;
					temp.hasARAExtension = false;
					listRes.add(temp);
				}
				if (instr && i.hasARAExtension) {
					listRes.add(i);
				}
			}
		}
		else {
			for (auto& i : listAll) {
				{
					auto temp = i;
					temp.hasARAExtension = false;
					listRes.add(temp);
				}
				if (i.hasARAExtension) {
					listRes.add(i);
				}
			}
		}
		
		return { result, listRes };
	}

	const juce::Array<BusType> createAllBusTypeWithName() {
		auto list = utils::getAllBusTypes();

		juce::Array<BusType> result;
		for (auto i : list) {
			auto channelSet = utils::getChannelSet(i);
			result.add({ (int)i, channelSet.getDescription() });
		}
		return result;
	}

	const juce::Array<BusType> getAllBusTypeWithName() {
		static juce::Array<BusType> list = createAllBusTypeWithName();
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

	static EditorPointer getPluginEditorExists(PluginHolder pointer) {
		if (pointer) {
			return pointer->getActiveEditor();
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

	const juce::String getPluginIdentifier(PluginHolder pointer) {
		if (pointer) {
			return pointer->getPluginIdentifier();
		}
		return "";
	}

	PluginHolder getInstrPointer(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(TrackType::Track, index)) {
				if (auto seq = track->getSequencer()) {
					if (auto instr = seq->getInstrProcessor()) {
						return PluginHolder{ instr };
					}
				}
			}
		}
		return PluginHolder{};
	}

	bool isInstrValid(int index) {
		return getInstrPointer(index);
	}

	const juce::String getInstrName(int index) {
		auto ptr = getInstrPointer(index);
		return getInstrName(ptr);
	}

	bool getInstrBypass(int index) {
		auto ptr = getInstrPointer(index);
		return getInstrBypass(ptr);
	}

	bool getInstrOffline(int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(TrackType::Track, index)) {
				if (auto seq = track->getSequencer()) {
					return seq->getInstrOffline();
				}
			}
		}
		return false;
	}

	EditorPointer getInstrEditor(int index) {
		auto ptr = getInstrPointer(index);
		return getInstrEditor(ptr);
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

	EditorPointer getInstrEditorExists(PluginHolder pointer) {
		return getPluginEditorExists(pointer);
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

	int getEffectSlotNum() {
		return PluginDock::getSlotNum();
	}

	PluginHolder getEffectPointer(TrackType trackType, int trackIndex, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(trackType, trackIndex)) {
				if (auto mixer = track->getMixer()) {
					if (auto pluginDock = mixer->getPluginDock()) {
						if (auto plugin = pluginDock->getPluginProcessor(index)) {
							return PluginHolder{ plugin };
						}
					}
				}
			}
		}
		return PluginHolder{};
	}

	const juce::String getEffectName(TrackType trackType, int trackIndex, int index) {
		auto ptr = getEffectPointer(trackType, trackIndex, index);
		return getEffectName(ptr);
	}

	bool getEffectBypass(TrackType trackType, int trackIndex, int index) {
		auto ptr = getEffectPointer(trackType, trackIndex, index);
		return getEffectBypass(ptr);
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

	EditorPointer getEffectEditorExists(PluginHolder pointer) {
		return getPluginEditorExists(pointer);
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

	const juce::AudioChannelSet getEffectChannelSet(TrackType trackType, int trackIndex, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(trackType, trackIndex)) {
				if (auto mixer = track->getMixer()) {
					if (auto pluginDock = mixer->getPluginDock()) {
						if (auto plugin = pluginDock->getPluginProcessor(index)) {
							return plugin->getAudioChannelSet();
						}
					}
				}
			}
		}
		return {};
	}

	int getEffectInputChannelNum(TrackType trackType, int trackIndex, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(trackType, trackIndex)) {
				if (auto mixer = track->getMixer()) {
					if (auto pluginDock = mixer->getPluginDock()) {
						if (auto plugin = pluginDock->getPluginProcessor(index)) {
							return plugin->getTotalNumInputChannels();
						}
					}
				}
			}
		}
		return 0;
	}

	int getTrackNum(TrackType type) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackNum(type);
		}
		return 0;
	}

	const juce::String getTrackName(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				return track->getTrackName();
			}
		}
		return "";
	}

	const juce::Colour getTrackColor(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				return track->getTrackColor();
			}
		}
		return {};
	}

	const juce::StringArray getTrackNameList(TrackType type) {
		int size = getTrackNum(type);

		juce::StringArray result;
		for (int i = 0; i < size; i++) {
			result.add(getTrackName(type, i));
		}

		return result;
	}

	const juce::Array<TrackInfo> getTrackInfos(TrackType type) {
		juce::Array<TrackInfo> result;

		if (auto graph = AudioCore::getInstance()->getGraph()) {
			int size = graph->getTrackNum(type);
			for (int i = 0; i < size; i++) {
				auto track = graph->getTrackProcessor(type, i);
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

	const juce::AudioChannelSet getTrackChannelSet(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				return track->getAudioChannelSet();
			}
		}
		return {};
	}

	int getTrackInputChannelNum(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				return track->getTotalNumInputChannels();
			}
		}
		return 0;
	}

	int getTrackOutputChannelNum(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				return track->getTotalNumOutputChannels();
			}
		}
		return 0;
	}

	int getTrackSideChainBusNum(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackAdditionalAudioBusNum(type, index);
		}
		return 0;
	}

	bool isTrackMIDIInputConnected(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->isTrackMIDIInputConnected(type, index);
		}
		return false;
	}

	bool isTrackAudioInputConnected(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->isTrackAudioInputConnected(type, index);
		}
		return false;
	}

	const AudioChannelLinkList getTrackAudioInputChannels(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackAudioInputChannels(type, index);
		}
		return {};
	}

	bool isTrackMIDISendConnected(TrackType type, int index, int slot) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->isTrackMIDISendConnected(type, index, slot);
		}
		return false;
	}

	const SendDst getTrackMIDISendDst(TrackType type, int index, int slot) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackMIDISendDst(type, index, slot);
		}
		return {};
	}

	bool isTrackAudioSendConnected(TrackType type, int index, int slot) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->isTrackAudioSendConnected(type, index, slot);
		}
		return false;
	}

	const SendDst getTrackAudioSendDst(TrackType type, int index, int slot) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackAudioSendDst(type, index, slot);
		}
		return {};
	}

	const AudioChannelLinkList getTrackAudioSendChannels(TrackType type, int index, int slot) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			return graph->getTrackAudioSendChannels(type, index, slot);
		}
		return {};
	}

	float getTrackGain(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				if (auto mixer = track->getMixer()) {
					return mixer->getGain();
				}
			}
		}
		return 0.f;
	}

	float getTrackPan(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				if (auto mixer = track->getMixer()) {
					return mixer->getPan();
				}
			}
		}
		return 0.f;
	}

	float getTrackFader(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				if (auto mixer = track->getMixer()) {
					return mixer->getFader();
				}
			}
		}
		return 0.f;
	}

	bool isTrackPanValid(TrackType type, int index) {
		return getTrackChannelSet(type, index).size() == 2;
	}

	bool getTrackMute(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				return track->getMute();
			}
		}
		return false;
	}

	bool getTrackSolo(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				return track->getSolo();
			}
		}
		return false;
	}

	bool getTrackEquivalentMute(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				return track->getEquivalentMute();
			}
		}
		return false;
	}

	bool getTrackInputMonitoring(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				if (auto seq = track->getSequencer()) {
					return seq->getInputMonitoring();
				}
			}
		}
		return false;
	}

	RecordState getTrackRecording(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				if (auto seq = track->getSequencer()) {
					return seq->getRecording();
				}
			}
		}
		return RecordState::NotRecording;
	}

	const juce::Array<float> getTrackSeqOutputLevel(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				if (auto seq = track->getSequencer()) {
					return seq->getOutputLevels();
				}
			}
		}
		return {};
	}

	const juce::Array<float> getTrackMixerOutputLevel(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				if (auto seq = track->getSequencer()) {
					return seq->getOutputLevels();
				}
			}
		}
		return {};
	}

	const juce::String getTrackBusTypeName(TrackType type, int index) {
		return getTrackChannelSet(type, index).getDescription();
	}

	int getTrackCurrentMIDITrack(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				if (auto seq = track->getSequencer()) {
					return seq->getCurrentMIDITrack();
				}
			}
		}
		return -1;
	}

	bool isTrackAudioRef(TrackType type, int index, uint64_t ref) {
		return getTrackAudioRef(type, index) == ref;
	}

	bool isTrackMIDIRef(TrackType type, int index, uint64_t ref) {
		return getTrackMIDIRef(type, index) == ref;
	}

	uint64_t getTrackAudioRef(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				if (auto seq = track->getSequencer()) {
					return seq->getAudioRef();
				}
			}
		}
		return 0;
	}

	uint64_t getTrackMIDIRef(TrackType type, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(type, index)) {
				if (auto seq = track->getSequencer()) {
					return seq->getMIDIRef();
				}
			}
		}
		return 0;
	}

	int getLabelNum() {
		return PlayPosition::getInstance()->getTempoLabelNum();
	}

	bool isLabelTempo(int index) {
		return PlayPosition::getInstance()->isTempoLabelTempoEvent(index);
	}

	double getLabelTime(int index) {
		return PlayPosition::getInstance()->getTempoLabelTime(index);
	}

	double getLabelTempo(int index) {
		return PlayPosition::getInstance()->getTempoLabelTempo(index);
	}

	std::tuple<int, int> getLabelBeat(int index) {
		return PlayPosition::getInstance()->getTempoLabelBeat(index);
	}

	const juce::Array<TempoLabelData> getLabelDataList() {
		return PlayPosition::getInstance()->getTempoDataList();
	}

	int getBlockNum(TrackType trackType, int trackIndex) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(trackType, trackIndex)) {
				if (auto seq = track->getSequencer()) {
					return seq->getSeqNum();
				}
			}
		}
		return 0;
	}

	const juce::Array<SeqBlock> getBlockList(TrackType trackType, int trackIndex) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(trackType, trackIndex)) {
				if (auto seq = track->getSequencer()) {
					int blockNum = seq->getSeqNum();

					juce::Array<SeqBlock> result;
					for (int i = 0; i < blockNum; i++) {
						result.add(seq->getSeq(i));
					}
					return result;
				}
			}
		}
		return {};
	}

	const SeqBlock getBlock(TrackType trackType, int trackIndex, int index) {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			if (auto track = graph->getTrackProcessor(trackType, trackIndex)) {
				if (auto seq = track->getSequencer()) {
					int blockNum = seq->getSeqNum();
					if (index >= 0 && index < blockNum) {
						return seq->getSeq(index);
					}
				}
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

	double getAudioSourceLength(uint64_t ref) {
		if (ref == 0) { return 0; }
		return SourceManager::getInstance()->getLength(
			ref, SourceManager::SourceType::Audio);
	}

	double getMIDISourceLength(uint64_t ref) {
		if (ref == 0) { return 0; }
		return SourceManager::getInstance()->getLength(
			ref, SourceManager::SourceType::MIDI);
	}

	const juce::String getAudioSourceName(uint64_t ref) {
		if (ref == 0) { return ""; }
		return SourceManager::getInstance()->getFileName(
			ref, SourceManager::SourceType::Audio);
	}

	const juce::String getMIDISourceName(uint64_t ref) {
		if (ref == 0) { return ""; }
		return SourceManager::getInstance()->getFileName(
			ref, SourceManager::SourceType::MIDI);
	}

	bool isAudioSourceValid(uint64_t ref) {
		if (ref == 0) { return false; }
		return SourceManager::getInstance()->isValid(
			ref, SourceManager::SourceType::Audio);
	}

	bool isMIDISourceValid(uint64_t ref) {
		if (ref == 0) { return false; }
		return SourceManager::getInstance()->isValid(
			ref, SourceManager::SourceType::MIDI);
	}

	int getMIDISourceTrackNum(uint64_t ref) {
		if (ref == 0) { return 0; }
		return SourceManager::getInstance()->getMIDITrackNum(ref);
	}

	bool isMIDISourceTrackEmpty(uint64_t ref, int track) {
		if (ref == 0) { return true; }
		return SourceManager::getInstance()->isMIDITrackEmpty(ref, track);
	}

	const NoteList getMIDISourceNotes(uint64_t ref, int track) {
		if (ref == 0) { return {}; }
		return SourceManager::getInstance()->getMIDINoteList(
			ref, track);
	}

	const std::tuple<double, juce::AudioSampleBuffer> getAudioSourceData(uint64_t ref) {
		if (ref == 0) { return {}; }
		return SourceManager::getInstance()->getAudio(ref);
	}
}
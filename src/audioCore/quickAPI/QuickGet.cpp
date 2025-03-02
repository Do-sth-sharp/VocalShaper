#include "QuickGet.h"
#include "../AudioCore.h"
#include "../plugin/Plugin.h"
#include "../misc/Device.h"
#include "../misc/PlayPosition.h"
#include "../misc/VMath.h"
#include "../source/SourceManager.h"

namespace quickAPI {
	static MainGraph* getGraphProcessor() {
		return AudioCore::getInstance()->getGraph();
	}

	static Track* getTrackProcessor(TrackIndex index) {
		if (auto graph = getGraphProcessor()) {
			return graph->getTrackProcessor(index.first, index.second);
		}
		return nullptr;
	}

	static SeqSourceProcessor* getSeqProcessor(int index) {
		if (auto track = getTrackProcessor({ TrackType::Track, index })) {
			return track->getSequencer();
		}
		return nullptr;
	}

	static PluginDecorator* getInstrProcessor(int index) {
		if (auto seq = getSeqProcessor(index)) {
			return seq->getInstrProcessor();
		}
		return nullptr;
	}

	static MixerTrack* getMixerProcessor(TrackIndex index) {
		if (auto track = getTrackProcessor(index)) {
			return track->getMixer();
		}
		return nullptr;
	}

	static PluginDock* getPluginDockProcessor(TrackIndex index) {
		if (auto mixer = getMixerProcessor(index)) {
			return mixer->getPluginDock();
		}
		return nullptr;
	}

	static PluginDecorator* getEffectProcessor(TrackIndex trackIndex, int index) {
		if (auto pluginDock = getPluginDockProcessor(trackIndex)) {
			return pluginDock->getPluginProcessor(index);
		}
		return nullptr;
	}

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
		if (auto graph = getGraphProcessor()) {
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
		if (auto graph = getGraphProcessor()) {
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

	const juce::String getTrackTypeName(TrackType type) {
		return utils::getTrackTypeName(type);
	}

	PluginRef getInstrRef(int index) {
		return reinterpret_cast<PluginRef>(getInstrProcessor(index));
	}

	bool isInstrValid(int index) {
		return getInstrProcessor(index);
	}

	const juce::String getInstrName(int index) {
		if (auto ptr = getInstrProcessor(index)) {
			return ptr->getName();
		}
		return "";
	}

	bool getInstrBypass(int index) {
		if (auto seq = getSeqProcessor(index)) {
			return seq->getInstrumentBypass();
		}
		return false;
	}

	bool getInstrOffline(int index) {
		if (auto seq = getSeqProcessor(index)) {
			return seq->getInstrOffline();
		}
		return false;
	}

	EditorPointer getInstrEditor(int index) {
		if (auto ptr = getInstrProcessor(index)) {
			return ptr->createEditorIfNeeded();
		}
		return nullptr;
	}

	EditorPointer getInstrEditorExists(int index) {
		if (auto ptr = getInstrProcessor(index)) {
			return ptr->getActiveEditor();
		}
		return nullptr;
	}

	int getInstrMIDIChannel(int index) {
		if (auto ptr = getInstrProcessor(index)) {
			return ptr->getMIDIChannel();
		}
		return 0;
	}

	bool getInstrMIDICCIntercept(int index) {
		if (auto ptr = getInstrProcessor(index)) {
			return ptr->getMIDICCIntercept();
		}
		return false;
	}

	bool getInstrMIDIOutput(int index) {
		if (auto ptr = getInstrProcessor(index)) {
			return ptr->getMIDIOutput();
		}
		return false;
	}

	const juce::Array<PluginParamLink> getInstrParamCCLinks(int index) {
		if (auto ptr = getInstrProcessor(index)) {
			return ptr->getParamCCLinks();
		}
		return {};
	}

	const juce::String getInstrParamName(int index, int paramIndex) {
		if (auto ptr = getInstrProcessor(index)) {
			return ptr->getParamName(paramIndex);
		}
		return "";
	}

	const juce::StringArray getInstrParamList(int index) {
		if (auto ptr = getInstrProcessor(index)) {
			return ptr->getParamNameList();
		}
		return {};
	}

	const juce::String getInstrIdentifier(int index) {
		if (auto ptr = getInstrProcessor(index)) {
			return ptr->getPluginIdentifier();
		}
		return "";
	}

	int getEffectSlotNum() {
		return PluginDock::getSlotNum();
	}

	bool isEffectValid(TrackIndex trackIndex, int index) {
		return getEffectProcessor(trackIndex, index);
	}

	PluginRef getEffectRef(TrackIndex trackIndex, int index) {
		return reinterpret_cast<PluginRef>(getEffectProcessor(trackIndex, index));
	}

	const juce::String getEffectName(TrackIndex trackIndex, int index) {
		if (auto ptr = getEffectProcessor(trackIndex, index)) {
			return ptr->getName();
		}
		return "";
	}

	bool getEffectBypass(TrackIndex trackIndex, int index) {
		if (auto pluginDock = getPluginDockProcessor(trackIndex)) {
			return pluginDock->getPluginBypass(index);
		}
		return false;
	}

	EditorPointer getEffectEditor(TrackIndex trackIndex, int index) {
		if (auto ptr = getEffectProcessor(trackIndex, index)) {
			return ptr->createEditorIfNeeded();
		}
		return nullptr;
	}

	EditorPointer getEffectEditorExists(TrackIndex trackIndex, int index) {
		if (auto ptr = getEffectProcessor(trackIndex, index)) {
			return ptr->getActiveEditor();
		}
		return nullptr;
	}

	int getEffectMIDIChannel(TrackIndex trackIndex, int index) {
		if (auto ptr = getEffectProcessor(trackIndex, index)) {
			return ptr->getMIDIChannel();
		}
		return 0;
	}

	bool getEffectMIDICCIntercept(TrackIndex trackIndex, int index) {
		if (auto ptr = getEffectProcessor(trackIndex, index)) {
			return ptr->getMIDICCIntercept();
		}
		return false;
	}

	bool getEffectMIDIOutput(TrackIndex trackIndex, int index) {
		if (auto ptr = getEffectProcessor(trackIndex, index)) {
			return ptr->getMIDIOutput();
		}
		return false;
	}

	const juce::Array<PluginParamLink> getEffectParamCCLinks(TrackIndex trackIndex, int index) {
		if (auto ptr = getEffectProcessor(trackIndex, index)) {
			return ptr->getParamCCLinks();
		}
		return {};
	}

	const juce::String getEffectParamName(TrackIndex trackIndex, int index, int paramIndex) {
		if (auto ptr = getEffectProcessor(trackIndex, index)) {
			return ptr->getParamName(paramIndex);
		}
		return "";
	}

	const juce::StringArray getEffectParamList(TrackIndex trackIndex, int index) {
		if (auto ptr = getEffectProcessor(trackIndex, index)) {
			return ptr->getParamNameList();
		}
		return {};
	}

	const juce::AudioChannelSet getEffectChannelSet(TrackIndex trackIndex, int index) {
		if (auto ptr = getEffectProcessor(trackIndex, index)) {
			return ptr->getAudioChannelSet();
		}
		return {};
	}

	int getEffectInputChannelNum(TrackIndex trackIndex, int index) {
		if (auto ptr = getEffectProcessor(trackIndex, index)) {
			return ptr->getTotalNumInputChannels();
		}
		return 0;
	}

	const juce::String getEffectIdentifier(TrackIndex trackIndex, int index) {
		if (auto ptr = getEffectProcessor(trackIndex, index)) {
			return ptr->getPluginIdentifier();
		}
		return "";
	}

	const juce::String getSendTypeName(SendDstType type) {
		return utils::getSendTypeName(type);
	}

	const juce::String getSendTypeShortName(SendDstType type) {
		return utils::getSendTypeShortName(type);
	}

	const juce::String getSendDstName(SendDst dst, bool isAudio) {
		/** Device */
		if (dst.first == quickAPI::SendDstType::ToDevice) {
			if (isAudio) {
				return quickAPI::getAudioDeviceName(false);
			}
			return quickAPI::getMIDIOutputDeviceName();
		}

		/** Master Track */
		if (dst.first == quickAPI::SendDstType::ToMaster) {
			auto name = quickAPI::getTrackName(
				{ quickAPI::TrackType::MasterTrack, dst.second });
			if (name.isNotEmpty()) {
				return name;
			}
		}

		/** Aux Track */
		if (dst.first == quickAPI::SendDstType::ToAUX) {
			auto name = quickAPI::getTrackName(
				{ quickAPI::TrackType::AuxTrack, dst.second });
			if (name.isNotEmpty()) {
				return name;
			}
		}

		/** Default Name */
		return quickAPI::getSendTypeName(dst.first) + " #" + juce::String{ dst.second };
	}

	const juce::String getSendDstShortName(SendDst dst, bool isAudio) {
		/** Device */
		if (dst.first == quickAPI::SendDstType::ToDevice) {
			/*if (isAudio) {
				return quickAPI::getAudioDeviceName(false);
			}
			return quickAPI::getMIDIOutputDeviceName();*/
			return "Output";
		}

		/** Master Track */
		if (dst.first == quickAPI::SendDstType::ToMaster) {
			auto name = quickAPI::getTrackName(
				{ quickAPI::TrackType::MasterTrack, dst.second });
			if (name.isNotEmpty()) {
				return name;
			}
		}

		/** Aux Track */
		if (dst.first == quickAPI::SendDstType::ToAUX) {
			auto name = quickAPI::getTrackName(
				{ quickAPI::TrackType::AuxTrack, dst.second });
			if (name.isNotEmpty()) {
				return name;
			}
		}

		/** Default Name */
		return quickAPI::getSendTypeShortName(dst.first) + juce::String{ dst.second };
	}

	int getTrackNum(TrackType type) {
		if (auto graph = getGraphProcessor()) {
			return graph->getTrackNum(type);
		}
		return 0;
	}

	const juce::String getTrackName(TrackIndex index) {
		if (auto track = getTrackProcessor(index)) {
			return track->getTrackName();
		}
		return "";
	}

	const juce::Colour getTrackColor(TrackIndex index) {
		if (auto track = getTrackProcessor(index)) {
			return track->getTrackColor();
		}
		return {};
	}

	const juce::StringArray getTrackNameList(TrackType type) {
		int size = getTrackNum(type);

		juce::StringArray result;
		for (int i = 0; i < size; i++) {
			result.add(getTrackName({ type, i }));
		}

		return result;
	}

	const juce::Array<TrackInfo> getTrackInfos(TrackType type) {
		juce::Array<TrackInfo> result;

		if (auto graph = getGraphProcessor()) {
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

	const juce::AudioChannelSet getTrackChannelSet(TrackIndex index) {
		if (auto track = getTrackProcessor(index)) {
			return track->getAudioChannelSet();
		}
		return {};
	}

	int getTrackMIDISendSlotNum() {
		return MainGraph::getMIDISendSlotNum();
	}

	int getTrackAudioSendSlotNum() {
		return MainGraph::getAudioSendSlotNum();
	}

	int getTrackInputChannelNum(TrackIndex index) {
		if (auto track = getTrackProcessor(index)) {
			return track->getTotalNumInputChannels();
		}
		return 0;
	}

	int getTrackOutputChannelNum(TrackIndex index) {
		return getTrackChannelSet(index).size();
	}

	int getTrackSideChainBusNum(TrackIndex index) {
		if (auto graph = getGraphProcessor()) {
			return graph->getTrackAdditionalAudioBusNum(index.first, index.second);
		}
		return 0;
	}

	bool isTrackMIDIInputConnected(TrackIndex index) {
		if (auto graph = getGraphProcessor()) {
			return graph->isTrackMIDIInputConnected(index.first, index.second);
		}
		return false;
	}

	bool isTrackAudioInputConnected(TrackIndex index) {
		if (auto graph = getGraphProcessor()) {
			return graph->isTrackAudioInputConnected(index.first, index.second);
		}
		return false;
	}

	const AudioChannelLinkList getTrackAudioInputChannels(TrackIndex index) {
		if (auto graph = getGraphProcessor()) {
			return graph->getTrackAudioInputChannels(index.first, index.second);
		}
		return {};
	}

	bool isTrackMIDISendConnected(TrackIndex index, int slot) {
		if (auto graph = getGraphProcessor()) {
			return graph->isTrackMIDISendConnected(index.first, index.second, slot);
		}
		return false;
	}

	const SendDst getTrackMIDISendDst(TrackIndex index, int slot) {
		if (auto graph = getGraphProcessor()) {
			return graph->getTrackMIDISendDst(index.first, index.second, slot);
		}
		return { SendDstType::ToDevice, -1 };
	}

	bool isTrackAudioSendConnected(TrackIndex index, int slot) {
		if (auto graph = getGraphProcessor()) {
			return graph->isTrackAudioSendConnected(index.first, index.second, slot);
		}
		return false;
	}

	const SendDst getTrackAudioSendDst(TrackIndex index, int slot) {
		if (auto graph = getGraphProcessor()) {
			return graph->getTrackAudioSendDst(index.first, index.second, slot);
		}
		return { SendDstType::ToDevice, -1 };
	}

	const AudioChannelLinkList getTrackAudioSendChannels(TrackIndex index, int slot) {
		if (auto graph = getGraphProcessor()) {
			return graph->getTrackAudioSendChannels(index.first, index.second, slot);
		}
		return {};
	}

	const juce::String getTrackMIDISendDstName(TrackIndex index, int slot) {
		/** Get Dst */
		auto dst = quickAPI::getTrackMIDISendDst(index, slot);
		if (dst.second < 0) { return ""; }

		/** Get Name */
		return quickAPI::getSendDstName(dst, false);
	}

	const juce::String getTrackAudioSendDstName(TrackIndex index, int slot) {
		/** Get Dst */
		auto dst = quickAPI::getTrackAudioSendDst(index, slot);
		if (dst.second < 0) { return ""; }

		/** Get Name */
		return quickAPI::getSendDstName(dst, true);
	}

	const juce::String getTrackMIDISendDstShortName(TrackIndex index, int slot) {
		/** Get Dst */
		auto dst = quickAPI::getTrackMIDISendDst(index, slot);
		if (dst.second < 0) { return ""; }

		/** Get Name */
		return quickAPI::getSendDstShortName(dst, false);
	}

	const juce::String getTrackAudioSendDstShortName(TrackIndex index, int slot) {
		/** Get Dst */
		auto dst = quickAPI::getTrackAudioSendDst(index, slot);
		if (dst.second < 0) { return ""; }

		/** Get Name */
		return quickAPI::getSendDstShortName(dst, true);
	}

	float getTrackGain(TrackIndex index) {
		if (auto mixer = getMixerProcessor(index)) {
			return mixer->getGain();
		}
		return 0.f;
	}

	float getTrackPan(TrackIndex index) {
		if (auto mixer = getMixerProcessor(index)) {
			return mixer->getPan();
		}
		return 0.f;
	}

	float getTrackFader(TrackIndex index) {
		if (auto mixer = getMixerProcessor(index)) {
			return mixer->getFader();
		}
		return 0.f;
	}

	bool isTrackPanValid(TrackIndex index) {
		return getTrackChannelSet(index).size() == 2;
	}

	bool getTrackMute(TrackIndex index) {
		if (auto track = getTrackProcessor(index)) {
			return track->getMute();
		}
		return false;
	}

	bool getTrackSolo(TrackIndex index) {
		if (auto track = getTrackProcessor(index)) {
			return track->getSolo();
		}
		return false;
	}

	bool getTrackEquivalentMute(TrackIndex index) {
		if (auto track = getTrackProcessor(index)) {
			return track->getEquivalentMute();
		}
		return false;
	}

	bool getTrackInputMonitoring(TrackIndex index) {
		if (auto seq = getSeqProcessor(index.second)) {
			return seq->getInputMonitoring();
		}
		return false;
	}

	RecordState getTrackRecording(TrackIndex index) {
		if (auto seq = getSeqProcessor(index.second)) {
			return seq->getRecording();
		}
		return RecordState::NotRecording;
	}

	const juce::Array<float> getTrackSeqOutputLevel(TrackIndex index) {
		if (auto seq = getSeqProcessor(index.second)) {
			return seq->getOutputLevels();
		}
		return {};
	}

	const juce::Array<float> getTrackMixerOutputLevel(TrackIndex index) {
		if (auto track = getTrackProcessor(index)) {
			return track->getOutputLevels();
		}
		return {};
	}

	const juce::String getTrackBusTypeName(TrackIndex index) {
		return getTrackChannelSet(index).getDescription();
	}

	int getTrackCurrentMIDITrack(TrackIndex index) {
		if (auto seq = getSeqProcessor(index.second)) {
			return seq->getCurrentMIDITrack();
		}
		return -1;
	}

	bool isTrackAudioRef(TrackIndex index, uint64_t ref) {
		return getTrackAudioRef(index) == ref;
	}

	bool isTrackMIDIRef(TrackIndex index, uint64_t ref) {
		return getTrackMIDIRef(index) == ref;
	}

	uint64_t getTrackAudioRef(TrackIndex index) {
		if (auto seq = getSeqProcessor(index.second)) {
			return seq->getAudioRef();
		}
		return 0;
	}

	uint64_t getTrackMIDIRef(TrackIndex index) {
		if (auto seq = getSeqProcessor(index.second)) {
			return seq->getMIDIRef();
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

	int getBlockNum(TrackIndex trackIndex) {
		if (auto seq = getSeqProcessor(trackIndex.second)) {
			return seq->getSeqNum();
		}
		return 0;
	}

	const juce::Array<SeqBlock> getBlockList(TrackIndex trackIndex) {
		if (auto seq = getSeqProcessor(trackIndex.second)) {
			int blockNum = seq->getSeqNum();

			juce::Array<SeqBlock> result;
			for (int i = 0; i < blockNum; i++) {
				result.add(seq->getSeq(i));
			}
			return result;
		}
		return {};
	}

	const SeqBlock getBlock(TrackIndex trackIndex, int index) {
		if (auto seq = getSeqProcessor(trackIndex.second)) {
			int blockNum = seq->getSeqNum();
			if (index >= 0 && index < blockNum) {
				return seq->getSeq(index);
			}
		}
		return {};
	}

	const juce::String getMIDIOutputDeviceName() {
		return Device::getInstance()->getMIDIOutputDevice();
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

	const juce::String getAudioSourcePath(uint64_t ref) {
		if (ref == 0) { return ""; }
		return SourceManager::getInstance()->getFilePath(
			ref, SourceManager::SourceType::Audio);
	}

	const juce::String getMIDISourcePath(uint64_t ref) {
		if (ref == 0) { return ""; }
		return SourceManager::getInstance()->getFilePath(
			ref, SourceManager::SourceType::MIDI);
	}

	const juce::String getAudioSourceName(uint64_t ref) {
		if (ref == 0) { return ""; }
		auto path = getAudioSourcePath(ref);

		return juce::File::createFileWithoutCheckingPath(path).getFileName();
	}

	const juce::String getMIDISourceName(uint64_t ref) {
		if (ref == 0) { return ""; }
		auto path = getMIDISourcePath(ref);

		return juce::File::createFileWithoutCheckingPath(path).getFileName();
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

	int getMIDISourceEventNum(uint64_t ref, int track) {
		if (ref == 0) { return {}; }
		return SourceManager::getInstance()->getMIDIEventNum(
			ref, track);
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
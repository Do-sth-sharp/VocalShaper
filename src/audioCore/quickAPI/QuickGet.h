#pragma once

#include <JuceHeader.h>
#include "../graph/PluginDecorator.h"
#include "../graph/MainGraph.h"
#include "../source/SourceMIDITemp.h"
#include "../Utils.h"

namespace quickAPI {
	juce::Component* getAudioDebugger();
	juce::Component* getMidiDebugger();

	const juce::File getProjectDir();

	double getCPUUsage();
	bool getReturnToStartOnStop();
	bool getAnonymousMode();
	std::unique_ptr<juce::Component> createAudioDeviceSelector();

	std::tuple<int64_t, double> getTimeInBeat();
	double getTimeInSecond();
	std::tuple<double, double> getLoopTimeSec();
	const juce::Array<float> getAudioOutputLevel();
	bool isPlaying();
	bool isRecording();
	double getTotalLength();
	int getTempoTempIndexBySec(double timeSec);
	/** timeInSec, timeInQuarter, timeInBar, secPerQuarter, numerator, denominator */
	using TempoData = std::tuple<double, double, double, double, int, int>;
	const TempoData getTempoData(int tempIndex);
	double limitTimeSec(double timeSec, double limitLevel);

	const juce::StringArray getFormatQualityOptionsForExtension(const juce::String& extension);
	const juce::Array<int> getFormatPossibleBitDepthsForExtension(const juce::String& extension);
	const juce::Array<int> getFormatPossibleSampleRatesForExtension(const juce::String& extension);
	const juce::StringArray getFormatPossibleMetaKeyForExtension(const juce::String& extension);

	const juce::StringArray getAudioFormatsSupported(bool isWrite);
	const juce::StringArray getMidiFormatsSupported(bool isWrite);
	const juce::StringArray getProjectFormatsSupported(bool isWrite);
	const juce::StringArray getPluginFormatsSupported(bool isWrite);

	int getFormatBitsPerSample(const juce::String& extension);
	const juce::StringPairArray getFormatMetaData(const juce::String& extension);
	int getFormatQualityOptionIndex(const juce::String& extension);

	using AudioFormatInfo = utils::AudioFormatInfo;
	const AudioFormatInfo getAudioFormatData(const juce::String& path);

	const juce::Array<double> getSampleRateSupported();
	const juce::Array<double> getSourceSampleRateSupported();
	double getCurrentSampleRate();
	double getCurrentBufferSize();

	const juce::StringArray getPluginBlackList();
	const juce::StringArray getPluginSearchPath();

	int getSIMDLevel();
	const juce::String getSIMDInsName();
	const juce::StringArray getAllSIMDInsName();

	const std::tuple<bool, juce::Array<juce::PluginDescription>>
		getPluginList(bool filter = false, bool instr = true);

	/** ID, Name */
	using BusType = std::tuple<int, juce::String>;
	const juce::Array<BusType> createAllBusTypeWithName();
	const juce::Array<BusType> getAllBusTypeWithName();

	using PluginHolder = PluginDecorator::SafePointer;
	using EditorPointer = juce::Component::SafePointer<juce::AudioProcessorEditor>;

	/** Param Index，CC Channel  */
	using PluginParamLink = std::tuple<int, int>;

	using TrackType = Track::TrackType;

	const juce::String getPluginIdentifier(PluginHolder pointer);

	PluginHolder getInstrPointer(int index);
	bool isInstrValid(int index);
	const juce::String getInstrName(int index);
	bool getInstrBypass(int index);
	bool getInstrOffline(int index);
	EditorPointer getInstrEditor(int index);
	const juce::String getInstrName(PluginHolder pointer);
	bool getInstrBypass(PluginHolder pointer);
	EditorPointer getInstrEditor(PluginHolder pointer);
	EditorPointer getInstrEditorExists(PluginHolder pointer);
	int getInstrMIDIChannel(PluginHolder pointer);
	bool getInstrMIDICCIntercept(PluginHolder pointer);
	bool getInstrMIDIOutput(PluginHolder pointer);
	const juce::Array<PluginParamLink> getInstrParamCCLink(PluginHolder pointer);
	const juce::String getInstrParamName(PluginHolder pointer, int paramIndex);
	const juce::StringArray getInstrParamList(PluginHolder pointer);

	int getEffectNum(TrackType trackType, int trackIndex);
	PluginHolder getEffectPointer(TrackType trackType, int trackIndex, int index);
	const juce::String getEffectName(TrackType trackType, int trackIndex, int index);
	bool getEffectBypass(TrackType trackType, int trackIndex, int index);
	const juce::String getEffectName(PluginHolder pointer);
	bool getEffectBypass(PluginHolder pointer);
	EditorPointer getEffectEditor(PluginHolder pointer);
	EditorPointer getEffectEditorExists(PluginHolder pointer);
	int getEffectMIDIChannel(PluginHolder pointer);
	bool getEffectMIDICCIntercept(PluginHolder pointer);
	bool getEffectMIDIOutput(PluginHolder pointer);
	const juce::Array<PluginParamLink> getEffectParamCCLink(PluginHolder pointer);
	const juce::String getEffectParamName(PluginHolder pointer, int paramIndex);
	const juce::StringArray getEffectParamList(PluginHolder pointer);
	const juce::AudioChannelSet getEffectChannelSet(TrackType trackType, int trackIndex, int index);
	int getEffectInputChannelNum(TrackType trackType, int trackIndex, int index);

	using AudioChannelLinkList = MainGraph::AudioChannelLinkList;
	using SendDst = MainGraph::SendDst;
	using RecordState = SeqSourceProcessor::RecordState;
	int getTrackNum(TrackType type);
	const juce::String getTrackName(TrackType type, int index);
	const juce::Colour getTrackColor(TrackType type, int index);
	const juce::StringArray getTrackNameList(TrackType type);
	using TrackInfo = std::tuple<juce::String, juce::String>;
	const juce::Array<TrackInfo> getTrackInfos(TrackType type);
	const juce::AudioChannelSet getTrackChannelSet(TrackType type, int index);
	int getTrackInputChannelNum(TrackType type, int index);
	int getTrackOutputChannelNum(TrackType type, int index);
	int getTrackSideChainBusNum(TrackType type, int index);
	bool isTrackMIDIInputConnected(TrackType type, int index);
	bool isTrackAudioInputConnected(TrackType type, int index);
	const AudioChannelLinkList getTrackAudioInputChannels(TrackType type, int index);
	bool isTrackMIDISendConnected(TrackType type, int index, int slot);
	const SendDst getTrackMIDISendDst(TrackType type, int index, int slot);
	bool isTrackAudioSendConnected(TrackType type, int index, int slot);
	const SendDst getTrackAudioSendDst(TrackType type, int index, int slot);
	const AudioChannelLinkList getTrackAudioSendChannels(TrackType type, int index, int slot);
	float getTrackGain(TrackType type, int index);
	float getTrackPan(TrackType type, int index);
	float getTrackFader(TrackType type, int index);
	bool isTrackPanValid(TrackType type, int index);
	bool getTrackMute(TrackType type, int index);
	bool getTrackSolo(TrackType type, int index);
	bool getTrackEquivalentMute(TrackType type, int index);
	bool getTrackInputMonitoring(TrackType type, int index);
	RecordState getTrackRecording(TrackType type, int index);
	const juce::Array<float> getTrackSeqOutputLevel(TrackType type, int index);
	const juce::Array<float> getTrackMixerOutputLevel(TrackType type, int index);
	const juce::String getTrackBusTypeName(TrackType type, int index);
	int getTrackCurrentMIDITrack(TrackType type, int index);
	bool isTrackAudioRef(TrackType type, int index, uint64_t ref);
	bool isTrackMIDIRef(TrackType type, int index, uint64_t ref);
	uint64_t getTrackAudioRef(TrackType type, int index);
	uint64_t getTrackMIDIRef(TrackType type, int index);

	int getLabelNum();
	bool isLabelTempo(int index);
	double getLabelTime(int index);
	double getLabelTempo(int index);
	std::tuple<int, int> getLabelBeat(int index);
	/** timeInSec, tempo, numerator, denominator, isTempo */
	using TempoLabelData = std::tuple<double, double, int, int, bool>;
	const juce::Array<TempoLabelData> getLabelDataList();

	/** StartTime, EndTime, Offset */
	using SeqBlock = std::tuple<double, double, double>;
	int getBlockNum(TrackType trackType, int trackIndex);
	const juce::Array<SeqBlock> getBlockList(TrackType trackType, int trackIndex);
	const SeqBlock getBlock(TrackType trackType, int trackIndex, int index);

	const juce::String getAudioDeviceName(bool isInput);
	int getAudioDeviceChannelNum(bool isInput);

	const juce::String getMIDICCChannelName(int channel);
	const juce::StringArray getMIDICCChannelNameList();

	using Note = SourceMIDITemp::Note;
	using NoteList = juce::Array<Note>;

	double getAudioSourceLength(uint64_t ref);
	double getMIDISourceLength(uint64_t ref);
	const juce::String getAudioSourceName(uint64_t ref);
	const juce::String getMIDISourceName(uint64_t ref);
	bool isAudioSourceValid(uint64_t ref);
	bool isMIDISourceValid(uint64_t ref);
	int getMIDISourceTrackNum(uint64_t ref);
	bool isMIDISourceTrackEmpty(uint64_t ref, int track);
	const NoteList getMIDISourceNotes(uint64_t ref, int track);
	const std::tuple<double, juce::AudioSampleBuffer> getAudioSourceData(uint64_t ref);
}
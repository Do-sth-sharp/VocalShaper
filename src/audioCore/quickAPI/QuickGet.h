#pragma once

#include <JuceHeader.h>
#include "../graph/SeqSourceProcessor.h"
#include "../graph/PluginDecorator.h"
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

	using PluginRef = uint64_t;
	using EditorPointer = juce::Component::SafePointer<juce::AudioProcessorEditor>;

	/** Param Index，CC Channel */
	using PluginParamLink = std::tuple<int, int>;

	using TrackType = utils::TrackType;
	using TrackIndex = utils::TrackIndex;
	const juce::String getTrackTypeName(TrackType type);

	/*const juce::String getPluginIdentifier(PluginHolder pointer);*/

	PluginRef getInstrRef(int index);
	bool isInstrValid(int index);
	const juce::String getInstrName(int index);
	bool getInstrBypass(int index);
	bool getInstrOffline(int index);
	EditorPointer getInstrEditor(int index);
	EditorPointer getInstrEditorExists(int index);
	int getInstrMIDIChannel(int index);
	bool getInstrMIDICCIntercept(int index);
	bool getInstrMIDIOutput(int index);
	const juce::Array<PluginParamLink> getInstrParamCCLinks(int index);
	const juce::String getInstrParamName(int index, int paramIndex);
	const juce::StringArray getInstrParamList(int index);
	const juce::String getInstrIdentifier(int index);

	int getEffectSlotNum();
	bool isEffectValid(TrackIndex trackIndex, int index);
	PluginRef getEffectRef(TrackIndex trackIndex, int index);
	const juce::String getEffectName(TrackIndex trackIndex, int index);
	bool getEffectBypass(TrackIndex trackIndex, int index);
	EditorPointer getEffectEditor(TrackIndex trackIndex, int index);
	EditorPointer getEffectEditorExists(TrackIndex trackIndex, int index);
	int getEffectMIDIChannel(TrackIndex trackIndex, int index);
	bool getEffectMIDICCIntercept(TrackIndex trackIndex, int index);
	bool getEffectMIDIOutput(TrackIndex trackIndex, int index);
	const juce::Array<PluginParamLink> getEffectParamCCLinks(TrackIndex trackIndex, int index);
	const juce::String getEffectParamName(TrackIndex trackIndex, int index, int paramIndex);
	const juce::StringArray getEffectParamList(TrackIndex trackIndex, int index);
	const juce::AudioChannelSet getEffectChannelSet(TrackIndex trackIndex, int index);
	int getEffectInputChannelNum(TrackIndex trackIndex, int index);
	const juce::String getEffectIdentifier(TrackIndex trackIndex, int index);

	using AudioChannelLinkList = utils::AudioChannelLinkList;
	using SendDstType = utils::SendDstType;
	using SendDst = utils::SendDst;
	const juce::String getSendTypeName(SendDstType type);
	const juce::String getSendTypeShortName(SendDstType type);
	const juce::String getSendDstName(SendDst dst, bool isAudio);
	const juce::String getSendDstShortName(SendDst dst, bool isAudio);
	using RecordState = SeqSourceProcessor::RecordState;
	int getTrackNum(TrackType type);
	const juce::String getTrackName(TrackIndex index);
	const juce::Colour getTrackColor(TrackIndex index);
	const juce::StringArray getTrackNameList(TrackType type);
	using TrackInfo = std::tuple<juce::String, juce::String>;
	const juce::Array<TrackInfo> getTrackInfos(TrackType type);
	const juce::AudioChannelSet getTrackChannelSet(TrackIndex index);
	int getTrackMIDISendSlotNum();
	int getTrackAudioSendSlotNum();
	int getTrackInputChannelNum(TrackIndex index);
	int getTrackOutputChannelNum(TrackIndex index);
	int getTrackSideChainBusNum(TrackIndex index);
	bool isTrackMIDIInputConnected(TrackIndex index);
	bool isTrackAudioInputConnected(TrackIndex index);
	const AudioChannelLinkList getTrackAudioInputChannels(TrackIndex index);
	bool isTrackMIDISendConnected(TrackIndex index, int slot);
	const SendDst getTrackMIDISendDst(TrackIndex index, int slot);
	bool isTrackAudioSendConnected(TrackIndex index, int slot);
	const SendDst getTrackAudioSendDst(TrackIndex index, int slot);
	const AudioChannelLinkList getTrackAudioSendChannels(TrackIndex index, int slot);
	const juce::String getTrackMIDISendDstName(TrackIndex index, int slot);
	const juce::String getTrackAudioSendDstName(TrackIndex index, int slot);
	const juce::String getTrackMIDISendDstShortName(TrackIndex index, int slot);
	const juce::String getTrackAudioSendDstShortName(TrackIndex index, int slot);
	float getTrackGain(TrackIndex index);
	float getTrackPan(TrackIndex index);
	float getTrackFader(TrackIndex index);
	bool isTrackPanValid(TrackIndex index);
	bool getTrackMute(TrackIndex index);
	bool getTrackSolo(TrackIndex index);
	bool getTrackEquivalentMute(TrackIndex index);
	bool getTrackInputMonitoring(TrackIndex index);
	RecordState getTrackRecording(TrackIndex index);
	const juce::Array<float> getTrackSeqOutputLevel(TrackIndex index);
	const juce::Array<float> getTrackMixerOutputLevel(TrackIndex index);
	const juce::String getTrackBusTypeName(TrackIndex index);
	int getTrackCurrentMIDITrack(TrackIndex index);
	bool isTrackAudioRef(TrackIndex index, uint64_t ref);
	bool isTrackMIDIRef(TrackIndex index, uint64_t ref);
	uint64_t getTrackAudioRef(TrackIndex index);
	uint64_t getTrackMIDIRef(TrackIndex index);

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
	int getBlockNum(TrackIndex trackIndex);
	const juce::Array<SeqBlock> getBlockList(TrackIndex trackIndex);
	const SeqBlock getBlock(TrackIndex trackIndex, int index);

	const juce::String getMIDIOutputDeviceName();
	const juce::String getAudioDeviceName(bool isInput);
	int getAudioDeviceChannelNum(bool isInput);

	const juce::String getMIDICCChannelName(int channel);
	const juce::StringArray getMIDICCChannelNameList();

	using Note = SourceMIDITemp::Note;
	using NoteList = juce::Array<Note>;

	double getAudioSourceLength(uint64_t ref);
	double getMIDISourceLength(uint64_t ref);
	const juce::String getAudioSourcePath(uint64_t ref);
	const juce::String getMIDISourcePath(uint64_t ref);
	const juce::String getAudioSourceName(uint64_t ref);
	const juce::String getMIDISourceName(uint64_t ref);
	bool isAudioSourceValid(uint64_t ref);
	bool isMIDISourceValid(uint64_t ref);
	int getMIDISourceTrackNum(uint64_t ref);
	bool isMIDISourceTrackEmpty(uint64_t ref, int track);
	int getMIDISourceEventNum(uint64_t ref, int track);
	const NoteList getMIDISourceNotes(uint64_t ref, int track);
	const std::tuple<double, juce::AudioSampleBuffer> getAudioSourceData(uint64_t ref);
}
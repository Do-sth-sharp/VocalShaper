#pragma once

#include <JuceHeader.h>
#include "PluginType.h"
#include "../../audioCore/AC_API.h"

class CoreActions final {
	CoreActions() = delete;

public:
	static void loadProject(const juce::String& filePath);
	static void newProject(const juce::String& dirPath);
	static void saveProject(const juce::String& fileName);

	static void rescanPlugins();
	static bool addPluginBlackList(const juce::String& filePath);
	static bool removePluginBlackList(const juce::String& filePath);
	static bool addPluginSearchPath(const juce::String& path);
	static bool removePluginSearchPath(const juce::String& path);

	static void render(const juce::String& dirPath, const juce::String& fileName,
		const juce::String& fileExtension, const juce::Array<int>& tracks,
		const juce::StringPairArray& metaData, int bitDepth, int quality);

	static void undo();
	static void redo();
	static bool canUndo();
	static bool canRedo();
	static const juce::String getUndoName();
	static const juce::String getRedoName();

	static void play();
	static void pause();
	static void stop();
	static void rewind();
	static void record(bool start);

	static void insertInstr(int index, const juce::String& pid, bool addARA);
	static void bypassInstr(int index, bool bypass);
	static void offlineInstr(int index, bool offline);
	static void setInstrMIDIChannel(int index, int channel);
	static void setInstrMIDICCIntercept(int index, bool intercept);
	static void setInstrMIDIOutput(int index, bool output);
	static void setInstrParamCCLink(int index, int paramIndex, int ccChannel);
	static void removeInstrParamCCLink(int index, int ccChannel);
	static void loadInstrPreset(int index, const juce::String& path);
	static void saveInstrPreset(int index, const juce::String& path);
	static void removeInstr(int index);

	static void insertEffect(
		quickAPI::TrackType type, int track, int index, const juce::String& pid);
	static void bypassEffect(
		quickAPI::TrackType type, int track, int index, bool bypass);
	static void setEffectMIDIChannel(
		quickAPI::TrackType type, int track, int index, int channel);
	static void setEffectMIDICCIntercept(
		quickAPI::TrackType type, int track, int index, bool intercept);
	static void setEffectMIDIOutput(
		quickAPI::TrackType type, int track, int index, bool output);
	static void setEffectParamCCLink(
		quickAPI::TrackType type, int track, int index, int paramIndex, int ccChannel);
	static void removeEffectParamCCLink(
		quickAPI::TrackType type, int track, int index, int ccChannel);
	static void loadEffectPreset(
		quickAPI::TrackType type, int track, int index, const juce::String& path);
	static void saveEffectPreset(
		quickAPI::TrackType type, int track, int index, const juce::String& path);
	static void removeEffect(
		quickAPI::TrackType type, int track, int index);
	static void setEffectIndex(
		quickAPI::TrackType type, int track, int oldIndex, int newIndex);
	static void replaceEffect(
		quickAPI::TrackType type, int track, int index, const juce::String& pid);

	static void insertTrack(quickAPI::TrackType type, int index, int bus);
	static void setTrackColor(quickAPI::TrackType type, int index, const juce::Colour& color);
	static void setTrackName(quickAPI::TrackType type, int index, const juce::String& name);
	static void addTrackSideChain(quickAPI::TrackType type, int index);
	static void removeTrackSideChain(quickAPI::TrackType type, int index);
	static void addTrackMIDIInput(quickAPI::TrackType type, int index);
	static void addTrackAudioInput(quickAPI::TrackType type, int index, int srcc, int dstc);
	static void addTrackMIDISend(quickAPI::TrackType type, int index, int slot,
		quickAPI::SendDst dst);
	static void addTrackAudioSend(quickAPI::TrackType type, int index, int slot,
		quickAPI::SendDst dst, int srcc, int dstc);
	static void removeTrackMIDIInput(quickAPI::TrackType type, int index);
	static void removeTrackAudioInput(quickAPI::TrackType type, int index, int srcc, int dstc);
	static void removeTrackMIDISend(quickAPI::TrackType type, int index, int slot,
		quickAPI::SendDst dst);
	static void removeTrackMIDISendOnSlot(quickAPI::TrackType type, int index, int slot);
	static void removeTrackAudioSend(quickAPI::TrackType type, int index, int slot,
		quickAPI::SendDst dst, int srcc, int dstc);
	static void removeTrackAudioSendAllChannel(quickAPI::TrackType type, int index, int slot,
		quickAPI::SendDst dst);
	static void removeTrackAudioSendOnSlot(quickAPI::TrackType type, int index, int slot);
	static void setTrackGain(quickAPI::TrackType type, int index, float value);
	static void setTrackPan(quickAPI::TrackType type, int index, float value);
	static void setTrackFader(quickAPI::TrackType type, int index, float value);
	static void setTrackMute(quickAPI::TrackType type, int index, bool mute);
	static void setTrackSolo(quickAPI::TrackType type, int index, bool solo);
	static void setTrackInputMonitoring(int index, bool inputMonitoring);
	static void setTrackRecording(int index, quickAPI::RecordState rec);
	static void setTrackMIDITrack(int index, int midiTrack);
	static void setTrackAudioRef(int index, const juce::String& path,
		const std::function<void(uint64_t)>& callback = {});
	static void setTrackMIDIRef(int index, const juce::String& path,
		bool getTempo, const std::function<void(uint64_t)>& callback = {});
	static void createTrackAudioSource(int index, const juce::String& name,
		double sampleRate, int channels, double length);
	static void createTrackMIDISource(int index, const juce::String& name);
	static void removeTrack(quickAPI::TrackType type, int index);

	static void addTempoLabel(double time, double tempo);
	static void addBeatLabel(double time, int numerator, int denominator);
	static void removeLabel(int index);
	static void setLabelTime(int index, double time);
	static void setLabelTempo(int index, double tempo);
	static void setLabelBeat(int index, int numerator, int denominator);

	static void insertSeqBlock(int track, double startTime, double endTime, double offset);
	static void splitSeqBlock(int track, int index, double time);
	static void setSeqBlock(int track, int index, double startTime, double endTime, double offset);
	static void removeSeqBlock(int track, int index);

	using CreateAudioSourceCancelCallback = std::function<void(int)>;
	using CreateMIDISourceCancelCallback = CreateAudioSourceCancelCallback;

	static void loadProjectGUI(const juce::String& filePath);
	static void loadProjectGUI();
	static void newProjectGUI();
	static void saveProjectGUI();

	static bool addPluginBlackListGUI(const juce::String& filePath);
	static bool removePluginBlackListGUI(const juce::String& filePath);
	static bool addPluginSearchPathGUI(const juce::String& path);
	static bool removePluginSearchPathGUI(const juce::String& path);

	static void renderGUI(const juce::String& dirPath, const juce::String& fileName,
		const juce::String& fileExtension, const juce::StringPairArray& metaData, int bitDepth, int quality);
	static void renderGUI(const juce::String& dirPath, const juce::String& fileName,
		const juce::String& fileExtension);
	static void renderGUI();

	static void insertInstrGUI(int index);
	static void editInstrParamCCLinkGUI(int index, int paramIndex, int defaultCC = -1);
	static void addInstrParamCCLinkGUI(int index);
	static void loadInstrPresetGUI(int index);
	static void saveInstrPresetGUI(int index);
	static void removeInstrGUI(int index);

	static void insertEffectGUI(quickAPI::TrackType type, int track, int index);
	static void editEffectParamCCLinkGUI(
		quickAPI::TrackType type, int track, int index, int paramIndex, int defaultCC = -1);
	static void addEffectParamCCLinkGUI(quickAPI::TrackType type, int track, int index);
	static void loadPluginPresetGUI(quickAPI::TrackType type, int track, int index);
	static void savePluginPresetGUI(quickAPI::TrackType type, int track, int index);
	static void removeEffectGUI(quickAPI::TrackType type, int track, int index);

	static void insertTrackGUI(quickAPI::TrackType type, int index);
	static void insertTrackGUI(quickAPI::TrackType type);
	static void insertTrackGUI();
	static void setTrackColorGUI(quickAPI::TrackType type, int index);
	static void setTrackNameGUI(quickAPI::TrackType type, int index);
	static void setTrackAudioInputGUI(quickAPI::TrackType type, int index);
	static void setTrackMIDIInputGUI(quickAPI::TrackType type, int index);
	static void setTrackAudioSendGUI(quickAPI::TrackType type, int index, int slot);
	static void setTrackMIDISendGUI(quickAPI::TrackType type, int index, int slot);
	static void removeTrackGUI(int index);

	static void insertSeqGUI(int index);
	static void insertSeqGUI();
	static void setSeqColorGUI(int index);
	static void setSeqNameGUI(int index);
	static void setSeqAudioInputFromDeviceGUI(int index, bool input,
		const juce::Array<std::tuple<int, int>>& links);
	static void setSeqAudioOutputToMixerGUI(int index, int mixerIndex, bool output,
		const juce::Array<std::tuple<int, int>>& links);
	static void setSeqMIDITrackGUI(int index);
	static void setSeqAudioRefGUIThenAddBlock(int index, const juce::String& path);
	static void setSeqAudioRefGUI(int index, const juce::String& path);
	static void setSeqMIDIRefGUIThenAddBlock(int index, const juce::String& path, bool getTempo);
	static void setSeqMIDIRefGUI(int index, const juce::String& path);
	static void createSeqAudioSourceGUI(
		int index, const juce::String& name,
		const CreateAudioSourceCancelCallback& cancelCallback);
	static void createSeqMIDISourceGUI(
		int index, const juce::String& name,
		const CreateMIDISourceCancelCallback& cancelCallback);
	static void createSeqAudioSourceGUI(
		int index, const CreateAudioSourceCancelCallback& cancelCallback);
	static void createSeqMIDISourceGUI(
		int index, const CreateMIDISourceCancelCallback& cancelCallback);
	static void removeSeqGUI(int index);

	static void addLabelGUI(double time);
	static void setLabelTimeGUI(int index, double time);
	static void editLabelGUI(int index);
	static void removeLabelGUI(int index);

	using CancelCallback = std::function<void(void)>;

	static bool askForSaveGUI();
	static void askForAudioPropGUIAsync(
		const std::function<void(double, int, double)>& callback,
		const CancelCallback& cancelCallback = {});
	static void askForMixerTracksListGUIAsync(
		const std::function<void(const juce::Array<int>&)>& callback,
		const CancelCallback& cancelCallback = {});
	static void askForNameGUIAsync(
		const std::function<void(const juce::String&)>& callback,
		const juce::String& defaultName = "",
		const CancelCallback& cancelCallback = {});
	static void askForPluginGUIAsync(
		const std::function<void(const juce::String&, bool)>& callback,
		bool filter = false, bool instr = true,
		const CancelCallback& cancelCallback = {});
	static void askForBusTypeGUIAsync(
		const std::function<void(int)>& callback,
		int defaultType = 20,
		const CancelCallback& cancelCallback = {});
	static void askForTrackTypeGUIAsync(
		const std::function<void(quickAPI::TrackType)>& callback,
		bool allowMasterTrack = true,
		quickAPI::TrackType defaultType = quickAPI::TrackType::Track,
		const CancelCallback& cancelCallback = {});
	static void askForPluginParamGUIAsync(
		const std::function<void(int)>& callback,
		quickAPI::TrackType trackType, int track, int index, PluginType type,
		const CancelCallback& cancelCallback = {});
	static void askForPluginMIDICCGUIAsync(
		const std::function<void(int)>& callback,
		quickAPI::TrackType trackType, int track, int index, PluginType type,
		int defaultCCChannel = -1,
		const CancelCallback& cancelCallback = {});
	static void askForAudioChannelLinkGUIAsync(
		const std::function<void(int, int, bool)>& callback,
		const juce::Array<std::tuple<int, int>>& initList,
		const juce::AudioChannelSet& srcChannels, const juce::AudioChannelSet& dstChannels,
		int srcChannelNum, int dstChannelNum, const juce::String& srcName, const juce::String& dstName,
		bool initIfEmpty, const CancelCallback& cancelCallback = {});
	static void askForColorGUIAsync(
		const std::function<void(const juce::Colour&)>& callback,
		const juce::Colour& defaultColor,
		const CancelCallback& cancelCallback = {});
	static void askForTempoGUIAsync(
		const std::function<void(bool, double, int, int)>& callback,
		bool defaultIsTempo = true,
		double defaultTempo = 120.0, int defaultNumerator = 4, int defaultDenominator = 4,
		bool switchable = true, const CancelCallback& cancelCallback = {});
	static void askForMIDITrackAsync(
		const std::function<void(int)>& callback,
		int totalNum, int defaltTrack = 0,
		const CancelCallback& cancelCallback = {});
	static void askForAudioSaveFormatsAsync(
		const std::function<void(bool, int, int)>& callback,
		const juce::String& format,
		const CancelCallback& cancelCallback = {});
	static void askForPluginPresetAsync(
		const std::function<void(const juce::String&)>& callback,
		const juce::String& identifier, bool saveMode,
		const CancelCallback& cancelCallback = {});
};

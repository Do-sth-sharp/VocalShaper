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
		const juce::String& fileExtension, const juce::Array<int>& tracks);

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

	static void insertInstr(int index, const juce::String& pid);
	static void bypassInstr(int index, bool bypass);
	static void offlineInstr(int index, bool offline);
	static void bypassInstr(quickAPI::PluginHolder instr, bool bypass);
	static void setInstrMIDIChannel(quickAPI::PluginHolder instr, int channel);
	static void setInstrMIDICCIntercept(quickAPI::PluginHolder instr, bool intercept);
	static void setInstrMIDIOutput(quickAPI::PluginHolder instr, bool output);
	static void setInstrParamCCLink(quickAPI::PluginHolder instr, int paramIndex, int ccChannel);
	static void removeInstrParamCCLink(quickAPI::PluginHolder instr, int ccChannel);
	static void removeInstr(int index);

	static void insertEffect(int track, int index, const juce::String& pid);
	static void bypassEffect(int track, int index, bool bypass);
	static void bypassEffect(quickAPI::PluginHolder effect, bool bypass);
	static void setEffectMIDIChannel(quickAPI::PluginHolder effect, int channel);
	static void setEffectMIDICCIntercept(quickAPI::PluginHolder effect, bool intercept);
	static void setEffectMIDIOutput(quickAPI::PluginHolder effect, bool output);
	static void setEffectParamCCLink(quickAPI::PluginHolder effect, int paramIndex, int ccChannel);
	static void removeEffectParamCCLink(quickAPI::PluginHolder effect, int ccChannel);
	static void removeEffect(int track, int index);
	static void setEffectIndex(int track, int oldIndex, int newIndex);
	static void replaceEffect(int track, int index, const juce::String& pid);

	static void insertTrack(int index, int type);
	static void setTrackColor(int index, const juce::Colour& color);
	static void setTrackName(int index, const juce::String& name);
	static void addTrackSideChain(int index);
	static void removeTrackSideChain(int index);
	static void setTrackMIDIInputFromDevice(int index, bool input);
	static void setTrackMIDIInputFromSeqTrack(int index, int seqIndex, bool input);
	static void setTrackAudioInputFromDevice(int index, int channel, int srcChannel, bool input);
	static void setTrackAudioInputFromSource(int index, int channel, int seqIndex, int srcChannel, bool input);
	static void setTrackAudioInputFromSend(int index, int channel, int trackIndex, int srcChannel, bool input);
	static void setTrackMIDIOutputToDevice(int index, bool output);
	static void setTrackAudioOutputToDevice(int index, int channel, int dstChannel, bool output);
	static void setTrackAudioOutputToSend(int index, int channel, int trackIndex, int dstChannel, bool output);
	static void setTrackGain(int index, float value);
	static void setTrackPan(int index, float value);
	static void setTrackFader(int index, float value);
	static void setTrackMute(int index, bool mute);
	static void setTrackSolo(int index);
	static void setTrackMuteAll(bool mute);
	static void removeTrack(int index);

	static void insertSeq(int index, int type);
	static void setSeqColor(int index, const juce::Colour& color);
	static void setSeqName(int index, const juce::String& name);
	static void setSeqMIDIOutputToMixer(int index, int mixerIndex, bool output);
	static void setSeqAudioOutputToMixer(int index, int channel, int mixerIndex, int dstChannel, bool output);
	static void setSeqMute(int index, bool mute);
	static void setSeqSolo(int index);
	static void setSeqMuteAll(bool mute);
	static void setSeqRec(int index, bool rec);
	static void setSeqMIDITrack(int index, int midiTrack);
	static void removeSeq(int index);

	static void addTempoLabel(double time, double tempo);
	static void addBeatLabel(double time, int numerator, int denominator);
	static void removeLabel(int index);
	static void setLabelTime(int index, double time);
	static void setLabelTempo(int index, double tempo);
	static void setLabelBeat(int index, int numerator, int denominator);

	static void loadProjectGUI(const juce::String& filePath);
	static void loadProjectGUI();
	static void newProjectGUI();
	static void saveProjectGUI();

	static bool addPluginBlackListGUI(const juce::String& filePath);
	static bool removePluginBlackListGUI(const juce::String& filePath);
	static bool addPluginSearchPathGUI(const juce::String& path);
	static bool removePluginSearchPathGUI(const juce::String& path);

	static void renderGUI(const juce::Array<int>& tracks);
	static void renderGUI();

	static void insertInstrGUI(int index);
	static void editInstrParamCCLinkGUI(quickAPI::PluginHolder instr, int paramIndex, int defaultCC = -1);
	static void addInstrParamCCLinkGUI(quickAPI::PluginHolder instr);
	static void removeInstrGUI(int index);

	static void insertEffectGUI(int track, int index);
	static void insertEffectGUI(int track);
	static void editEffectParamCCLinkGUI(quickAPI::PluginHolder effect, int paramIndex, int defaultCC = -1);
	static void addEffectParamCCLinkGUI(quickAPI::PluginHolder effect);
	static void removeEffectGUI(int track, int index);

	static void insertTrackGUI(int index);
	static void insertTrackGUI();
	static void setTrackColorGUI(int index);
	static void setTrackNameGUI(int index);
	static void setTrackAudioInputFromDeviceGUI(int index, bool input,
		const juce::Array<std::tuple<int, int>>& links);
	static void setTrackAudioInputFromSourceGUI(int index, int seqIndex, bool input,
		const juce::Array<std::tuple<int, int>>& links);
	static void setTrackAudioInputFromSendGUI(int index, int trackIndex, bool input,
		const juce::Array<std::tuple<int, int>>& links);
	static void setTrackAudioOutputToDeviceGUI(int index, bool output,
		const juce::Array<std::tuple<int, int>>& links);
	static void setTrackAudioOutputToSendGUI(int index, int trackIndex, bool output,
		const juce::Array<std::tuple<int, int>>& links);
	static void removeTrackGUI(int index);

	static void insertSeqGUI(int index);
	static void insertSeqGUI();
	static void setSeqColorGUI(int index);
	static void setSeqNameGUI(int index);
	static void setSeqAudioOutputToMixerGUI(int index, int mixerIndex, bool output,
		const juce::Array<std::tuple<int, int>>& links);
	static void setSeqMIDITrackGUI(int index);
	static void removeSeqGUI(int index);

	static void addLabelGUI(double time);
	static void setLabelTimeGUI(int index, double time);
	static void editLabelGUI(int index);
	static void removeLabelGUI(int index);

	static bool askForSaveGUI();
	static void askForAudioPropGUIAsync(
		const std::function<void(double, int, double)>& callback);
	static void askForMixerTracksListGUIAsync(
		const std::function<void(const juce::Array<int>&)>& callback);
	static void askForNameGUIAsync(
		const std::function<void(const juce::String&)>& callback,
		const juce::String& defaultName = "");
	static void askForPluginGUIAsync(
		const std::function<void(const juce::String&)>& callback,
		bool filter = false, bool instr = true);
	static void askForBusTypeGUIAsync(
		const std::function<void(int)>& callback,
		int defaultType = 20);
	static void askForPluginParamGUIAsync(
		const std::function<void(int)>& callback,
		quickAPI::PluginHolder plugin, PluginType type);
	static void askForPluginMIDICCGUIAsync(
		const std::function<void(int)>& callback,
		quickAPI::PluginHolder plugin, PluginType type,
		int defaultCCChannel = -1);
	static void askForAudioChannelLinkGUIAsync(
		const std::function<void(int, int, bool)>& callback,
		const juce::Array<std::tuple<int, int>>& initList,
		const juce::AudioChannelSet& srcChannels, const juce::AudioChannelSet& dstChannels,
		int srcChannelNum, int dstChannelNum, const juce::String& srcName, const juce::String& dstName,
		bool initIfEmpty);
	static void askForColorGUIAsync(
		const std::function<void(const juce::Colour&)>& callback,
		const juce::Colour& defaultColor);
	static void askForTempoGUIAsync(
		const std::function<void(bool, double, int, int)>& callback,
		bool defaultIsTempo = true,
		double defaultTempo = 120.0, int defaultNumerator = 4, int defaultDenominator = 4,
		bool switchable = true);
	static void askForMIDITrackAsync(
		const std::function<void(int)>& callback,
		int totalNum, int defaltTrack = 0);
};

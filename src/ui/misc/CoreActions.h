#pragma once

#include <JuceHeader.h>

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

	static void loadMIDISource(const juce::String& filePath, bool copy);
	static void loadAudioSource(const juce::String& filePath, bool copy);
	static void newMIDISource();
	static void newAudioSource(double sampleRate, int channels, double length);
	static void saveSource(int index, const juce::String& filePath);
	static void cloneSource(int index);
	static void reloadSource(int index, const juce::String& filePath, bool copy);
	static void synthSource(int index);
	static void removeSource(int index);
	static void setSourceName(int index, const juce::String& name);
	static void setSourceSynthesizer(int index, const juce::String& pid);
	static void setSourceSynthDst(int index, int dstIndex);

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

	static void insertInstr(int index, int type, const juce::String& pid);

	static void loadProjectGUI(const juce::String& filePath);
	static void loadProjectGUI();
	static void newProjectGUI();
	static void saveProjectGUI();

	static bool addPluginBlackListGUI(const juce::String& filePath);
	static bool removePluginBlackListGUI(const juce::String& filePath);
	static bool addPluginSearchPathGUI(const juce::String& path);
	static bool removePluginSearchPathGUI(const juce::String& path);

	static void loadSourceGUI();
	static void newMIDISourceGUI();
	static void newAudioSourceGUI(double sampleRate, int channels, double length);
	static void newAudioSourceGUI();
	static void saveSourceGUI(int index);
	static void saveSourceGUI();
	static void reloadSourceGUI(int index);
	static void reloadSourceGUI();
	static void synthSourceGUI(int index);
	static void synthSourceGUI();
	static void removeSourceGUI(int index);
	static void setSourceNameGUI(int index);
	static void setSourceNameGUI();
	static void setSourceSynthesizerGUI(int index);
	static void setSourceSynthesizerGUI();

	static void renderGUI(const juce::Array<int>& tracks);
	static void renderGUI();

	static void insertInstrGUI(int index, const juce::String& pid);
	static void insertInstrGUI(int index);
	static void insertInstrGUI();

	static bool askForSaveGUI();
	static void askForAudioPropGUIAsync(
		const std::function<void(double, int, double)>& callback);
	static void askForSourceIndexGUIAsync(
		const std::function<void(int)>& callback);
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
};

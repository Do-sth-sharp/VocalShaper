#pragma once

#include <JuceHeader.h>
#include "plugin/AudioPluginSearchThread.h"
#include "AudioConfig.h"
#include "graph/MainGraph.h"
#include "source/CloneableSourceManager.h"
#include "source/CloneableAudioSource.h"
#include "source/CloneableMIDISource.h"
#include "source/CloneableSynthSource.h"
#include "source/AudioIOList.h"
#include "misc/MackieControlHub.h"
#include "project/Serializable.h"

class AudioDeviceChangeListener;
namespace audioCommand {
	class CommandBase;
}

class AudioCore final 
	: private juce::DeletedAtShutdown, public Serializable {
public:
	AudioCore();
	~AudioCore() override;

public:
	enum class AudioDeviceType {
		CoreAudio,
		IOSAudio,
		WASAPIShared,
		WASAPIExclusive,
		WASAPISharedLowLatency,
		DirectSound,
		ASIO,
		ALSA,
		JACK,
		Android,
		OpenSLES,
		Oboe,
		Bela
	};

	static const juce::StringArray getAudioDeviceList(AudioDeviceType type, bool isInput = false);
	static const juce::StringArray getAllAudioDeviceList(bool isInput = false);

	static const juce::StringArray getAllAudioInputDeviceList();
	static const juce::StringArray getAllAudioOutputDeviceList();

	const juce::String setAudioInputDevice(const juce::String& deviceName);
	const juce::String setAudioOutputDevice(const juce::String& deviceName);
	const juce::String getAudioInputDeviceName() const;
	const juce::String getAudioOutputDeviceName() const;

	void setCurrentAudioDeviceType(const juce::String& typeName);
	const juce::String getCurrentAudioDeivceType() const;

	const juce::String setAudioSampleRate(double value);
	const juce::String setAudioBufferSize(int value);
	double getAudioSampleRate() const;
	int getAudioBufferSize() const;

	void playTestSound() const;

	void setMIDIInputDeviceEnabled(const juce::String& deviceID, bool enabled);
	void setMIDIOutputDevice(const juce::String& deviceID);
	bool getMIDIInputDeviceEnabled(const juce::String& deviceID) const;
	const juce::String getMIDIOutputDevice() const;

	static const juce::StringArray getAllMIDIDeviceList(bool isInput = false);

	static const juce::StringArray getAllMIDIInputDeviceList();
	static const juce::StringArray getAllMIDIOutputDeviceList();

	juce::Component* getAudioDebugger() const;
	juce::Component* getMIDIDebugger() const;

	void setMIDIDebuggerMaxNum(int num);
	int getMIDIDebuggerMaxNum() const;

	const juce::StringArray getPluginTypeList() const;
	const std::tuple<bool, juce::KnownPluginList&> getPluginList() const;
	void clearPluginList();
	void clearPluginTemporary();
	bool pluginSearchThreadIsRunning() const;
	const std::unique_ptr<juce::PluginDescription> findPlugin(const juce::String& identifier, bool isInstrument = false) const;

	bool addEffect(const juce::String& identifier, int trackIndex, int effectIndex);
	PluginDecorator* getEffect(int trackIndex, int effectIndex) const;
	bool removeEffect(int trackIndex, int effectIndex);
	void bypassEffect(int trackIndex, int effectIndex, bool bypass);

	bool addInstrument(const juce::String& identifier, int instrIndex,
		const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	PluginDecorator* getInstrument(int instrIndex) const;
	bool removeInstrument(int instrIndex);
	void bypassInstrument(int instrIndex, bool bypass);

	int addSequencerSourceInstance(int trackIndex, int srcIndex,
		double startTime, double endTime, double offset);
	void removeSequencerSourceInstance(int trackIndex, int index);
	int getSequencerSourceInstanceNum(int trackIndex) const;

	bool addRecorderSourceInstance(int srcIndex, double offset);
	void removeRecorderSourceInstance(int index);
	int getRecorderSourceInstanceNum() const;

	void play();
	void pause();
	void stop();
	void rewind();
	void record(bool start);
	void setPositon(double pos);
	void setReturnToPlayStartPosition(bool returnToStart);
	bool getReturnToPlayStartPosition() const;
	juce::Optional<juce::AudioPlayHead::PositionInfo> getPosition() const;

	/**
	 * @attention	Don't change plugin black list while searching plugin.
	 */
	const juce::StringArray getPluginBlackList() const;
	/**
	 * @attention	Don't change plugin black list while searching plugin.
	 */
	void addToPluginBlackList(const juce::String& plugin) const;
	/**
	 * @attention	Don't change plugin black list while searching plugin.
	 */
	void removeFromPluginBlackList(const juce::String& plugin) const;

	/**
	 * @attention	Don't change plugin search path while searching plugin.
	 */
	const juce::StringArray getPluginSearchPath() const;
	/**
	 * @attention	Don't change plugin search path while searching plugin.
	 */
	void addToPluginSearchPath(const juce::String& path) const;
	/**
	 * @attention	Don't change plugin search path while searching plugin.
	 */
	void removeFromPluginSearchPath(const juce::String& path) const;

	/**
	 * @attention	For Renderer Only.
	 */
	void setIsolation(bool isolation);

	bool renderNow(const juce::Array<int>& tracks, const juce::String& path,
		const juce::String& name, const juce::String& extension);
	bool isRendering() const;

	MainGraph* getGraph() const;
	MackieControlHub* getMackie() const;
	juce::AudioDeviceManager* getDevice() const;

	bool save(const juce::String& name);
	bool load(const juce::String& path);
	bool newProj(const juce::String& workingPath);

	void clearGraph();

public:
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<google::protobuf::Message> serialize() const override;

private:
	friend class AudioDebugger;
	friend class audioCommand::CommandBase;
	std::unique_ptr<juce::AudioDeviceManager> audioDeviceManager = nullptr;
	std::unique_ptr<MainGraph> mainAudioGraph = nullptr;
	std::unique_ptr<juce::AudioProcessorPlayer> mainGraphPlayer = nullptr;
	std::unique_ptr<juce::Component> audioDebugger = nullptr;
	std::unique_ptr<juce::Component> midiDebugger = nullptr;
	std::unique_ptr<AudioPluginSearchThread> audioPluginSearchThread = nullptr;
	std::unique_ptr<juce::AudioPlayHead> playHead = nullptr;
	std::unique_ptr<MackieControlHub> mackieHub = nullptr;

	std::unique_ptr<AudioDeviceChangeListener> audioDeviceListener = nullptr;

	bool returnToStart = true;
	double playStartTime = 0;

	friend class AudioDeviceChangeListener;
	void initAudioDevice();
	void updateAudioBuses();

public:
	static AudioCore* getInstance();
	static AudioCore* getInstanceWithoutCreate();
	static void releaseInstance();

private:
	static AudioCore* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCore)
};

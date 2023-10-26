#pragma once

#include <JuceHeader.h>
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
	juce::Component* getAudioDebugger() const;
	juce::Component* getMIDIDebugger() const;

	void setMIDIDebuggerMaxNum(int num);
	int getMIDIDebuggerMaxNum() const;

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
	 * @attention	For Renderer Only.
	 */
	void setIsolation(bool isolation);

	bool renderNow(const juce::Array<int>& tracks, const juce::String& path,
		const juce::String& name, const juce::String& extension);
	bool isRendering() const;

	MainGraph* getGraph() const;
	MackieControlHub* getMackie() const;

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
	std::unique_ptr<MainGraph> mainAudioGraph = nullptr;
	std::unique_ptr<juce::AudioProcessorPlayer> mainGraphPlayer = nullptr;
	std::unique_ptr<juce::Component> audioDebugger = nullptr;
	std::unique_ptr<juce::Component> midiDebugger = nullptr;
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

#pragma once

#include "ActionBase.h"
#include "../quickAPI/QuickGet.h"

class ActionClearPlugin final : public ActionBase {
public:
	ActionClearPlugin();

	bool doAction() override;
	const juce::String getName() const override {
		return "Clear Plugin";
	};
	int getShieldMask() const override {
		return ShieldPluginLoad | ShieldPluginScan;
	};
	const juce::String getStatusStr() const override;

private:
	JUCE_LEAK_DETECTOR(ActionClearPlugin)
};

class ActionSearchPlugin final : public ActionBase {
public:
	ActionSearchPlugin();

	bool doAction() override;
	const juce::String getName() const override {
		return "Search Plugin";
	};
	int getShieldMask() const override {
		return ShieldPluginLoad;
	};
	const juce::String getStatusStr() const override;

private:
	JUCE_LEAK_DETECTOR(ActionSearchPlugin)
};

class ActionRefreshPlugin final : public ActionBase {
public:
	ActionRefreshPlugin();

	bool doAction() override;
	const juce::String getName() const override {
		return "Refresh Plugin";
	};
	int getShieldMask() const override {
		return ShieldPluginLoad | ShieldPluginScan;
	};
	const juce::String getStatusStr() const override;

private:
	JUCE_LEAK_DETECTOR(ActionRefreshPlugin)
};

class ActionPlay final : public ActionBase {
public:
	ActionPlay();

	bool doAction() override;
	const juce::String getName() const override {
		return "Play";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	JUCE_LEAK_DETECTOR(ActionPlay)
};

class ActionPause final : public ActionBase {
public:
	ActionPause();

	bool doAction() override;
	const juce::String getName() const override {
		return "Pause";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	JUCE_LEAK_DETECTOR(ActionPause)
};

class ActionStop final : public ActionBase {
public:
	ActionStop();

	bool doAction() override;
	const juce::String getName() const override {
		return "Stop";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	JUCE_LEAK_DETECTOR(ActionStop)
};

class ActionRewind final : public ActionBase {
public:
	ActionRewind();

	bool doAction() override;
	const juce::String getName() const override {
		return "Rewind";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	JUCE_LEAK_DETECTOR(ActionRewind)
};

class ActionStartRecord final : public ActionBase {
public:
	ActionStartRecord();

	bool doAction() override;
	const juce::String getName() const override {
		return "Start Record";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	JUCE_LEAK_DETECTOR(ActionStartRecord)
};

class ActionStopRecord final : public ActionBase {
public:
	ActionStopRecord();

	bool doAction() override;
	const juce::String getName() const override {
		return "Stop Record";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	JUCE_LEAK_DETECTOR(ActionStopRecord)
};

class ActionRenderNow final : public ActionBase {
public:
	ActionRenderNow() = delete;
	ActionRenderNow(
		const juce::String& path, const juce::String& name,
		const juce::String& extension, const juce::Array<quickAPI::TrackIndex>& tracks,
		const juce::StringPairArray& metaData, int bitDepth, int quality);

	bool doAction() override;
	const juce::String getName() const override {
		return "Render Now";
	};
	int getShieldMask() const override {
		return ShieldRendering | ShieldSourceIO | ShieldPluginLoad | ShieldARAAnalysis;
	};
	const juce::String getStatusStr() const override;

private:
	const juce::String path, name, extension;
	const juce::Array<quickAPI::TrackIndex> tracks;
	const juce::StringPairArray metaData;
	const int bitDepth;
	const int quality;

	JUCE_LEAK_DETECTOR(ActionRenderNow)
};

class ActionNewProject final : public ActionBase {
public:
	ActionNewProject() = delete;
	ActionNewProject(const juce::String& path);

	bool doAction() override;
	const juce::String getName() const override {
		return "New Project";
	};
	int getShieldMask() const override {
		return ShieldRendering | ShieldSourceIO | ShieldPluginLoad;
	};
	const juce::String getStatusStr() const override;

private:
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionNewProject)
};

class ActionSave final : public ActionBase {
public:
	ActionSave() = delete;
	ActionSave(const juce::String& name);

	bool doAction() override;
	const juce::String getName() const override {
		return "Save";
	};
	int getShieldMask() const override {
		return ShieldRendering | ShieldSourceIO | ShieldPluginLoad;
	};
	ActionType getActionType() const override { return ActionType::ActionSave; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const juce::String name;

	JUCE_LEAK_DETECTOR(ActionSave)
};

class ActionLoad final : public ActionBase {
public:
	ActionLoad() = delete;
	ActionLoad(const juce::String& path);

	bool doAction() override;
	const juce::String getName() const override {
		return "Load";
	};
	int getShieldMask() const override {
		return ShieldRendering | ShieldSourceIO | ShieldPluginLoad | ShieldPluginScan;
	};
	const juce::String getStatusStr() const override;

private:
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionLoad)
};

class ActionInitAudioSource final : public ActionBase {
public:
	ActionInitAudioSource() = delete;
	ActionInitAudioSource(int index,
		const juce::String& name,
		double sampleRate, int channels, double length);

	bool doAction() override;
	const juce::String getName() const override {
		return "Init Audio Source";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	bool getProjectChange() const override { return true; };
	const juce::String getStatusStr() const override;

private:
	const int index;
	const juce::String name;
	const double sampleRate;
	const int channels;
	const double length;

	JUCE_LEAK_DETECTOR(ActionInitAudioSource)
};

class ActionInitMidiSource final : public ActionBase {
public:
	ActionInitMidiSource() = delete;
	ActionInitMidiSource(int index,
		const juce::String& name);

	bool doAction() override;
	const juce::String getName() const override {
		return "Init MIDI Source";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	bool getProjectChange() const override { return true; };
	const juce::String getStatusStr() const override;

private:
	const int index;
	const juce::String name;

	JUCE_LEAK_DETECTOR(ActionInitMidiSource)
};

class ActionLoadAudioSource final : public ActionBase {
public:
	ActionLoadAudioSource() = delete;
	using SourceCallback = std::function<void(uint64_t)>;
	ActionLoadAudioSource(int index,
		const juce::String& path, const SourceCallback& callback = {});

	bool doAction() override;
	const juce::String getName() const override {
		return "Load Audio Source";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	bool getProjectChange() const override { return true; };
	const juce::String getStatusStr() const override;

private:
	const int index;
	const juce::String path;
	const SourceCallback callback;

	JUCE_LEAK_DETECTOR(ActionLoadAudioSource)
};

class ActionLoadMidiSource final : public ActionBase {
public:
	ActionLoadMidiSource() = delete;
	using SourceCallback = std::function<void(uint64_t)>;
	ActionLoadMidiSource(int index,
		const juce::String& path, bool getTempo = false,
		const SourceCallback& callback = {});

	bool doAction() override;
	const juce::String getName() const override {
		return "Load MIDI Source";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	bool getProjectChange() const override { return true; };
	const juce::String getStatusStr() const override;

private:
	const int index;
	const juce::String path;
	const bool getTempo;
	const SourceCallback callback;

	JUCE_LEAK_DETECTOR(ActionLoadMidiSource)
};

class ActionSaveAudioSource final : public ActionBase {
public:
	ActionSaveAudioSource() = delete;
	ActionSaveAudioSource(int index,
		const juce::String& path);

	bool doAction() override;
	const juce::String getName() const override {
		return "Save Audio Source";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	const int index;
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionSaveAudioSource)
};

class ActionSaveMidiSource final : public ActionBase {
public:
	ActionSaveMidiSource() = delete;
	ActionSaveMidiSource(int index,
		const juce::String& path);

	bool doAction() override;
	const juce::String getName() const override {
		return "Save MIDI Source";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	const int index;
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionSaveMidiSource)
};

class ActionSplitSequencerBlock final : public ActionUndoableBase {
public:
	ActionSplitSequencerBlock() = delete;
	ActionSplitSequencerBlock(
		int track, int block, double time);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Split Sequencer Track Block";
	};
	ActionType getActionType() const override { return ActionType::ActionSplitSequencerBlock; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int track, block;
	const double time;

	JUCE_LEAK_DETECTOR(ActionSplitSequencerBlock)
};

class ActionLoadInstrState final : public ActionUndoableBase {
public:
	ActionLoadInstrState() = delete;
	ActionLoadInstrState(
		int index, const juce::String& path);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Load Instr State";
	};
	ActionType getActionType() const override { return ActionType::ActionLoadInstrState; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int index;
	const juce::String path;

	juce::MemoryBlock oldState;

	JUCE_LEAK_DETECTOR(ActionLoadInstrState)
};

class ActionSaveInstrState final : public ActionBase {
public:
	ActionSaveInstrState() = delete;
	ActionSaveInstrState(
		int index, const juce::String& path);

	bool doAction() override;
	const juce::String getName() const override {
		return "Save Instr State";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	const int index;
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionSaveInstrState)
};

class ActionLoadEffectState final : public ActionUndoableBase {
public:
	ActionLoadEffectState() = delete;
	ActionLoadEffectState(
		quickAPI::TrackIndex track, int index, const juce::String& path);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Load Effect State";
	};
	ActionType getActionType() const override { return ActionType::ActionLoadEffectState; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex track;
	const int index;
	const juce::String path;

	juce::MemoryBlock oldState;

	JUCE_LEAK_DETECTOR(ActionLoadEffectState)
};

class ActionSaveEffectState final : public ActionBase {
public:
	ActionSaveEffectState() = delete;
	ActionSaveEffectState(
		quickAPI::TrackIndex track, int index, const juce::String& path);

	bool doAction() override;
	const juce::String getName() const override {
		return "Save Effect State";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	const quickAPI::TrackIndex track;
	const int index;
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionSaveEffectState)
};

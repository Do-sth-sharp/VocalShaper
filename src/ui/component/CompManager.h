#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>

class CompManager final : private juce::DeletedAtShutdown {
public:
	CompManager();

	enum class CompType : int {
		StartMenu,
		ToolBar,
		PluginView,
		SourceView,
		TrackView,
		InstrView,
		MixerView,
		SourceEditView,
		SourceRecordView,
		AudioDebugger,
		MidiDebugger,

		CompMaxSize
	};

	void set(CompType type, std::unique_ptr<flowUI::FlowComponent> comp);
	flowUI::FlowComponent* get(CompType type) const;

private:
	juce::OwnedArray<flowUI::FlowComponent> compList;

public:
	static CompManager* getInstance();
	static void releaseInstance();

private:
	static CompManager* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompManager)
};

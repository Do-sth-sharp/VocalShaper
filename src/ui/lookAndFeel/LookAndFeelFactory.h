#pragma once

#include <JuceHeader.h>

class LookAndFeelFactory final : private juce::DeletedAtShutdown {
public:
	LookAndFeelFactory() = default;

	void initialise();
	void setDefaultSansSerifTypeface(juce::Typeface::Ptr typeface);

	enum LAFType {
		ToolBar, MainMenu, SysStatus, Time, Controller, Tools,
		Message, MessageView, PluginView, PluginEditor, ChannelLink,
		Mixer, Scroller, ColorEditor, SideChain, LevelMeter,
		MuteButton, SoloButton, RecButton, Effect, Send, Seq, TimeRuler, SeqTrack,
		SeqTrackName, InstrName, SeqBlock, Editor, EditorSwitchBar,
		Piano, MidiContent, InputMonitoringButton,

		TotalTypeNum
	};

	juce::LookAndFeel* getLAFFor(LAFType type) const;

private:
	juce::OwnedArray<juce::LookAndFeel> lafList;
	std::unique_ptr<juce::LookAndFeel> mainLAF = nullptr;

public:
	static LookAndFeelFactory* getInstance();
	static void releaseInstance();

private:
	static LookAndFeelFactory* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeelFactory)
};

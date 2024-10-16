﻿#pragma once

#include <JuceHeader.h>

class LookAndFeelFactory final : private juce::DeletedAtShutdown {
public:
	LookAndFeelFactory() = default;

	void initialise();
	void setDefaultSansSerifTypeface(juce::Typeface::Ptr typeface);

	juce::LookAndFeel_V4* forToolBar() const;
	juce::LookAndFeel_V4* forMainMenu() const;
	juce::LookAndFeel_V4* forSysStatus() const;
	juce::LookAndFeel_V4* forTime() const;
	juce::LookAndFeel_V4* forController() const;
	juce::LookAndFeel_V4* forTools() const;
	juce::LookAndFeel_V4* forMessage() const;
	juce::LookAndFeel_V4* forMessageView() const;
	juce::LookAndFeel_V4* forPluginView() const;
	juce::LookAndFeel_V4* forPluginEditor() const;
	juce::LookAndFeel_V4* forChannelLink() const;
	juce::LookAndFeel_V4* forMixer() const;
	juce::LookAndFeel_V4* forScroller() const;
	juce::LookAndFeel_V4* forColorEditor() const;
	juce::LookAndFeel_V4* forSideChain() const;
	juce::LookAndFeel_V4* forLevelMeter() const;
	juce::LookAndFeel_V4* forMuteButton() const;
	juce::LookAndFeel_V4* forRecButton() const;
	juce::LookAndFeel_V4* forEffect() const;
	juce::LookAndFeel_V4* forSeq() const;
	juce::LookAndFeel_V4* forTimeRuler() const;
	juce::LookAndFeel_V4* forSeqTrack() const;
	juce::LookAndFeel_V4* forSeqTrackName() const;
	juce::LookAndFeel_V4* forInstrName() const;
	juce::LookAndFeel_V4* forSeqBlock() const;
	juce::LookAndFeel_V4* forEditor() const;
	juce::LookAndFeel_V4* forEditorSwitchBar() const;

private:
	std::unique_ptr<juce::LookAndFeel> mainLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> toolBarLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> mainMenuLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> sysStatusLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> timeLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> controllerLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> toolsLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> messageLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> messageViewLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> pluginViewLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> pluginEditorLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> channelLinkLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> mixerLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> scrollerLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> colorEditorLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> sideChainLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> levelMeterLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> muteButtonLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> recButtonLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> effectLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> seqLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> timeRulerLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> seqTrackLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> seqTrackNameLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> instrNameLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> seqBlockLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> editorLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> editorSwitchBarLAF = nullptr;

public:
	static LookAndFeelFactory* getInstance();
	static void releaseInstance();

private:
	static LookAndFeelFactory* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeelFactory)
};

#include "LookAndFeelFactory.h"
#include "MainLookAndFeel.h"
#include "toolbar/ToolBarLookAndFeel.h"
#include "toolbar/MainMenuLookAndFeel.h"
#include "toolbar/SystemStatusLookAndFeel.h"
#include "toolbar/TimeLookAndFeel.h"
#include "toolbar/ControllerLookAndFeel.h"
#include "toolbar/ToolsLookAndFeel.h"
#include "toolbar/MessageLookAndFeel.h"
#include "toolbar/MessageViewLookAndFeel.h"
#include "plugin/PluginViewLookAndFeel.h"
#include "plugin/PluginEditorLookAndFeel.h"
#include "askfor/ChannelLinkLookAndFeel.h"
#include "mixer/MixerLookAndFeel.h"
#include "base/ScrollerLookAndFeel.h"
#include "askfor/ColorEditorLookAndFeel.h"
#include "mixer/SideChainLookAndFeel.h"
#include "mixer/LevelMeterLookAndFeel.h"
#include "base/MuteButtonLookAndFeel.h"
#include "base/SoloButtonLookAndFeel.h"
#include "base/RecButtonLookAndFeel.h"
#include "mixer/EffectLookAndFeel.h"
#include "mixer/SendLookAndFeel.h"
#include "sequencer/SeqLookAndFeel.h"
#include "sequencer/TimeRulerLookAndFeel.h"
#include "sequencer/SeqTrackLookAndFeel.h"
#include "sequencer/SeqTrackNameLookAndFeel.h" 
#include "sequencer/InstrNameLookAndFeel.h"
#include "sequencer/SeqBlockLookAndFeel.h"
#include "editor/EditorLookAndFeel.h"
#include "editor/EditorSwitchBarLookAndFeel.h"
#include "editor/PianoLookAndFeel.h"
#include "editor/MidiContentLookAndFeel.h"
#include "base/InputMonitoringButtonLookAndFeel.h"
#include "../misc/ColorMap.h"
#include <FlowUI.h>

void LookAndFeelFactory::initialise() {
	/** Main LookAndFeel */
	this->mainLAF = std::unique_ptr<juce::LookAndFeel>(new MainLookAndFeel);
	juce::Desktop::getInstance().setDefaultLookAndFeel(this->mainLAF.get());

	/** Set FlowUI Color */
	flowUI::FlowStyle::setTitleBackgroundColor(
		ColorMap::getInstance()->get("ThemeColorB2"));
	flowUI::FlowStyle::setTitleHighlightColor(
		ColorMap::getInstance()->get("ThemeColorB1"));
	flowUI::FlowStyle::setTitleBorderColor(
		ColorMap::getInstance()->get("ThemeColorA2"));
	flowUI::FlowStyle::setTitleSplitColor(
		ColorMap::getInstance()->get("ThemeColorB7"));
	flowUI::FlowStyle::setTitleTextColor(
		ColorMap::getInstance()->get("ThemeColorB10"));
	flowUI::FlowStyle::setTitleTextHighlightColor(
		ColorMap::getInstance()->get("ThemeColorB10"));

	flowUI::FlowStyle::setResizerColor(
		ColorMap::getInstance()->get("ThemeColorB3"));

	flowUI::FlowStyle::setContainerBorderColor(
		ColorMap::getInstance()->get("ThemeColorB3"));

	flowUI::FlowStyle::setButtonIconColor(
		ColorMap::getInstance()->get("ThemeColorB9"));
	flowUI::FlowStyle::setButtonIconBackgroundColor(
		ColorMap::getInstance()->get("ThemeColorB7").withAlpha(0.3f));
	flowUI::FlowStyle::setAdsorbColor(
		ColorMap::getInstance()->get("ThemeColorA2").withAlpha(0.3f));

	/** Look And Feel List */
	this->lafList.ensureStorageAllocated(LAFType::TotalTypeNum);
	this->lafList = {
		new ToolBarLookAndFeel{},
		new MainMenuLookAndFeel{},
		new SystemStatusLookAndFeel{},
		new TimeLookAndFeel{},
		new ControllerLookAndFeel{},
		new ToolsLookAndFeel{},
		new MessageLookAndFeel{},
		new MessageViewLookAndFeel{},
		new PluginViewLookAndFeel{},
		new PluginEditorLookAndFeel{},
		new ChannelLinkLookAndFeel{},
		new MixerLookAndFeel{},
		new ScrollerLookAndFeel{},
		new ColorEditorLookAndFeel{},
		new SideChainLookAndFeel{},
		new LevelMeterLookAndFeel{},
		new MuteButtonLookAndFeel{},
		new SoloButtonLookAndFeel{},
		new RecButtonLookAndFeel{},
		new EffectLookAndFeel{},
		new SendLookAndFeel{},
		new SeqLookAndFeel{},
		new TimeRulerLookAndFeel{},
		new SeqTrackLookAndFeel{},
		new SeqTrackNameLookAndFeel{},
		new InstrNameLookAndFeel{},
		new SeqBlockLookAndFeel{},
		new EditorLookAndFeel{},
		new EditorSwitchBarLookAndFeel{},
		new PianoLookAndFeel{},
		new MidiContentLookAndFeel{},
		new InputMonitoringButtonLookAndFeel{}
	};
}

void LookAndFeelFactory::setDefaultSansSerifTypeface(juce::Typeface::Ptr typeface) {
	auto& laf = juce::Desktop::getInstance().getDefaultLookAndFeel();
	laf.setDefaultSansSerifTypeface(typeface);
}

juce::LookAndFeel* LookAndFeelFactory::getLAFFor(LAFType type) const {
	if (type < this->lafList.size()) {
		return this->lafList[type];
	}
	return nullptr;
}

LookAndFeelFactory* LookAndFeelFactory::getInstance() {
	return LookAndFeelFactory::instance ? LookAndFeelFactory::instance 
		: (LookAndFeelFactory::instance = new LookAndFeelFactory{});
}

void LookAndFeelFactory::releaseInstance() {
	if (LookAndFeelFactory::instance) {
		delete LookAndFeelFactory::instance;
		LookAndFeelFactory::instance = nullptr;
	}
}

LookAndFeelFactory* LookAndFeelFactory::instance = nullptr;

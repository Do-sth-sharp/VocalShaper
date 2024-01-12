#include "LookAndFeelFactory.h"
#include "MainLookAndFeel.h"
#include "ToolBarLookAndFeel.h"
#include "MainMenuLookAndFeel.h"
#include "SystemStatusLookAndFeel.h"
#include "TimeLookAndFeel.h"
#include "ControllerLookAndFeel.h"
#include "ToolsLookAndFeel.h"
#include "MessageLookAndFeel.h"
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

	/** ToolBar */
	this->toolBarLAF = std::make_unique<ToolBarLookAndFeel>();

	/** Main Menu */
	this->mainMenuLAF = std::make_unique<MainMenuLookAndFeel>();

	/** System Status */
	this->sysStatusLAF = std::make_unique<SystemStatusLookAndFeel>();

	/** Time */
	this->timeLAF = std::make_unique<TimeLookAndFeel>();
	
	/** Controller */
	this->controllerLAF = std::make_unique<ControllerLookAndFeel>();

	/** Tools */
	this->toolsLAF = std::make_unique<ToolsLookAndFeel>();

	/** Message */
	this->messageLAF = std::make_unique<MessageLookAndFeel>();
}

void LookAndFeelFactory::setDefaultSansSerifTypeface(juce::Typeface::Ptr typeface) {
	auto& laf = juce::Desktop::getInstance().getDefaultLookAndFeel();
	laf.setDefaultSansSerifTypeface(typeface);
}

juce::LookAndFeel_V4* LookAndFeelFactory::forToolBar() const {
	return this->toolBarLAF.get();
}

juce::LookAndFeel_V4* LookAndFeelFactory::forMainMenu() const {
	return this->mainMenuLAF.get();
}

juce::LookAndFeel_V4* LookAndFeelFactory::forSysStatus() const {
	return this->sysStatusLAF.get();
}

juce::LookAndFeel_V4* LookAndFeelFactory::forTime() const {
	return this->timeLAF.get();
}

juce::LookAndFeel_V4* LookAndFeelFactory::forController() const {
	return this->controllerLAF.get();
}

juce::LookAndFeel_V4* LookAndFeelFactory::forTools() const {
	return this->toolsLAF.get();
}

juce::LookAndFeel_V4* LookAndFeelFactory::forMessage() const {
	return this->messageLAF.get();
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

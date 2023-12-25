#include "LookAndFeelFactory.h"
#include "MainLookAndFeel.h"
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
	this->toolBarLAF = std::make_unique<juce::LookAndFeel_V4>();
	this->toolBarLAF->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));

	/** Main Menu */
	this->mainMenuLAF = std::make_unique<juce::LookAndFeel_V4>();
	this->mainMenuLAF->setColour(juce::TextButton::ColourIds::buttonColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->mainMenuLAF->setColour(juce::TextButton::ColourIds::buttonOnColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->mainMenuLAF->setColour(juce::TextButton::ColourIds::textColourOffId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->mainMenuLAF->setColour(juce::TextButton::ColourIds::textColourOnId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->mainMenuLAF->setColour(juce::ComboBox::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));

	/** System Status */
	this->sysStatusLAF = std::make_unique<juce::LookAndFeel_V4>();
	this->sysStatusLAF->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));

	this->sysStatusLAF->setColour(juce::Label::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->sysStatusLAF->setColour(juce::Label::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));/**< Label Color */
	this->sysStatusLAF->setColour(juce::Label::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	this->sysStatusLAF->setColour(juce::Label::ColourIds::backgroundWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->sysStatusLAF->setColour(juce::Label::ColourIds::textWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));/**< Value Color */
	this->sysStatusLAF->setColour(juce::Label::ColourIds::outlineWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));/**< Curve Color */

	/** Time */
	this->timeLAF = std::make_unique<juce::LookAndFeel_V4>();
	this->timeLAF->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));

	this->timeLAF->setColour(juce::Label::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->timeLAF->setColour(juce::Label::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));/**< Label Color */
	this->timeLAF->setColour(juce::Label::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	this->timeLAF->setColour(juce::Label::ColourIds::backgroundWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->timeLAF->setColour(juce::Label::ColourIds::textWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));/**< Value Color */
	this->timeLAF->setColour(juce::Label::ColourIds::outlineWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));/**< Curve Color */
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

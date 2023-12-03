#include "LookAndFeelFactory.h"
#include "../misc/ColorMap.h"
#include <FlowUI.h>

void LookAndFeelFactory::initialise() {
	auto& laf = juce::Desktop::getInstance().getDefaultLookAndFeel();

	/** Set Menu Color */
	laf.setColour(juce::PopupMenu::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));
	laf.setColour(juce::PopupMenu::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	laf.setColour(juce::PopupMenu::ColourIds::headerTextColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	laf.setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	laf.setColour(juce::PopupMenu::ColourIds::highlightedTextColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));

	/** Set Alert Color */
	laf.setColour(juce::AlertWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));
	laf.setColour(juce::AlertWindow::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	laf.setColour(juce::AlertWindow::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));
	laf.setColour(juce::TextButton::ColourIds::buttonColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));
	laf.setColour(juce::TextButton::ColourIds::buttonOnColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));
	laf.setColour(juce::TextButton::ColourIds::textColourOffId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	laf.setColour(juce::TextButton::ColourIds::textColourOnId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	laf.setColour(juce::ComboBox::ColourIds::outlineColourId,
		juce::Colour::fromRGBA(0, 0, 0, 0));

	/** Set Combo Color */
	laf.setColour(juce::ComboBox::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	laf.setColour(juce::ComboBox::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	laf.setColour(juce::ComboBox::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	laf.setColour(juce::ComboBox::ColourIds::buttonColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));
	laf.setColour(juce::ComboBox::ColourIds::arrowColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	laf.setColour(juce::ComboBox::ColourIds::focusedOutlineColourId,
		ColorMap::getInstance()->get("ThemeColorA1"));

	/** Set Window Background Color */
	laf.setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));

	/** Set List Box Color */
	laf.setColour(juce::ListBox::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	laf.setColour(juce::ListBox::ColourIds::backgroundColourId + 1,
		ColorMap::getInstance()->get("ThemeColorA1"));/**< Background On */
	laf.setColour(juce::ListBox::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	laf.setColour(juce::ListBox::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	laf.setColour(juce::ListBox::ColourIds::textColourId + 1,
		ColorMap::getInstance()->get("ThemeColorB10"));/**< Text On */

	/** Set Text Editor Color */
	laf.setColour(juce::TextEditor::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	laf.setColour(juce::TextEditor::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	laf.setColour(juce::TextEditor::ColourIds::highlightColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	laf.setColour(juce::TextEditor::ColourIds::highlightedTextColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	laf.setColour(juce::TextEditor::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	laf.setColour(juce::TextEditor::ColourIds::focusedOutlineColourId,
		ColorMap::getInstance()->get("ThemeColorA1"));
	laf.setColour(juce::TextEditor::ColourIds::shadowColourId,
		juce::Colour::fromRGBA(0, 0, 0, 0));
	laf.setColour(juce::TextEditor::ColourIds::shadowColourId + 1,
		ColorMap::getInstance()->get("ThemeColorB9"));/**< Empty Text */

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

#include "LookAndFeelFactory.h"
#include "../misc/ColorMap.h"

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

	/** Set Window Background Color */
	laf.setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
}

void LookAndFeelFactory::setDefaultSansSerifTypeface(juce::Typeface::Ptr typeface) {
	auto& laf = juce::Desktop::getInstance().getDefaultLookAndFeel();
	laf.setDefaultSansSerifTypeface(typeface);
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

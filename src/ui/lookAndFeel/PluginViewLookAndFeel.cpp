#include "PluginViewLookAndFeel.h"
#include "../misc/ColorMap.h"

PluginViewLookAndFeel::PluginViewLookAndFeel()
	: MainLookAndFeel() {
	/** Background */
	this->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));

	/** Search Box */
	this->setColour(juce::TextEditor::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextEditor::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::TextEditor::ColourIds::highlightColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	this->setColour(juce::TextEditor::ColourIds::highlightedTextColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextEditor::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextEditor::ColourIds::focusedOutlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextEditor::ColourIds::shadowColourId,
		juce::Colour::fromRGBA(0, 0, 0, 0));
	this->setColour(juce::TextEditor::ColourIds::shadowColourId + 1,
		ColorMap::getInstance()->get("ThemeColorB9"));/**< Empty Text */
	this->setColour(juce::CaretComponent::ColourIds::caretColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));

	/** Plugin Tree */
	this->setColour(juce::TreeView::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::TreeView::ColourIds::linesColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	this->setColour(juce::TreeView::ColourIds::dragAndDropIndicatorColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::TreeView::ColourIds::selectedItemBackgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB4"));
	this->setColour(juce::TreeView::ColourIds::oddItemsColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::TreeView::ColourIds::evenItemsColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));

	/** Plugin Item */
	this->setColour(juce::Label::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));
	this->setColour(juce::Label::ColourIds::backgroundWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	this->setColour(juce::Label::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::Label::ColourIds::outlineWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::Label::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::Label::ColourIds::textWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
}

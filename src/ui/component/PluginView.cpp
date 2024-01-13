#include "PluginView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../Utils.h"
#include <IconManager.h>

PluginView::PluginView()
	: FlowComponent(TRANS("Plugin")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forPluginView());

	/** Search Icon */
	this->searchIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("System", "search-line").getFullPathName());
	this->searchIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextEditor::ColourIds::textColourId));

	/** Search Box */
	this->searchBox = std::make_unique<juce::TextEditor>(TRANS("Search For Plugin"));
	this->searchBox->setHasFocusOutline(false);
	this->searchBox->setMultiLine(false);
	this->searchBox->setJustification(juce::Justification::centredLeft);
	this->searchBox->setClicksOutsideDismissVirtualKeyboard(true);
	this->searchBox->setPopupMenuEnabled(false);
	this->searchBox->setTextToShowWhenEmpty(
		TRANS("Find plugins in the list."),
		this->getLookAndFeel().findColour(
			juce::TextEditor::ColourIds::shadowColourId + 1));
	this->addAndMakeVisible(this->searchBox.get());

	/** Plugin Tree */
	this->pluginTree = std::make_unique<juce::TreeView>(TRANS("Plugin List"));
	this->pluginTree->setDefaultOpenness(true);
	this->pluginTree->setOpenCloseButtonsVisible(true);
	this->addAndMakeVisible(this->pluginTree.get());
}

void PluginView::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int searchHeight = screenSize.getHeight() * 0.03;

	/** Search Box */
	auto searchRect = this->getLocalBounds().withHeight(searchHeight);
	this->searchBox->setBounds(searchRect.withTrimmedLeft(searchRect.getHeight()));

	/** Plugin Tree */
	this->pluginTree->setBounds(this->getLocalBounds().withTrimmedTop(searchHeight));
}

void PluginView::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int searchHeight = screenSize.getHeight() * 0.03;
	float searchIconHeight = searchHeight * 0.6f;

	/** Color */
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour searchBackgroundColor = laf.findColour(
		juce::TextEditor::ColourIds::backgroundColourId);

	/** BackGround */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Search Box */
	auto searchRect = this->getLocalBounds().withHeight(searchHeight);
	g.setColour(searchBackgroundColor);
	g.fillRect(searchRect);

	/** Search Icon */
	auto searchIconArea = searchRect.withWidth(searchRect.getHeight());
	juce::Rectangle<float> searchIconRect(
		searchIconArea.getCentreX() - searchIconHeight / 2,
		searchIconArea.getCentreY() - searchIconHeight / 2,
		searchIconHeight, searchIconHeight);
	this->searchIcon->drawWithin(g, searchIconRect,
		juce::RectanglePlacement::centred | juce::RectanglePlacement::fillDestination, 1.f);
}

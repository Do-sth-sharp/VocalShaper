#include "PluginView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreCallbacks.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"
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
	this->pluginTree->setOpenCloseButtonsVisible(true);
	this->pluginTree->setRootItemVisible(false);
	this->addAndMakeVisible(this->pluginTree.get());

	/** Searching Message */
	this->searchingMes = TRANS("Plugin search in progress, please wait...");

	/** Add Callback */
	CoreCallbacks::getInstance()->addSearchPlugin(
		[comp = PluginView::SafePointer(this)](bool status) {
			if (comp) {
				if (status) { comp->searchStart(); }
				else { comp->searchEnd(); }
			}
		}
	);

	/** Update Plugin List */
	this->update();
}

PluginView::~PluginView() {
	this->pluginTree->setRootItem(nullptr);
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

	int paddingWidth = screenSize.getWidth() * 0.01;
	int paddingHeight = screenSize.getHeight() * 0.02;
	float textHeight = screenSize.getHeight() * 0.02f;

	/** Color */
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour searchBackgroundColor = laf.findColour(
		juce::TextEditor::ColourIds::backgroundColourId);
	juce::Colour textColor = laf.findColour(
		juce::Label::ColourIds::textColourId);

	/** Font */
	juce::Font textFont(textHeight);

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

	/** Searching Text */
	if (!this->pluginTree->isVisible()) {
		juce::Rectangle<int> textRect(
			paddingWidth, searchRect.getBottom() + paddingHeight,
			this->getWidth() - paddingWidth * 2, textHeight);
		g.setFont(textFont);
		g.setColour(textColor);
		g.drawFittedText(this->searchingMes, textRect,
			juce::Justification::centred, 1, 1.f);
	}
}

void PluginView::update() {
	/** Clear Plugin Tree */
	this->pluginTree->setRootItem(nullptr);
	this->pluginModel = nullptr;

	/** Get Plugin List */
	auto [valid, list] = quickAPI::getPluginList();
	if (valid) {
		/** Plugin Groups */
		std::map<juce::String, juce::Array<juce::PluginDescription>> groupMap;

		/** Group Plugin */
		for (auto& i : list) {
			/** Group Index */
			juce::String index;
			switch (this->groupType) {
			case GroupType::Format:
				index = i.pluginFormatName;
				break;
			case GroupType::Manufacturer:
				index = i.manufacturerName;
				break;
			case GroupType::Category:
				index = i.category;
				break;
			}

			/** Insert To Group List */
			auto& groupList = groupMap[index];
			groupList.add(i);
		}

		/** Plugin Group List */
		juce::Array<PluginTreeModel::PluginClass> groupList;
		for (auto& i : groupMap) {
			groupList.add({ i.first, i.second });
		}

		/** Create Plugin Tree Model */
		this->pluginModel = std::make_unique<PluginTreeModel>(groupList);
		this->pluginTree->setRootItem(this->pluginModel.get());
		this->pluginTree->setDefaultOpenness(false);
	}
}

void PluginView::searchStart() {
	/** Hide Plugin Tree */
	this->pluginTree->setVisible(false);

	/** Clear Plugin Tree */
	this->pluginTree->setRootItem(nullptr);
	this->pluginModel = nullptr;
}

void PluginView::searchEnd() {
	/** Update Plugin Tree */
	this->update();

	/** Show Plugin Tree */
	this->pluginTree->setVisible(true);
}

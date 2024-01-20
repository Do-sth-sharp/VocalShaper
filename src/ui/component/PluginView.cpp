#include "PluginView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreCallbacks.h"
#include "../misc/CoreActions.h"
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
	this->searchBox->onReturnKey = [this] { this->searchUpdate(); };
	this->searchBox->onTextChange = [this] {if (this->searchBox->isEmpty()) { this->update(); }};
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
		/** Group Plugins */
		auto searchText = this->searchBox->getText();
		bool search = searchText.isNotEmpty();
		auto groupList = utils::groupPlugin(
			list, this->groupType, search, searchText);

		/** Create Plugin Tree Model */
		this->pluginModel = std::make_unique<PluginTreeModel>(groupList,
			[this](const juce::String& name) { this->showGroupMenu(name); },
			[this](const juce::PluginDescription& plugin) { this->showPluginMenu(plugin); });
		this->pluginTree->setRootItem(this->pluginModel.get());
		this->pluginTree->setDefaultOpenness(search);
	}
}

void PluginView::searchUpdate() {
	this->update();
}

void PluginView::rescan() {
	CoreActions::rescanPlugins();
}

void PluginView::expandAll() {
	if (this->pluginModel) {
		this->pluginModel->changeAllOpenness(true);
	}
}

void PluginView::foldAll() {
	if (this->pluginModel) {
		this->pluginModel->changeAllOpenness(false);
	}
}

void PluginView::showGroupMenu(const juce::String& name) {
	auto menu = this->createGroupMenu();
	int result = menu.show();

	switch (result) {
	case 1:
		this->expandAll();
		break;
	case 2:
		this->foldAll();
		break;
	case 3:
		this->groupType = utils::PluginGroupType::Format;
		this->update();
		break;
	case 4:
		this->groupType = utils::PluginGroupType::Manufacturer;
		this->update();
		break;
	case 5:
		this->groupType = utils::PluginGroupType::Category;
		this->update();
		break;
	case 6:
		this->rescan();
		break;
	}
}

void PluginView::showPluginMenu(const juce::PluginDescription& plugin) {
	auto menu = this->createPluginMenu();
	int result = menu.show();

	switch (result) {
	case 1:
		this->expandAll();
		break;
	case 2:
		this->foldAll();
		break;
	case 3:
		juce::Process::openDocument(
			juce::File{ plugin.fileOrIdentifier }.getParentDirectory().getFullPathName(), "");
		break;
	case 4:
		if (CoreActions::addPluginBlackListGUI(plugin.fileOrIdentifier)) {
			juce::AlertWindow::showMessageBox(juce::MessageBoxIconType::InfoIcon,
				TRANS("Add to Blacklist"), TRANS("This will take effect during the next plugin scan."));
		}
		break;
	case 5:
		this->rescan();
		break;
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

juce::PopupMenu PluginView::createGroupMenu() const {
	juce::PopupMenu menu;

	menu.addItem(1, TRANS("Expand All"), true);
	menu.addItem(2, TRANS("Fold All"), true);
	menu.addSeparator();
	menu.addItem(3, TRANS("Group by Format"), true, this->groupType == utils::PluginGroupType::Format);
	menu.addItem(4, TRANS("Group by Manufacturer"), true, this->groupType == utils::PluginGroupType::Manufacturer);
	menu.addItem(5, TRANS("Group by Category"), true, this->groupType == utils::PluginGroupType::Category);
	menu.addSeparator();
	menu.addItem(6, TRANS("Rescan Plugins"), true);

	return menu;
}

juce::PopupMenu PluginView::createPluginMenu() const {
	juce::PopupMenu menu;

	menu.addItem(1, TRANS("Expand All"), true);
	menu.addItem(2, TRANS("Fold All"), true);
	menu.addSeparator();
	menu.addItem(3, TRANS("Open Plugin Folder"), true);
	menu.addItem(4, TRANS("Add to Blacklist"), true);
	menu.addSeparator();
	menu.addItem(5, TRANS("Rescan Plugins"), true);

	return menu;
}

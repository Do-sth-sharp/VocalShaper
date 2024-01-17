#include "PluginConfigComponent.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

PluginConfigComponent::PluginConfigComponent() {
	/** Tip Label */
	this->tipLabel = std::make_unique<juce::Label>(
		TRANS("Tip"), TRANS("The following settings will take effect the next time the plugins are scanned."));
	this->addAndMakeVisible(this->tipLabel.get());

	/** Rescan Button */
	this->rescanButton = std::make_unique<juce::TextButton>(TRANS("Rescan Plugins"));
	this->rescanButton->setWantsKeyboardFocus(false);
	this->rescanButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->rescanButton->onClick = [this] { this->rescan(); };
	this->addAndMakeVisible(this->rescanButton.get());

	/** Plugin Configs */
	auto pluginPath = quickAPI::getPluginSearchPath();
	auto pluginBlackList = quickAPI::getPluginBlackList();

	/** Plugin Path */
	this->pathLabel = std::make_unique<juce::Label>(
		TRANS("Plugin Search Path"), TRANS("Plugin Search Path"));
	this->addAndMakeVisible(this->pathLabel.get());

	this->pathModel = std::make_unique<EditableStringListModel>(
		pluginPath, [this](int index) { this->addPath(index); },
		[](int, const juce::String&) {},
		[this](int index, const juce::String& data) { this->removePath(index, data); },
		EditableStringListModel::MenuType::MenuNoEditItem);
	this->pathList = std::make_unique<juce::ListBox>(
		TRANS("Plugin Search Path"), this->pathModel.get());
	this->addAndMakeVisible(this->pathList.get());

	/** Plugin Black List */
	this->blackListLabel = std::make_unique<juce::Label>(
		TRANS("Plugin Black List"), TRANS("Plugin Black List"));
	this->addAndMakeVisible(this->blackListLabel.get());

	this->blackListModel = std::make_unique<EditableStringListModel>(
		pluginBlackList, [this](int index) { this->addBlackList(index); },
		[](int, const juce::String&) {},
		[this](int index, const juce::String& data) { this->removeBlackList(index, data); },
		EditableStringListModel::MenuType::MenuNoEditItem);
	this->blackList = std::make_unique<juce::ListBox>(
		TRANS("Plugin Black List"), this->blackListModel.get());
	this->addAndMakeVisible(this->blackList.get());
}

int PluginConfigComponent::getHeightPrefered() const {
	auto screenSize = utils::getScreenSize(this);
	return screenSize.getHeight() * 0.8;
}

void PluginConfigComponent::resized() {
	auto screenSize = utils::getScreenSize(this);

	/** Sizes */
	int itemHeight = screenSize.getHeight() * 0.04;
	int pluginListHeight = screenSize.getHeight() * 0.35;
	int blackListHeight = screenSize.getHeight() * 0.35;
	int labelWidth = std::min(200, screenSize.getWidth() / 3);
	int listRowHeight = itemHeight * 0.8;

	int rescanButtonAreaWidth = screenSize.getWidth() * 0.075;
	int rescanButtonWidth = rescanButtonAreaWidth * 0.8;
	int rescanButtonHeight = itemHeight * 0.8;

	/** Tip */
	juce::Rectangle<int> tipLabelRect(
		0, 0, this->getWidth() - rescanButtonAreaWidth, itemHeight);
	this->tipLabel->setBounds(tipLabelRect);

	/** Rescan Button */
	juce::Rectangle<int> rescanButtonRect(
		tipLabelRect.getRight() + (rescanButtonAreaWidth - rescanButtonWidth) / 2,
		(itemHeight - rescanButtonHeight) / 2,
		rescanButtonWidth, rescanButtonHeight);
	this->rescanButton->setBounds(rescanButtonRect);

	/** Plugin Search Path */
	juce::Rectangle<int> searchPathLabelRect(
		0, tipLabelRect.getBottom(), labelWidth, pluginListHeight);
	this->pathLabel->setBounds(searchPathLabelRect);

	juce::Rectangle<int> searchPathListRect(
		labelWidth, tipLabelRect.getBottom(), this->getWidth() - labelWidth, pluginListHeight);
	this->pathList->setBounds(searchPathListRect);
	this->pathList->setRowHeight(listRowHeight);

	/** Plugin Black List */
	juce::Rectangle<int> blackListLabelRect(
		0, searchPathLabelRect.getBottom() + itemHeight, labelWidth, blackListHeight);
	this->blackListLabel->setBounds(blackListLabelRect);

	juce::Rectangle<int> blackListRect(
		labelWidth, searchPathLabelRect.getBottom() + itemHeight, this->getWidth() - labelWidth, blackListHeight);
	this->blackList->setBounds(blackListRect);
	this->blackList->setRowHeight(listRowHeight);
}

void PluginConfigComponent::addPath(int index) {
	if (!this->pathModel) { return; }

	auto defaultPath = utils::getAppRootDir();
	auto chooser = std::make_unique<juce::FileChooser>(
		TRANS("Add Plugin Search Path"), defaultPath, "*");
	if (chooser->browseForDirectory()) {
		juce::File result = chooser->getResult();

		juce::String path = result.getFullPathName().replaceCharacter('\\', '/');
		if (result.isAChildOf(defaultPath)) {
			path = "./" + result.getRelativePathFrom(defaultPath).replaceCharacter('\\', '/');
		}

		if (this->pathModel->contains(path)) {
			juce::AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon,
				TRANS("Add Plugin Search Path"),
				TRANS("The path is already in the list:") + " " + path);
			return;
		}

		if (!CoreActions::addPluginSearchPathGUI(path)) {
			return;
		}

		this->pathModel->insert(index, path);
		this->pathList->updateContent();
	}
}

void PluginConfigComponent::removePath(int index, const juce::String& path) {
	if (this->pathModel) {
		if (!CoreActions::removePluginSearchPathGUI(path)) {
			return;
		}
		
		this->pathModel->remove(index);
		this->pathList->updateContent();
	}
}

void PluginConfigComponent::addBlackList(int index) {
	if (!this->blackListModel) { return; }

	auto pluginFormats = utils::getPluginFormatsSupported();

	auto defaultPath = utils::getAppRootDir();
	auto chooser = std::make_unique<juce::FileChooser>(
		TRANS("Add Plugin Black List"), defaultPath,
		pluginFormats.joinIntoString(","));
	if (chooser->browseForMultipleFilesToOpen()) {
		auto results = chooser->getResults();

		for (auto& i : results) {
			juce::String path = i.getFullPathName();

			if (this->blackListModel->contains(path)) {
				juce::AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon,
					TRANS("Add Plugin Black List"),
					TRANS("The plugin is already in the list:") + " " + path);
				continue;
			}

			if (!CoreActions::addPluginBlackListGUI(path)) {
				return;
			}

			this->blackListModel->insert(index, path);
		}
		
		this->blackList->updateContent();
	}
}

void PluginConfigComponent::removeBlackList(int index, const juce::String& path) {
	if (this->blackListModel) {
		if (!CoreActions::removePluginBlackListGUI(path)) {
			return;
		}
		this->blackListModel->remove(index);
		this->blackList->updateContent();
	}
}

void PluginConfigComponent::rescan() {
	CoreActions::rescanPlugins();
}

#include "PluginConfigComponent.h"
#include "../Utils.h"

PluginConfigComponent::PluginConfigComponent() {
	/** Plugin Path */
	this->pathLabel = std::make_unique<juce::Label>(
		TRANS("Plugin Search Path"), TRANS("Plugin Search Path"));
	this->addAndMakeVisible(this->pathLabel.get());

	this->pathList = std::make_unique<juce::ListBox>(
		TRANS("Plugin Search Path"), nullptr);
	this->addAndMakeVisible(this->pathList.get());

	/** Plugin Black List */
	this->blackListLabel = std::make_unique<juce::Label>(
		TRANS("Plugin Black List"), TRANS("Plugin Black List"));
	this->addAndMakeVisible(this->blackListLabel.get());

	this->blackList = std::make_unique<juce::ListBox>(
		TRANS("Plugin Black List"), nullptr);
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

	/** Plugin Search Path */
	juce::Rectangle<int> searchPathLabelRect(
		0, 0, this->getWidth(), itemHeight);
	this->pathLabel->setBounds(searchPathLabelRect);

	juce::Rectangle<int> searchPathListRect(
		0, searchPathLabelRect.getBottom(), this->getWidth(), pluginListHeight);
	this->pathList->setBounds(searchPathListRect);

	/** Plugin Black List */
	juce::Rectangle<int> blackListLabelRect(
		0, searchPathListRect.getBottom(), this->getWidth(), itemHeight);
	this->blackListLabel->setBounds(blackListLabelRect);

	juce::Rectangle<int> blackListRect(
		0, blackListLabelRect.getBottom(), this->getWidth(), blackListHeight);
	this->blackList->setBounds(blackListRect);
}

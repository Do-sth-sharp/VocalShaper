#include "Plugin.h"

Plugin::Plugin() {
	/** Audio Plugin Manager */
	this->audioPluginSearchThread = std::make_unique<AudioPluginSearchThread>();
}

const juce::StringArray Plugin::getPluginTypeList() const {
	auto formatList = this->audioPluginSearchThread->getFormats();

	juce::StringArray result;
	for (auto i : formatList) {
		result.add(i->getName());
	}

	return result;
}

const std::tuple<bool, juce::KnownPluginList&> Plugin::getPluginList() const {
	return this->audioPluginSearchThread->getPluginList();
}

void Plugin::clearPluginList() {
	this->audioPluginSearchThread->clearList();
}

void Plugin::clearPluginTemporary() {
	this->audioPluginSearchThread->clearTemporary();
}

bool Plugin::pluginSearchThreadIsRunning() const {
	return this->audioPluginSearchThread->isThreadRunning();
}

const std::unique_ptr<juce::PluginDescription> Plugin::findPlugin(
	const juce::String& identifier, bool isInstrument, bool isARA) const {
	auto& list = std::get<1>(this->getPluginList());

	auto ptr = list.getTypeForIdentifierString(identifier);
	if (ptr && (ptr->isInstrument == isInstrument || (isARA && ptr->hasARAExtension))) {
		return std::move(ptr);
	}

	return nullptr;
}

const juce::StringArray Plugin::getPluginBlackList() const {
	return this->audioPluginSearchThread->getBlackList();
}

void Plugin::addToPluginBlackList(const juce::String& plugin) const {
	this->audioPluginSearchThread->addToBlackList(plugin);
}

void Plugin::removeFromPluginBlackList(const juce::String& plugin) const {
	this->audioPluginSearchThread->removeFromBlackList(plugin);
}

const juce::StringArray Plugin::getPluginSearchPath() const {
	return this->audioPluginSearchThread->getSearchPath();
}

void Plugin::addToPluginSearchPath(const juce::String& path) const {
	this->audioPluginSearchThread->addToSearchPath(path);
}

void Plugin::removeFromPluginSearchPath(const juce::String& path) const {
	this->audioPluginSearchThread->removeFromSearchPath(path);
}

Plugin* Plugin::getInstance() {
	return Plugin::instance ? Plugin::instance : (Plugin::instance = new Plugin);
}

void Plugin::releaseInstance() {
	if (Plugin::instance) {
		delete Plugin::instance;
		Plugin::instance = nullptr;
	}
}

Plugin* Plugin::instance = nullptr;

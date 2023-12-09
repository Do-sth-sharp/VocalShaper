#include "ConfigManager.h"
#include "../Utils.h"

void ConfigManager::clear() {
	this->confList.clear();
}

void ConfigManager::loadConfigs() {
	juce::File configDir = utils::getConfigDir();
	auto fileList = configDir.findChildFiles(juce::File::findFiles,
		true, "*.json", juce::File::FollowSymlinks::noCycles);
	for (auto& i : fileList) {
		auto var = juce::JSON::parse(i);
		if (var.isObject()) {
			this->confList.insert(std::make_pair(i.getFileNameWithoutExtension(), var));
		}
	}
}

bool ConfigManager::saveConfig(const juce::String& name) const {
	auto it = this->confList.find(name);
	if (it == this->confList.end()) { return false; }

	return ConfigManager::save(name, it->second);
}

bool ConfigManager::saveConfigs() const {
	bool result = true;
	for (auto& i : this->confList) {
		if (!ConfigManager::save(i.first, i.second)) {
			result = false;
		}
	}
	return result;
}

juce::var& ConfigManager::get(const juce::String& name) {
	return this->confList[name];
}

bool ConfigManager::save(const juce::String& name, const juce::var& var) {
	juce::File configDir = utils::getConfigDir();
	juce::File configFile = configDir.getChildFile("./" + name + ".json");

	juce::FileOutputStream stream(configFile);
	if (!stream.openedOk()) { return false; }
	stream.setPosition(0);
	stream.truncate();

	juce::JSON::writeToStream(stream, var);
	return false;
}

ConfigManager* ConfigManager::getInstance() {
	return ConfigManager::instance ? ConfigManager::instance
		: (ConfigManager::instance = new ConfigManager{});
}

void ConfigManager::releaseInstance() {
	if (ConfigManager::instance) {
		delete ConfigManager::instance;
		ConfigManager::instance = nullptr;
	}
}

ConfigManager* ConfigManager::instance = nullptr;

#include "Serializable.h"
#include "../Utils.h"

SerializeConfig Serializable::createSerializeConfigQuickly() {
	SerializeConfig config{};

	config.projectDir = utils::getProjectDir().getFullPathName();
	config.projectFileName = "_";
	config.projectFilePath = juce::File{ config.projectDir }
		.getChildFile("./" + config.projectFileName).getFullPathName();
	config.araDir = utils::getARADataDir(config.projectDir, config.projectFileName).getFullPathName();

	return config;
}

ParseConfig Serializable::createParseConfigQuickly() {
	ParseConfig config{};

	config.projectDir = utils::getProjectDir().getFullPathName();
	config.projectFileName = "_";
	config.projectFilePath = juce::File{ config.projectDir }
	.getChildFile("./" + config.projectFileName).getFullPathName();
	config.araDir = utils::getARADataDir(config.projectDir, config.projectFileName).getFullPathName();

	return config;
}

#include "AudioPluginSearchThread.h"
#include "AudioConfig.h"

/** Fixed JUCE Symbol Export */
namespace juce {
	XmlElement::TextFormat::TextFormat() {}
}

AudioPluginSearchThread::AudioPluginSearchThread()
	: Thread("Audio Plugin Search Thread") {
	this->audioPluginManager = std::make_unique<juce::AudioPluginFormatManager>();
	this->audioPluginManager->addDefaultFormats();
}

AudioPluginSearchThread::~AudioPluginSearchThread() {
	if (this->isThreadRunning()) {
		this->stopThread(-1);
	}
}

const juce::Array<juce::AudioPluginFormat*> AudioPluginSearchThread::getFormats() const {
	return this->audioPluginManager->getFormats();
}

std::tuple<bool, juce::KnownPluginList&> AudioPluginSearchThread::getPluginList() {
	if (this->isThreadRunning()) {
		return std::tuple<bool, juce::KnownPluginList&>(false, this->pluginList);
	}

	if (!this->pluginListValidFlag) {
		this->startThread();
		return std::tuple<bool, juce::KnownPluginList&>(false, this->pluginList);
	}
	return std::tuple<bool, juce::KnownPluginList&>(true, this->pluginList);
}

void AudioPluginSearchThread::clearList() {
	if (this->isThreadRunning()) {
		this->stopThread(-1);
	}
	this->pluginList.clear();
	this->pluginList.clearBlacklistedFiles();
	this->pluginListValidFlag = false;
}

void AudioPluginSearchThread::clearTemporary() {
	if (this->isThreadRunning()) {
		this->stopThread(-1);
	}
	this->pluginList.clear();
	this->pluginList.clearBlacklistedFiles();
	this->clearTemporaryInternal();
	this->pluginListValidFlag = false;
}

const juce::StringArray AudioPluginSearchThread::getBlackList() const {
	return this->pluginList.getBlacklistedFiles();
}

void AudioPluginSearchThread::addToBlackList(const juce::String& plugin) {
	this->pluginList.addToBlacklist(plugin);

	juce::String blackListFilePath = AudioConfig::getPluginBlackListFilePath();
	juce::File blackListFile =
		juce::File::getCurrentWorkingDirectory().getChildFile(blackListFilePath);
	this->saveBlackListInternal(blackListFile);
}

void AudioPluginSearchThread::removeFromBlackList(const juce::String& plugin) {
	this->pluginList.removeFromBlacklist(plugin);

	juce::String blackListFilePath = AudioConfig::getPluginBlackListFilePath();
	juce::File blackListFile =
		juce::File::getCurrentWorkingDirectory().getChildFile(blackListFilePath);
	this->saveBlackListInternal(blackListFile);
}

const juce::StringArray AudioPluginSearchThread::getSearchPath() const {
	juce::File file = this->getSearchPathFileInternal();

	juce::StringArray result;
	juce::File pathListFile = this->getSearchPathFileInternal();
	if (pathListFile.existsAsFile()) {
		pathListFile.readLines(result);
	}

	return result;
}

void AudioPluginSearchThread::addToSearchPath(const juce::String& path) const {
	auto paths = this->getSearchPath();

	if (!paths.contains(path)) {
		paths.add(path);
	}

	this->saveSearchPathInternal(paths);
}

void AudioPluginSearchThread::removeFromSearchPath(const juce::String& path) const {
	auto paths = this->getSearchPath();

	paths.removeString(path);

	this->saveSearchPathInternal(paths);
}

void AudioPluginSearchThread::run() {
	/** Get Temporary File */
	juce::String temporaryFilePath = AudioConfig::getPluginListTemporaryFilePath();
	juce::File temporaryFile =
		juce::File::getCurrentWorkingDirectory().getChildFile(temporaryFilePath);
	if (temporaryFile.existsAsFile()) {
		/** Load From Xml File */
		auto xmlElement = juce::XmlDocument::parse(temporaryFile);
		if (xmlElement) {
			/** Create Plugin List From Xml Data */
			this->pluginList.recreateFromXml(*xmlElement);

			/** Set List Valid Flag And Return */
			this->pluginListValidFlag = true;
			return;
		}
		else {
			/** The Xml File Is Invalid, Remove It */
			temporaryFile.deleteFile();
		}
	}

	/** Get Black List */
	juce::StringArray blackList;
	juce::String blackListFilePath = AudioConfig::getPluginBlackListFilePath();
	juce::File blackListFile =
		juce::File::getCurrentWorkingDirectory().getChildFile(blackListFilePath);
	if (blackListFile.existsAsFile()) {
		blackListFile.readLines(blackList);

		/** Apply Black List */
		this->pluginList.clearBlacklistedFiles();
		for (auto& s : blackList) {
			this->pluginList.addToBlacklist(s);
		}
	}

	/** Get Search Path */
	juce::StringArray searchPathList = this->getSearchPath();
	juce::FileSearchPath searchPath;
	for (auto& s : searchPathList) {
		searchPath.addPath(s);
	}

	/** Get Dead List Path */
	juce::String deadListPath = AudioConfig::getDeadPluginListPath();
	juce::File deadListFile =
		juce::File::getCurrentWorkingDirectory().getChildFile(deadListPath);
	if (!deadListFile.exists()) {
		deadListFile.createDirectory();
	}

	/** Get All Support Plugin Type */
	auto pluginTypes = this->audioPluginManager->getFormats();
	juce::StringArray deadPlugins;
	for (auto type : pluginTypes) {
		/** Prepare Dead List File */
		juce::File deadFile = deadListFile.getChildFile(type->getName() + ".txt");

		/** Init The Scanner */
		juce::PluginDirectoryScanner scanner(
			this->pluginList, *type, searchPath, true, deadFile, true);

		/** Scan Plugins */
		juce::String currentPlugin;
		while (scanner.scanNextFile(true, currentPlugin)) {
			/** Check For Stop */
			if (this->threadShouldExit()) {
				this->pluginList.clear();
				this->pluginList.clearBlacklistedFiles();
				this->pluginListValidFlag = false;
				return;
			}
		}

		/** Save Dead Plugins */
		deadPlugins.addArray(scanner.getFailedFiles());
	}

	/** Apply Dead Plugins As Black List */
	for (auto& s : deadPlugins) {
		this->pluginList.addToBlacklist(s);
	}
	
	/** Save Black List File */
	this->saveBlackListInternal(blackListFile);

	/** Save Plugin Temporary */
	auto xmlTemp = this->pluginList.createXml();
	if (xmlTemp) {
		if (!temporaryFile.getParentDirectory().exists()) {
			temporaryFile.getParentDirectory().createDirectory();
		}
		temporaryFile.replaceWithText(xmlTemp->toString());
	}

	/** Set List Valid Flag */
	this->pluginListValidFlag = true;
}

void AudioPluginSearchThread::clearTemporaryInternal() const {
	juce::String temporaryFilePath = AudioConfig::getPluginListTemporaryFilePath();
	juce::File temporaryFile =
		juce::File::getCurrentWorkingDirectory().getChildFile(temporaryFilePath);
	if (temporaryFile.existsAsFile()) {
		/** Remove Temporary File */
		temporaryFile.deleteFile();
	}
}

void AudioPluginSearchThread::saveBlackListInternal(const juce::File& file) const {
	if (!file.getParentDirectory().exists()) {
		file.getParentDirectory().createDirectory();
	}
	file.replaceWithText(
		this->pluginList.getBlacklistedFiles().joinIntoString("\n"));
}

const juce::File AudioPluginSearchThread::getSearchPathFileInternal() const {
	juce::String pathListFilePath = AudioConfig::getPluginSearchPathListFilePath();
	return juce::File::getCurrentWorkingDirectory().getChildFile(pathListFilePath);
}

void AudioPluginSearchThread::saveSearchPathInternal(const juce::StringArray& paths) const {
	juce::File file = this->getSearchPathFileInternal();
	if (!file.getParentDirectory().exists()) {
		file.getParentDirectory().createDirectory();
	}
	file.replaceWithText(paths.joinIntoString("\n"));
}

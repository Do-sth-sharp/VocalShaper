#include "AudioPluginSearchThread.h"
#include "AudioConfig.h"

/** Plugin Searcher Path */
#if JUCE_WINDOWS
#define PluginSearcherPath "./PluginSearcher.exe"
#elif JUCE_LINUX
#define PluginSearcherPath "./PluginSearcher"
#elif JUCE_MAC
#define PluginSearcherPath "./PluginSearcher"
#else
#define PluginSearcherPath "./PluginSearcher"
#endif // JUCE_WINDOWS

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
	if (file.existsAsFile()) {
		file.readLines(result);
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

	/** If Temporary File Doesn't Exist */
	if (!temporaryFile.existsAsFile()) {
		/** Init Child Process */
		juce::ChildProcess process;

		/** Get Searcher Path */
		juce::File searcher = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
			.getParentDirectory().getChildFile(PluginSearcherPath);
		juce::String searcherPath = searcher.getFullPathName().quoted();

		/** Get Command Args */
		juce::String arg0 = AudioConfig::getPluginSearchPathListFilePath().quoted();
		juce::String arg1 = AudioConfig::getPluginListTemporaryFilePath().quoted();
		juce::String arg2 = AudioConfig::getPluginBlackListFilePath().quoted();
		juce::String arg3 = AudioConfig::getDeadPluginListPath().quoted();

		/** Get Process Command */
		juce::String processCommand
			= searcherPath + " " + arg0 + " " + arg1 + " " + arg2 + " " + arg3;

		/** Start Process */
		process.start(processCommand, 0);

		/** Wait For Process Finished */
		while (process.isRunning()) {
			/** Check If Thread Terminate */
			if (this->threadShouldExit()) {
				process.kill();
				return;
			}

			/** Thread Sleep */
			juce::Thread::sleep(10);
		}
	}

	/** Read Temporary File */
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

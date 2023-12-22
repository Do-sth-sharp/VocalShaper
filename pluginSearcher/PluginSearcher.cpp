#include "PluginSearcher.h"

#define OUT(x) \
    DBG(x); \
    std::cout << (x) << std::endl

/** Fixed JUCE Symbol Export */
namespace juce {
	XmlElement::TextFormat::TextFormat() {}
}

PluginSearcher::PluginSearcher(
	const juce::String& pluginSearchPathListFilePath,
	const juce::String& pluginListTemporaryFilePath,
	const juce::String& pluginBlackListFilePath,
	const juce::String& deadPluginListPath)
	: pluginSearchPathListFilePath(pluginSearchPathListFilePath),
	pluginListTemporaryFilePath(pluginListTemporaryFilePath),
	pluginBlackListFilePath(pluginBlackListFilePath),
	deadPluginListPath(deadPluginListPath) {}

void PluginSearcher::start() {
	/** Search Result */
	juce::KnownPluginList plugins;

	/** Plugin Format Manager */
	juce::AudioPluginFormatManager formatManager;
	formatManager.addDefaultFormats();

	/** Get Search Path */
	auto searchPathList = this->getSearchPath();
	juce::FileSearchPath searchPath;
	for (auto& s : searchPathList) {
		searchPath.addPath(juce::File::getSpecialLocation(juce::File::hostApplicationPath)
			.getChildFile(s).getFullPathName());
	}

	/** Get Black List */
	auto blackList = this->getBlackList();
	for (auto& s : blackList) {
		plugins.addToBlacklist(s);
	}

	/** Get Dead List Path */
	juce::File deadListFile =
		juce::File::getSpecialLocation(juce::File::hostApplicationPath)
		.getChildFile(this->deadPluginListPath);
	if (!deadListFile.exists()) {
		deadListFile.createDirectory();
	}

	/** Get All Support Plugin Type */
	auto formats = formatManager.getFormats();
	juce::StringArray deadPlugins;
	for (auto type : formats) {
		/** Prepare Dead List File */
		juce::File deadFile = deadListFile.getChildFile(type->getName() + ".txt");

		/** Init The Scanner */
		juce::PluginDirectoryScanner scanner(
			plugins, *type, searchPath, true, deadFile, true);

		/** Scan Plugins */
		juce::String nextPlugin;
		bool scanFlag = false;
		do {
			nextPlugin = scanner.getNextPluginFileThatWillBeScanned();
			if (nextPlugin.isNotEmpty()) {
				if (blackList.contains(nextPlugin)) {
					OUT("\033[33m[SKIP]\033[0m " + nextPlugin.toStdString());
					scanFlag = scanner.skipNextFile();
				}
				else {
					OUT("\033[35m[SCAN]\033[0m " + nextPlugin.toStdString());
					scanFlag = scanner.scanNextFile(true, nextPlugin);
				}
			}
		} while(scanFlag);
		/*while (scanner.scanNextFile(true, currentPlugin)) {
			OUT("\033[35m[SCAN]\033[0m " + currentPlugin.toStdString());
		}*/

		/** Save Dead Plugins */
		deadPlugins.addArray(scanner.getFailedFiles());
	}

	/** Apply Dead Plugins As Black List */
	for (auto& s : deadPlugins) {
		plugins.addToBlacklist(s);
	}

	/** Save Black List */
	this->saveBlackList(plugins.getBlacklistedFiles());

	/** Save Search Result */
	auto xmlData = plugins.createXml();
	this->saveTemporaryFile(xmlData.get());

	/** Finished */
	OUT("\033[32m[FINISHED]\033[0m");
	juce::JUCEApplication::quit();
}

const juce::File PluginSearcher::getSearchPathFile() const {
	return juce::File::getSpecialLocation(juce::File::hostApplicationPath).getParentDirectory()
		.getChildFile(this->pluginSearchPathListFilePath);
}

const juce::StringArray PluginSearcher::getSearchPath() const {
	auto file = this->getSearchPathFile();

	juce::StringArray result;
	if (file.existsAsFile()) {
		file.readLines(result);
	}
	result.removeEmptyStrings(true);

	return result;
}

const juce::File PluginSearcher::getBlackListFile() const {
	return juce::File::getSpecialLocation(juce::File::hostApplicationPath).getParentDirectory()
		.getChildFile(this->pluginBlackListFilePath);
}

const juce::StringArray PluginSearcher::getBlackList() const {
	auto file = this->getBlackListFile();

	juce::StringArray result;
	if (file.existsAsFile()) {
		file.readLines(result);
	}
	result.removeEmptyStrings(true);

	return result;
}

void PluginSearcher::saveBlackList(const juce::StringArray& list) const {
	auto file = this->getBlackListFile();
	if (!file.getParentDirectory().exists()) {
		file.getParentDirectory().createDirectory();
	}

	file.replaceWithText(list.joinIntoString("\n"));
}

const juce::File PluginSearcher::getTemporaryFile() const {
	return juce::File::getSpecialLocation(juce::File::hostApplicationPath).getParentDirectory()
		.getChildFile(this->pluginListTemporaryFilePath);
}

void PluginSearcher::saveTemporaryFile(const juce::XmlElement* data) const {
	if (!data) { return; }

	auto file = this->getTemporaryFile();
	if (!file.getParentDirectory().exists()) {
		file.getParentDirectory().createDirectory();
	}

	juce::FileOutputStream ostream(file);
	if (!ostream.openedOk()) { return; }
	ostream.setPosition(0);
	ostream.truncate();

	data->writeTo(ostream, juce::XmlElement::TextFormat{});
}

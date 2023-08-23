#include "PluginLoader.h"
#include "../AudioCore.h"
#include "../Utils.h"

PluginLoader::PluginLoader() {
	this->loadThread = std::make_unique<PluginLoadThread>();
}

void PluginLoader::loadPlugin(
	const juce::PluginDescription& pluginInfo,
	const PluginLoader::PluginLoadCallback& callback) {
	/** Get Audio Config */
	auto mainGraph = AudioCore::getInstance()->getGraph();
	if (!mainGraph) {
		jassertfalse;
		return;
	}
	double sampleRate = mainGraph->getSampleRate();
	int bufferSize = mainGraph->getBlockSize();

	/** Create Task */
	this->loadThread->load(pluginInfo, callback, sampleRate, bufferSize);
}

PluginLoader* PluginLoader::getInstance() {
	return PluginLoader::instance ? PluginLoader::instance : (PluginLoader::instance = new PluginLoader());
}

PluginLoader* PluginLoader::instance = nullptr;

#include "PluginLoader.h"
#include "AudioCore.h"
#include "Utils.h"

PluginLoader::PluginLoader() {
	this->pluginFormatManager = std::make_unique<juce::AudioPluginFormatManager>();
	this->pluginFormatManager->addDefaultFormats();
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

	/** Create Callback */
	auto asyncCallback =
		[callback, &list = this->pluginInstanceList] (std::unique_ptr<juce::AudioPluginInstance> p, const juce::String& e) {
		if (p) {
			auto instance = p.release();
			list.add(instance);
			callback(instance);
			return;
		}

		/** Load Plugin Error */
		UNUSED(e);
		jassertfalse;
	};

	/** Create Instance */
	this->pluginFormatManager->createPluginInstanceAsync(pluginInfo, sampleRate, bufferSize, asyncCallback);
}

void PluginLoader::unloadPlugin(juce::AudioPluginInstance* pluginInstance) {
	this->pluginInstanceList.removeObject(pluginInstance, true);
}

PluginLoader* PluginLoader::getInstance() {
	return PluginLoader::instance ? PluginLoader::instance : (PluginLoader::instance = new PluginLoader());
}

PluginLoader* PluginLoader::instance = nullptr;

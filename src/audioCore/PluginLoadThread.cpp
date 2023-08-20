#include "PluginLoadThread.h"

PluginLoadThread::PluginLoadThread()
	: Thread("Plugin Load Thread") {
	this->pluginFormatManager = std::make_unique<juce::AudioPluginFormatManager>();
	this->pluginFormatManager->addDefaultFormats();
}

PluginLoadThread::~PluginLoadThread() {
	if (this->isThreadRunning()) {
		this->stopThread(30000);
	}
}

void PluginLoadThread::load(const juce::PluginDescription& pluginInfo,
	const PluginLoadCallback& callback, double sampleRate, int blockSize) {
	juce::GenericScopedLock locker(this->lock);

	this->list.push(std::make_tuple(pluginInfo, callback, sampleRate, blockSize));

	this->startThread();
}

void PluginLoadThread::run() {
	while (!this->threadShouldExit()) {
		/** Get Next Task */
		PluginLoadTask task;
		{
			juce::GenericScopedLock locker(this->lock);

			/** Check Empty */
			if (this->list.empty()) { break; }

			/** Dequque Task */
			task = this->list.front();
			this->list.pop();
		}

		/** Load Plugin */
		auto& [pluginDescription, callback, sampleRate, blockSize] = task;
		juce::String errorMessage;
		auto pluginInstance = this->pluginFormatManager->createPluginInstance(
			pluginDescription, sampleRate, blockSize, errorMessage);

		/** Error Message */
		if (!pluginInstance) {
			/** Handle Error */
			continue;
		}

		/** Invoke Callback */
		auto instanceHandler = [callback, instance =
			std::shared_ptr<juce::AudioPluginInstance>(pluginInstance.release())] () mutable {
			callback(std::unique_ptr<juce::AudioPluginInstance>(instance.get()));
			instance.reset();
		};
		juce::MessageManager::callAsync(instanceHandler);
	}
}

#include "PluginLoadThread.h"

#define PLUGIN_LOAD_ON_MESSAGE_THREAD 0

#if !PLUGIN_LOAD_ON_MESSAGE_THREAD

class PluginLoadMessageHelper;

class PluginLoadMessage final : public juce::Message {
public:
	PluginLoadMessage() = delete;
	explicit PluginLoadMessage(
		const std::function<void(std::unique_ptr<juce::AudioPluginInstance>,
			const juce::String&)>& callback,
		std::unique_ptr<juce::AudioPluginInstance> instance,
		const juce::String& errorMessage)
		: callback(callback), instance(std::move(instance)),
		errorMessage(errorMessage) {};

private:
	std::function<void(std::unique_ptr<juce::AudioPluginInstance>,
		const juce::String&)> callback;
	mutable std::unique_ptr<juce::AudioPluginInstance> instance = nullptr;
	juce::String errorMessage;

	friend class PluginLoadMessageHelper;
	void invoke() const {
		this->callback(std::move(this->instance), this->errorMessage);
	};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginLoadMessage)
};

class PluginLoadMessageHelper final : public juce::MessageListener {
public:
	PluginLoadMessageHelper() = default;

	void handleMessage(const juce::Message& message) override {
		if (auto m = dynamic_cast<const PluginLoadMessage*>(&message)) {
			m->invoke();
		}
	};

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginLoadMessageHelper)
};

#endif // !PLUGIN_LOAD_ON_MESSAGE_THREAD

PluginLoadThread::PluginLoadThread()
	: Thread("Plugin Load Thread") {
	this->pluginFormatManager = std::make_unique<juce::AudioPluginFormatManager>();
	this->pluginFormatManager->addDefaultFormats();

#if !PLUGIN_LOAD_ON_MESSAGE_THREAD
	this->messageHelper =
		std::unique_ptr<juce::MessageListener>(new PluginLoadMessageHelper);
#endif // !PLUGIN_LOAD_ON_MESSAGE_THREAD
}

PluginLoadThread::~PluginLoadThread() {
	if (this->isThreadRunning()) {
		this->stopThread(30000);
	}
}

void PluginLoadThread::load(const juce::PluginDescription& pluginInfo,
	DstPointer ptr, const Callback& callback, double sampleRate, int blockSize) {
	juce::GenericScopedLock locker(this->lock);

	this->list.push(std::make_tuple(pluginInfo, ptr, sampleRate, blockSize, callback));

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

		/** Prepare Plugin Load */
		auto& [pluginDescription, ptr, sampleRate, blockSize, callback] = task;
		auto asyncCallback =
			[ptr, pluginDescription, callback](std::unique_ptr<juce::AudioPluginInstance> p, const juce::String& /*e*/) {
			if (p) {
				auto identifier = pluginDescription.createIdentifierString();
				
				if (auto plugin = ptr.getPlugin()) {
					plugin->setPlugin(std::move(p), identifier);
					callback();
				}

				return;
			}

			/** Handle Error */
			jassertfalse;
		};

#if PLUGIN_LOAD_ON_MESSAGE_THREAD
		/** Load Async */
		this->pluginFormatManager->createPluginInstanceAsync(
			pluginDescription, sampleRate, blockSize, asyncCallback);

#else // PLUGIN_LOAD_ON_MESSAGE_THREAD
		/** Load Sync */
		juce::String errorMessage;
		auto pluginInstance = this->pluginFormatManager->createPluginInstance(
			pluginDescription, sampleRate, blockSize, errorMessage);

		/** Send Callback */
		this->messageHelper->postMessage(new PluginLoadMessage(
			asyncCallback, std::move(pluginInstance), errorMessage));
#endif // PLUGIN_LOAD_ON_MESSAGE_THREAD
	}
}

#include "PluginLoadThread.h"
#include "../uiCallback/UICallback.h"

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

class ARALoadMessage final : public juce::Message {
public:
	ARALoadMessage() = delete;
	explicit ARALoadMessage(
		const juce::PluginDescription& description,
		const juce::AudioPluginFormat::ARAFactoryCreationCallback& callback)
	: description(description), callback(callback) {
		this->pluginFormatManager = std::make_unique<juce::AudioPluginFormatManager>();
		this->pluginFormatManager->addDefaultFormats();
	};

private:
	const juce::PluginDescription description;
	const juce::AudioPluginFormat::ARAFactoryCreationCallback callback;

	std::unique_ptr<juce::AudioPluginFormatManager> pluginFormatManager;

	friend class PluginLoadMessageHelper;
	void invoke() const {
		this->pluginFormatManager->createARAFactoryAsync(
			this->description, this->callback);
	};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARALoadMessage)
};

class PluginLoadMessageHelper final : public juce::MessageListener {
public:
	PluginLoadMessageHelper() = default;

	void handleMessage(const juce::Message& message) override {
		if (auto m = dynamic_cast<const PluginLoadMessage*>(&message)) {
			m->invoke();
		}
		else if (auto m = dynamic_cast<const ARALoadMessage*>(&message)) {
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

	this->messageHelper =
		std::unique_ptr<juce::MessageListener>(new PluginLoadMessageHelper);
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

		/** Load Callback */
		auto asyncCallback =
			[ptr, pluginDescription, callback](std::unique_ptr<juce::AudioPluginInstance> p, const juce::String& errorMessage) {
			if (p) {
				auto identifier = pluginDescription.createIdentifierString();

				if (auto plugin = ptr.getPlugin()) {
					plugin->setPlugin(std::move(p), identifier,
						pluginDescription.hasARAExtension);
					callback();
				}

				return;
			}

			/** Handle Error */
			UICallbackAPI<const juce::String&, const juce::String&>::invoke(
				UICallbackType::ErrorAlert, "Load Plugin",
				"Can't load plugin with error message: " + errorMessage);
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
		this->messageHelper->postMessage(new PluginLoadMessage{
			asyncCallback, std::move(pluginInstance), errorMessage });
#endif // PLUGIN_LOAD_ON_MESSAGE_THREAD

		/** Load ARA */
		if (pluginDescription.hasARAExtension) {
			/** Load Callback */
			auto araLoadCallback = [ptr, pluginDescription, callback](juce::ARAFactoryResult result) {
				if (result.araFactory.get()) {
					auto identifier = pluginDescription.createIdentifierString();

					if (auto plugin = ptr.getPlugin()) {
						plugin->setARA(result.araFactory, identifier);
						callback();
					}

					return;
				}

				/** Handle Error */
				if (auto plugin = ptr.getPlugin()) {
					auto identifier = pluginDescription.createIdentifierString();
					plugin->handleARALoadError(identifier);
				}
				UICallbackAPI<const juce::String&, const juce::String&>::invoke(
					UICallbackType::ErrorAlert, "Load ARA Plugin",
					"Can't load ara plugin with error message: " + result.errorMessage);
				jassertfalse;
				};

			/** Load Async */
			this->messageHelper->postMessage(new ARALoadMessage{
				pluginDescription, araLoadCallback });
		}
	}
}

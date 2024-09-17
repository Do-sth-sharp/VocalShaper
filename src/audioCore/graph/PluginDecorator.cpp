#include "PluginDecorator.h"
#include "../plugin/Plugin.h"
#include "../plugin/PluginLoader.h"
#include "../uiCallback/UICallback.h"
#include "../misc/AudioLock.h"
#include "../misc/VMath.h"
#include "../misc/ARAController.h"
#include "../AudioCore.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

PluginDecorator::PluginDecorator(SeqSourceProcessor* seq,
	bool isInstr, const juce::AudioChannelSet& type)
	: seq(seq), isInstr(isInstr), audioChannels(type) {
	/** Channels */
	juce::AudioProcessorGraph::BusesLayout layout;
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(type.size()));
	layout.outputBuses.add(
		juce::AudioChannelSet::discreteChannels(type.size()));
	this->setBusesLayout(layout);

	/** MIDI CC */
	for (int i = 0; i < this->paramCCList.size(); i++) {
		this->paramCCList[i] = -1;
	}
}

PluginDecorator::PluginDecorator(std::unique_ptr<juce::AudioPluginInstance> plugin,
	const juce::String& identifier, SeqSourceProcessor* seq,
	bool isInstr, const juce::AudioChannelSet& type)
	: PluginDecorator(seq, isInstr, type) {
	this->setPlugin(std::move(plugin), identifier);
}

PluginDecorator::~PluginDecorator() {
	if (this->plugin) {
		if (auto editor = this->plugin->getActiveEditor()) {
			delete editor;
		}
	}
	this->araVirtualDocument = nullptr;
}

void PluginDecorator::setPlugin(
	std::unique_ptr<juce::AudioPluginInstance> plugin,
	const juce::String& pluginIdentifier, bool hasARA) {
	juce::ScopedWriteLock pluginLocker(audioLock::getPluginLock());

	if (!plugin) { return; }

	if (this->plugin) {
		if (auto editor = this->plugin->getActiveEditor()) {
			delete editor;
		}
	}
	this->araVirtualDocument = nullptr;

	this->plugin = std::move(plugin);
	this->pluginIdentifier = pluginIdentifier;

	this->updateBuffer();

	/** Load ARA */
	if (hasARA) {
		/** Load Callback */
		auto araLoadCallback = [ptr = SafePointer{ this }, pluginIdentifier](juce::ARAFactoryWrapper factory) {
			if (factory.get()) {
				if (ptr) {
					ptr->setARA(factory, pluginIdentifier);
				}

				return;
			}

			/** Handle Error */
			if (ptr) {
				ptr->handleARALoadError(pluginIdentifier);
			}
			UICallbackAPI<const juce::String&, const juce::String&>::invoke(
				UICallbackType::ErrorAlert, "Load ARA Plugin",
				"Can't load ara plugin: " + pluginIdentifier);
			jassertfalse;
			};

		/**
		 * Load Async.
		 * Load a plugin whose createARAFactory() returns a nullptr will cause a failure.
		 * It blames to the plugin and JUCE, not me.
		 * Never use an ARA plugin which returns a nullptr factory.
		 */
		juce::createARAFactoryAsync(*(this->plugin), araLoadCallback);
	}
	else {
		this->plugin->setPlayHead(this->getPlayHead());
		this->plugin->setNonRealtime(this->isNonRealtime());
		this->plugin->prepareToPlay(this->getSampleRate(), this->getBlockSize());
		this->pluginPrepared = true;

		//this->updatePluginBuses();

		/** Callback */
		if (this->isInstr) {
			UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);
		}
		else {
			UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, -1, -1);
		}
	}
}

void PluginDecorator::setARA(
	juce::ARAFactoryWrapper factory, const juce::String& pluginIdentifier) {
	if (this->plugin && pluginIdentifier == this->pluginIdentifier) {
		/** Create ARA Host Document Controller */
		if (auto controller = juce::ARAHostDocumentController::create(
			factory, juce::String{ "ARA Virtual Document for " } + this->plugin->getName(),
			std::make_unique<ARAAudioAccessController>(),
			std::make_unique<ARAArchivingController>(),
			std::make_unique<ARAContentAccessController>(),
			std::make_unique<ARAModelUpdateController>(),
			std::make_unique<ARAPlaybackController>())) {
			/** Bind To Plugin Instance */
			auto allRoles = ARA::kARAPlaybackRendererRole
				| ARA::kARAEditorRendererRole
				| ARA::kARAEditorViewRole;
			auto pluginInstance = controller->bindDocumentToPluginInstance(
				*(this->plugin), allRoles, allRoles);

			if (pluginInstance.isValid() && this->seq) {
				/** Set ARA Document Controller */
				this->araDocumentController = std::move(controller);

				/** Get Renderer Interfaces */
				this->araEditorRenderer = pluginInstance.getEditorRendererInterface();
				this->araPlaybackRenderer = pluginInstance.getPlaybackRendererInterface();

				/** Plugin On Off */
				auto pluginOnOffFunc = [plugin = this->plugin.get()](bool on) {
					if (plugin) {
						if (on) {
							plugin->prepareToPlay(plugin->getSampleRate(), plugin->getBlockSize());
						}
						else {
							plugin->releaseResources();
						}
					}
					};

				/** Create ARA Virtual Document */
				this->araVirtualDocument = std::make_unique<ARAVirtualDocument>(
					this->seq, this->araDocumentController->getDocumentController(),
					this->araEditorRenderer, this->araPlaybackRenderer, pluginOnOffFunc);
			}
		}

		/** Prepare Plugin */
		this->plugin->setPlayHead(this->getPlayHead());
		this->plugin->setNonRealtime(this->isNonRealtime());
		this->plugin->prepareToPlay(this->getSampleRate(), this->getBlockSize());
		this->pluginPrepared = true;

		//this->updatePluginBuses();

		/** Prepare ARA Document */
		if (this->araDocumentController) {
			this->araVirtualDocument->update();
		}

		/** Callback */
		if (this->isInstr) {
			UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);
		}
		else {
			UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, -1, -1);
		}
	}
}

void PluginDecorator::handleARALoadError(const juce::String& pluginIdentifier) {
	if (this->plugin && pluginIdentifier == this->pluginIdentifier) {
		/** Prepare Plugin */
		this->plugin->setPlayHead(this->getPlayHead());
		this->plugin->setNonRealtime(this->isNonRealtime());
		this->plugin->prepareToPlay(this->getSampleRate(), this->getBlockSize());
		this->pluginPrepared = true;

		//this->updatePluginBuses();

		/** Callback */
		if (this->isInstr) {
			UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);
		}
		else {
			UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, -1, -1);
		}
	}
}

const juce::AudioChannelSet& PluginDecorator::getAudioChannelSet() const {
	return this->audioChannels;
}

const juce::String PluginDecorator::getPluginIdentifier() const {
	return this->pluginIdentifier;
}

bool PluginDecorator::canPluginAddBus(bool isInput) const {
	if (!this->plugin) { return false; }
	return this->plugin->canAddBus(isInput);
}

bool PluginDecorator::canPluginRemoveBus(bool isInput) const {
	if (!this->plugin) { return false; }
	return this->plugin->canRemoveBus(isInput);
}

void PluginDecorator::setMIDIChannel(int channel) {
	if (channel > 16 || channel < 0) {
		channel = 0;
	}
	this->midiChannel = channel;

	/** Callback */
	if (this->isInstr) {
		UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);
	}
	else {
		UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, -1, -1);
	}
}

int PluginDecorator::getMIDIChannel() const {
	return this->midiChannel;
}

const juce::Array<juce::AudioProcessorParameter*>& PluginDecorator::getPluginParamList() const {
	if (!this->plugin) { static juce::Array<juce::AudioProcessorParameter*> empty; return empty; }
	return this->plugin->getParameters();
}

const juce::StringArray PluginDecorator::getParamNameList() const {
	auto& paramList = this->getPluginParamList();

	juce::StringArray result;
	for (auto i : paramList) {
		result.add(i->getName(INT_MAX));
	}

	return result;
}

const juce::String PluginDecorator::getParamName(int index) const {
	auto& paramList = this->getPluginParamList();
	if (index < 0 || index >= paramList.size()) { return juce::String{}; }

	auto param = paramList.getUnchecked(index);
	return param->getName(INT_MAX);
}

float PluginDecorator::getParamValue(int index) const {
	auto& paramList = this->getPluginParamList();
	if (index < 0 || index >= paramList.size()) { return 0; }

	auto param = paramList.getUnchecked(index);
	return param->getValue();
}

float PluginDecorator::getParamDefaultValue(int index) const {
	auto& paramList = this->getPluginParamList();
	if (index < 0 || index >= paramList.size()) { return 0; }

	auto param = paramList.getUnchecked(index);
	return param->getDefaultValue();
}

void PluginDecorator::setParamValue(int index, float value) {
	auto& paramList = this->getPluginParamList();
	if (index < 0 || index >= paramList.size()) { return; }

	auto param = paramList.getUnchecked(index);
	param->setValue(value);

	/** Callback */
	if (this->isInstr) {
		UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);
	}
	else {
		UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, -1, -1);
	}
}

void PluginDecorator::connectParamCC(int paramIndex, int CCIndex) {
	if (CCIndex < 0 || CCIndex >= this->paramCCList.size()) { return; }
	if (paramIndex < -1 || paramIndex >= this->getPluginParamList().size()) { return; }

	auto currentCC = this->getParamCCConnection(paramIndex);
	if (currentCC > -1) {
		this->paramCCList[currentCC] = -1;
	}

	this->paramCCList[CCIndex] = paramIndex;

	/** Callback */
	if (this->isInstr) {
		UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);
	}
	else {
		UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, -1, -1);
	}
}

int PluginDecorator::getCCParamConnection(int CCIndex) const {
	if (CCIndex < 0 || CCIndex >= this->paramCCList.size()) { return -1; }
	return this->paramCCList[CCIndex];
}

int PluginDecorator::getParamCCConnection(int paramIndex) const {
	auto it = std::find(this->paramCCList.begin(), this->paramCCList.end(), paramIndex);

	if (it != this->paramCCList.end()) {
		return it - this->paramCCList.begin();
	}

	return -1;
}

void PluginDecorator::removeCCParamConnection(int CCIndex) {
	if (CCIndex < 0 || CCIndex >= this->paramCCList.size()) { return; }
	this->paramCCList[CCIndex] = -1;

	/** Callback */
	if (this->isInstr) {
		UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);
	}
	else {
		UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, -1, -1);
	}
}

//void PluginDecorator::setParamCCListenning(int paramIndex) {
//	if (paramIndex < 0 || paramIndex >= this->getPluginParamList().size()) {
//		this->paramListenningCC = -1;
//		return;
//	}
//
//	auto currentCC = this->getParamCCConnection(paramIndex);
//	if (currentCC > -1) {
//		this->paramCCList[currentCC] = -1;
//	}
//
//	this->paramListenningCC = paramIndex;
//}

void PluginDecorator::setMIDICCIntercept(bool midiCCShouldIntercept) {
	this->midiCCShouldIntercept = midiCCShouldIntercept;

	/** Callback */
	if (this->isInstr) {
		UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);
	}
	else {
		UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, -1, -1);
	}
}

bool PluginDecorator::getMIDICCIntercept() const {
	return this->midiCCShouldIntercept;
}

void PluginDecorator::setMIDIOutput(bool midiShouldOutput) {
	this->midiShouldOutput = midiShouldOutput;

	/** Callback */
	if (this->isInstr) {
		UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);
	}
	else {
		UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, -1, -1);
	}
}

bool PluginDecorator::getMIDIOutput() const {
	return midiShouldOutput;
}

void PluginDecorator::setMIDICCListener(const MIDICCListener& listener) {
	juce::ScopedWriteLock locker(audioLock::getPluginLock());
	this->ccListener = listener;
}

void PluginDecorator::clearMIDICCListener() {
	juce::ScopedWriteLock locker(audioLock::getPluginLock());
	this->ccListener = MIDICCListener{};
}

const juce::String PluginDecorator::getName() const {
	if (!this->plugin) { return ""; }
	return this->plugin->getName() + ((bool)this->araDocumentController ? "(ARA)" : "");
}

juce::StringArray PluginDecorator::getAlternateDisplayNames() const {
	if (!this->plugin) { return {}; }
	return this->plugin->getAlternateDisplayNames();
}

void PluginDecorator::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	this->setRateAndBufferSizeDetails(sampleRate, maximumExpectedSamplesPerBlock);

	juce::ScopedWriteLock locker(audioLock::getPluginLock());
	if (!this->plugin) { return; }

	int channels = std::max(this->plugin->getTotalNumInputChannels(), this->plugin->getTotalNumOutputChannels());
	this->buffer = std::make_unique<juce::AudioBuffer<float>>(channels, this->getBlockSize());
	if (this->plugin->supportsDoublePrecisionProcessing()) {
		this->doubleBuffer = std::make_unique<juce::AudioBuffer<double>>(channels, this->getBlockSize());
	}

	this->plugin->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
	this->pluginPrepared = true;
}

void PluginDecorator::releaseResources() {
	if (!this->plugin) { return; }
	this->pluginPrepared = false;
	this->plugin->releaseResources();
}

void PluginDecorator::memoryWarningReceived() {
	if (!this->plugin) { return; }
	this->plugin->memoryWarningReceived();
}

void PluginDecorator::processBlock(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	PluginDecorator::filterMIDIMessage(this->midiChannel, midiMessages);
	this->parseMIDICC(midiMessages);
	PluginDecorator::interceptMIDICCMessage(this->midiCCShouldIntercept, midiMessages);

	{
		if (this->plugin && this->pluginPrepared && this->buffer) {
			this->buffer->clear();

			int totalChannels = std::min(buffer.getNumChannels(), this->buffer->getNumChannels());
			int totalSamples = std::min(buffer.getNumSamples(), this->buffer->getNumSamples());
			for (int i = 0; i < totalChannels; i++) {
				vMath::copyAudioData(
					*(this->buffer.get()), buffer,
					0, 0, i, i, totalSamples);
			}

			this->plugin->processBlock(*(this->buffer.get()), midiMessages);

			for (int i = 0; i < totalChannels; i++) {
				vMath::copyAudioData(
					buffer, *(this->buffer.get()),
					0, 0, i, i, totalSamples);
			}
		}
	}

	PluginDecorator::interceptMIDIMessage(this->midiShouldOutput, midiMessages);
}

void PluginDecorator::processBlock(
	juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) {
	PluginDecorator::filterMIDIMessage(this->midiChannel, midiMessages);
	this->parseMIDICC(midiMessages);
	PluginDecorator::interceptMIDICCMessage(this->midiCCShouldIntercept, midiMessages);

	{
		if (this->plugin && this->pluginPrepared && this->doubleBuffer) {
			this->doubleBuffer->clear();

			int totalChannels = std::min(buffer.getNumChannels(), this->buffer->getNumChannels());
			int totalSamples = std::min(buffer.getNumSamples(), this->buffer->getNumSamples());
			for (int i = 0; i < totalChannels; i++) {
				this->doubleBuffer->copyFrom(i, 0, buffer.getReadPointer(i), totalSamples);
			}

			this->plugin->processBlock(*(this->doubleBuffer.get()), midiMessages);

			for (int i = 0; i < totalChannels; i++) {
				buffer.copyFrom(i, 0, this->doubleBuffer->getReadPointer(i), totalSamples);
			}
		}
	}

	PluginDecorator::interceptMIDIMessage(this->midiShouldOutput, midiMessages);
}

void PluginDecorator::processBlockBypassed(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	if (this->plugin && this->pluginPrepared) {
		this->plugin->processBlockBypassed(buffer, midiMessages);
	}
}

void PluginDecorator::processBlockBypassed(
	juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) {
	if (this->plugin && this->pluginPrepared) {
		this->plugin->processBlockBypassed(buffer, midiMessages);
	}
}

bool PluginDecorator::canAddBus(bool isInput) const {
	return true;
}

bool PluginDecorator::canRemoveBus(bool isInput) const {
	return true;
}

bool PluginDecorator::supportsDoublePrecisionProcessing() const {
	if (!this->plugin) { return false; }
	return this->plugin->supportsDoublePrecisionProcessing();
}

double PluginDecorator::getTailLengthSeconds() const {
	if (!this->plugin) { return 0; }
	return this->plugin->getTailLengthSeconds();
}

bool PluginDecorator::acceptsMidi() const {
	if (!this->plugin) { return true; }
	return this->plugin->acceptsMidi();
}

bool PluginDecorator::producesMidi() const {
	if (!this->plugin) { return false; }
	return this->plugin->producesMidi();
}

bool PluginDecorator::supportsMPE() const {
	if (!this->plugin) { return false; }
	return this->plugin->supportsMPE();
}

bool PluginDecorator::isMidiEffect() const {
	if (!this->plugin) { return false; }
	return this->plugin->isMidiEffect();
}

void PluginDecorator::reset() {
	this->juce::AudioProcessor::reset();
	if (this->plugin) {
		this->plugin->reset();
	}
}

juce::AudioProcessorParameter* PluginDecorator::getBypassParameter() const {
	if (!this->plugin) { return nullptr; }
	return this->plugin->getBypassParameter();
}

void PluginDecorator::setNonRealtime(bool isNonRealtime) noexcept {
	this->juce::AudioProcessor::setNonRealtime(isNonRealtime);
	if (this->plugin) {
		this->plugin->setNonRealtime(isNonRealtime);
	}
}

juce::AudioProcessorEditor* PluginDecorator::createEditor() {
	if (!this->plugin) { return nullptr; }
	return this->plugin->createEditorIfNeeded();
}

bool PluginDecorator::hasEditor() const {
	if (!this->plugin) { return false; }
	return this->plugin->hasEditor();
}

void PluginDecorator::refreshParameterList() {
	this->juce::AudioProcessor::refreshParameterList();
	if (this->plugin) {
		this->plugin->refreshParameterList();
	}
}

int PluginDecorator::getNumPrograms() {
	if (!this->plugin) { return 0; }
	return this->plugin->getNumPrograms();
}

int PluginDecorator::getCurrentProgram() {
	if (!this->plugin) { return -1; }
	return this->plugin->getCurrentProgram();
}

void PluginDecorator::setCurrentProgram(int index) {
	if (!this->plugin) { return; }
	this->plugin->setCurrentProgram(index);
}

const juce::String PluginDecorator::getProgramName(int index) {
	if (!this->plugin) { return ""; }
	return this->plugin->getProgramName(index);
}

void PluginDecorator::changeProgramName(int index, const juce::String& newName) {
	if (!this->plugin) { return; }
	this->plugin->changeProgramName(index, newName);
}

void PluginDecorator::getStateInformation(juce::MemoryBlock& destData) {
	if (!this->plugin) { return; }
	this->plugin->getStateInformation(destData);
}

void PluginDecorator::setStateInformation(const void* data, int sizeInBytes) {
	if (!this->plugin) { return; }
	this->plugin->setStateInformation(data, sizeInBytes);
}

void PluginDecorator::setCurrentProgramStateInformation(const void* data, int sizeInBytes) {
	if (!this->plugin) { return; }
	this->plugin->setCurrentProgramStateInformation(data, sizeInBytes);
}

void PluginDecorator::processorLayoutsChanged() {
	this->updateBuffer();
}

void PluginDecorator::addListener(juce::AudioProcessorListener* newListener) {
	if (!this->plugin) { return; }
	this->plugin->addListener(newListener);
}

void PluginDecorator::removeListener(juce::AudioProcessorListener* listenerToRemove) {
	if (!this->plugin) { return; }
	this->plugin->removeListener(listenerToRemove);
}

void PluginDecorator::setPlayHead(juce::AudioPlayHead* newPlayHead) {
	this->juce::AudioProcessor::setPlayHead(newPlayHead);
	if (this->plugin) {
		this->plugin->setPlayHead(newPlayHead);
	}
}

juce::AudioProcessor::CurveData PluginDecorator::getResponseCurve(
	juce::AudioProcessor::CurveData::Type type) const {
	if (!this->plugin) { return juce::AudioProcessor::CurveData{}; }
	return this->plugin->getResponseCurve(type);
}

void PluginDecorator::updateTrackProperties(
	const juce::AudioProcessor::TrackProperties& properties) {
	if (!this->plugin) { return; }
	this->plugin->updateTrackProperties(properties);
}

bool PluginDecorator::parse(const google::protobuf::Message* data) {
	auto mes = dynamic_cast<const vsp4::Plugin*>(data);
	if (!mes) { return false; }

	auto& info = mes->info();
	if (this->audioChannels != utils::getChannelSet(
		static_cast<utils::TrackType>(info.decoratortype()))) {
		return false;
	}

	auto& state = mes->state();

	auto ptrPlugin = PluginDecorator::SafePointer(this);
	auto callback = [state, ptrPlugin] {
		if (!ptrPlugin) { return; }

		ptrPlugin->setMIDIChannel(state.midichannel());
		ptrPlugin->setMIDIOutput(state.midioutput());
		ptrPlugin->setMIDICCIntercept(state.midiintercept());
		
		auto& params = state.paramcclinks();
		for (auto& i : params) {
			ptrPlugin->connectParamCC(i.second, i.first);
		}

		auto& pluginData = state.data();
		ptrPlugin->setStateInformation(
			pluginData.c_str(), pluginData.size());
	};

	auto pluginDes = Plugin::getInstance()->findPlugin(info.id(), this->isInstr, info.addara());
	if (pluginDes) {
		PluginLoader::getInstance()->loadPlugin(
			*(pluginDes.get()), info.addara(), ptrPlugin, callback);
	}
	else {
		UICallbackAPI<const juce::String&, const juce::String&>::invoke(
			UICallbackType::ErrorAlert, "Load Plugin",
			"Can't find the plugin: " + info.id());
	}

	return true;
}

std::unique_ptr<google::protobuf::Message> PluginDecorator::serialize() const {
	auto mes = std::make_unique<vsp4::Plugin>();

	/** Plugin Info */
	auto info = mes->mutable_info();
	if (this->plugin) {
		info->set_id(this->getPluginIdentifier().toStdString());
		info->set_addara((bool)this->araDocumentController);
	}
	info->set_decoratortype(static_cast<vsp4::TrackType>(
		utils::getTrackType(this->getAudioChannelSet())));

	/** Plugin State */
	if (this->plugin) {
		auto state = mes->mutable_state();

		juce::MemoryBlock data;
		this->plugin->getStateInformation(data);
		state->set_data(data.getData(), data.getSize());

		state->set_midichannel(this->getMIDIChannel());
		state->set_midioutput(this->getMIDIOutput());
		state->set_midiintercept(this->getMIDICCIntercept());

		auto paramCCLinks = state->mutable_paramcclinks();
		for (int i = 0; i < this->paramCCList.size(); i++) {
			if (this->paramCCList[i] >= 0) {
				paramCCLinks->insert(google::protobuf::MapPair((uint32_t)i, (uint32_t)this->paramCCList[i]));
			}
		}
	}

	/** Plugin Bypassed */
	/** Use isBypassed method of AudioProcessorGraph::Node. */
	// if (auto bypass = this->getBypassParameter()) {
	// 	mes->set_bypassed(bypass->getValue());
	// }
	
	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

void PluginDecorator::numChannelsChanged() {
	//this->updatePluginBuses();
}

void PluginDecorator::numBusesChanged() {
	//this->updatePluginBuses();
}

void PluginDecorator::filterMIDIMessage(int channel, juce::MidiBuffer& midiMessages) {
	/** Filter MIDI Channel */
	if (channel >= 1 && channel <= 16) {
		juce::MidiBuffer bufferTemp;
		for (auto it = midiMessages.cbegin(); it != midiMessages.cend(); it++) {
			auto message = (*it).getMessage();
			if (message.getChannel() == channel) {
				bufferTemp.addEvent(message, (*it).samplePosition);
			}
		}
		midiMessages = bufferTemp;
	}
}

void PluginDecorator::interceptMIDIMessage(bool shouldMIDIOutput, juce::MidiBuffer& midiMessages) {
	if (!shouldMIDIOutput) {
		midiMessages = juce::MidiBuffer{};
	}
}

void PluginDecorator::interceptMIDICCMessage(bool shouldMIDICCIntercept, juce::MidiBuffer& midiMessages) {
	if (shouldMIDICCIntercept) {
		juce::MidiBuffer bufferTemp;
		for (auto i : midiMessages) {
			auto message = i.getMessage();
			if (!message.isController()) {
				bufferTemp.addEvent(message, i.samplePosition);
			}
		}
		midiMessages = bufferTemp;
	}
}

void PluginDecorator::parseMIDICC(juce::MidiBuffer& midiMessages) {
	/** Param Changed Temp */
	std::map<int, float> paramTemp;

	/** Parse Message */
	int lastCCChannel = -1;
	for (auto i : midiMessages) {
		/** Get Message */
		auto message = i.getMessage();
		if (!message.isController()) { continue; }

		/** Auto Link Param */
		lastCCChannel = message.getControllerNumber();

		/** Get Param Changed */
		int paramIndex = this->paramCCList[message.getControllerNumber()];
		if (paramIndex > -1) {
			paramTemp[paramIndex] = message.getControllerValue() / 127.f;
		}
	}

	/** Set Param Value */
	if (!this->plugin) { return; }
	auto& paramList = this->plugin->getParameters();
	for (auto& i : paramTemp) {
		auto param = paramList[i.first];
		if (param) {
			param->beginChangeGesture();
			param->setValueNotifyingHost(i.second);
			param->endChangeGesture();
		}
	}

	/** Send Auto Connect */
	if ((lastCCChannel > -1) && this->ccListener) {
		juce::MessageManager::callAsync(
			std::bind(this->ccListener, lastCCChannel));
	}
}

void PluginDecorator::updateBuffer() {
	juce::ScopedWriteLock locker(audioLock::getPluginLock());
	if (this->plugin) {
		int channels = std::max(this->plugin->getTotalNumInputChannels(), this->plugin->getTotalNumOutputChannels());
		this->buffer = std::make_unique<juce::AudioBuffer<float>>(channels, this->getBlockSize());
		if (this->plugin->supportsDoublePrecisionProcessing()) {
			this->doubleBuffer = std::make_unique<juce::AudioBuffer<double>>(channels, this->getBlockSize());
		}
	}
}

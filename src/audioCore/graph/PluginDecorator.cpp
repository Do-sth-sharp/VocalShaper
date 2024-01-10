#include "PluginDecorator.h"
#include "../plugin/Plugin.h"
#include "../plugin/PluginLoader.h"
#include "../uiCallback/UICallback.h"
#include "../misc/AudioLock.h"
#include "../AudioCore.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

PluginDecorator::PluginDecorator(bool isInstr,
	const juce::AudioChannelSet& type)
	: isInstr(isInstr), audioChannels(type) {
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
	const juce::String& identifier,
	bool isInstr, const juce::AudioChannelSet& type)
	: PluginDecorator(isInstr, type) {
	this->setPlugin(std::move(plugin), identifier);
}

void PluginDecorator::setPlugin(
	std::unique_ptr<juce::AudioPluginInstance> plugin, const juce::String& pluginIdentifier) {
	if (!plugin) { return; }

	{
		juce::ScopedWriteLock locker(audioLock::getLock());

		this->plugin = std::move(plugin);
		this->pluginIdentifier = pluginIdentifier;

		int channels = std::max(this->plugin->getTotalNumInputChannels(), this->plugin->getTotalNumOutputChannels());
		this->buffer = std::make_unique<juce::AudioBuffer<float>>(channels, this->getBlockSize());
		if (this->plugin->supportsDoublePrecisionProcessing()) {
			this->doubleBuffer = std::make_unique<juce::AudioBuffer<double>>(channels, this->getBlockSize());
		}
	}

	this->plugin->setPlayHead(this->getPlayHead());
	this->plugin->setNonRealtime(this->isNonRealtime());
	this->plugin->prepareToPlay(this->getSampleRate(), this->getBlockSize());

	//this->updatePluginBuses();
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
}

int PluginDecorator::getMIDIChannel() const {
	return this->midiChannel;
}

const juce::Array<juce::AudioProcessorParameter*>& PluginDecorator::getPluginParamList() const {
	if (!this->plugin) { static juce::Array<juce::AudioProcessorParameter*> empty; return empty; }
	return this->plugin->getParameters();
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
}

void PluginDecorator::connectParamCC(int paramIndex, int CCIndex) {
	if (CCIndex < 0 || CCIndex >= this->paramCCList.size()) { return; }
	if (paramIndex < -1 || paramIndex >= this->getPluginParamList().size()) { return; }

	auto currentCC = this->getParamCCConnection(paramIndex);
	if (currentCC > -1) {
		this->paramCCList[currentCC] = -1;
	}

	this->paramCCList[CCIndex] = paramIndex;
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
}

bool PluginDecorator::getMIDICCIntercept() const {
	return this->midiCCShouldIntercept;
}

void PluginDecorator::setMIDIOutput(bool midiShouldOutput) {
	this->midiShouldOutput = midiShouldOutput;
}

bool PluginDecorator::getMIDIOutput() const {
	return midiShouldOutput;
}

const juce::String PluginDecorator::getName() const {
	if (!this->plugin) { return ""; }
	return this->plugin->getName();
}

juce::StringArray PluginDecorator::getAlternateDisplayNames() const {
	if (!this->plugin) { return {}; }
	return this->plugin->getAlternateDisplayNames();
}

void PluginDecorator::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	this->setRateAndBufferSizeDetails(sampleRate, maximumExpectedSamplesPerBlock);

	juce::ScopedWriteLock locker(audioLock::getLock());
	if (!this->plugin) { return; }

	int channels = std::max(this->plugin->getTotalNumInputChannels(), this->plugin->getTotalNumOutputChannels());
	this->buffer = std::make_unique<juce::AudioBuffer<float>>(channels, this->getBlockSize());
	if (this->plugin->supportsDoublePrecisionProcessing()) {
		this->doubleBuffer = std::make_unique<juce::AudioBuffer<double>>(channels, this->getBlockSize());
	}

	this->plugin->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
}

void PluginDecorator::releaseResources() {
	if (!this->plugin) { return; }
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
		if (this->plugin && this->buffer) {
			this->buffer->clear();
			this->buffer->setSize(this->buffer->getNumChannels(), buffer.getNumSamples(), true, false, true);

			for (int i = 0; i < buffer.getNumChannels() && i < this->buffer->getNumChannels(); i++) {
				this->buffer->copyFrom(i, 0, buffer.getReadPointer(i), buffer.getNumSamples());
			}

			this->plugin->processBlock(*(this->buffer.get()), midiMessages);

			for (int i = 0; i < buffer.getNumChannels() && i < this->buffer->getNumChannels(); i++) {
				buffer.copyFrom(i, 0, this->buffer->getReadPointer(i), buffer.getNumSamples());
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
		if (this->plugin && this->doubleBuffer) {
			this->doubleBuffer->clear();
			this->doubleBuffer->setSize(this->doubleBuffer->getNumChannels(), buffer.getNumSamples(), true, false, true);

			for (int i = 0; i < buffer.getNumChannels() && i < this->doubleBuffer->getNumChannels(); i++) {
				this->doubleBuffer->copyFrom(i, 0, buffer.getReadPointer(i), buffer.getNumSamples());
			}

			this->plugin->processBlock(*(this->doubleBuffer.get()), midiMessages);

			for (int i = 0; i < buffer.getNumChannels() && i < this->doubleBuffer->getNumChannels(); i++) {
				buffer.copyFrom(i, 0, this->doubleBuffer->getReadPointer(i), buffer.getNumSamples());
			}
		}
	}

	PluginDecorator::interceptMIDIMessage(this->midiShouldOutput, midiMessages);
}

void PluginDecorator::processBlockBypassed(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	if (this->plugin) {
		this->plugin->processBlockBypassed(buffer, midiMessages);
	}
}

void PluginDecorator::processBlockBypassed(
	juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) {
	if (this->plugin) {
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
	this->updatePluginBuses();
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

	auto pluginDes = Plugin::getInstance()->findPlugin(info.id(), this->isInstr);
	if (pluginDes) {
		PluginLoader::getInstance()->loadPlugin(
			*(pluginDes.get()), ptrPlugin, callback);
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
		info->set_id(this->pluginIdentifier.toStdString());
	}
	info->set_decoratortype(static_cast<vsp4::TrackType>(
		utils::getTrackType(this->audioChannels)));

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

void PluginDecorator::updatePluginBuses() {
	if (!this->plugin) { return; }

	auto currentBusesLayout = this->getBusesLayout();
	auto pluginBusesLayout = this->plugin->getBusesLayout();

	if (pluginBusesLayout == currentBusesLayout) {
		return;
	}

	int inputBusNumDefer = 
		currentBusesLayout.inputBuses.size() - pluginBusesLayout.inputBuses.size();
	int outputBusNumDefer =
		currentBusesLayout.outputBuses.size() - pluginBusesLayout.outputBuses.size();

	if (inputBusNumDefer > 0) {
		for (int i = 0; i < inputBusNumDefer; i++) {
			this->plugin->addBus(true);
		}
	}
	else if (inputBusNumDefer < 0) {
		for (int i = 0; i < -inputBusNumDefer; i++) {
			this->plugin->removeBus(true);
		}
	}
	if (outputBusNumDefer > 0) {
		for (int i = 0; i < outputBusNumDefer; i++) {
			this->plugin->addBus(false);
		}
	}
	else if (outputBusNumDefer < 0) {
		for (int i = 0; i < -outputBusNumDefer; i++) {
			this->plugin->removeBus(false);
		}
	}

	this->plugin->setBusesLayout(currentBusesLayout);
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
	for (auto i : midiMessages) {
		/** Get Message */
		auto message = i.getMessage();
		if (!message.isController()) { continue; }

		/** Auto Link Param */
		/*int paramListenningCC = this->paramListenningCC;
		this->paramListenningCC = -1;
		if (paramListenningCC > -1) {
			this->paramCCList[message.getControllerNumber()] = paramListenningCC;
		}*/

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
}

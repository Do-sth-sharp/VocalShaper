#include "PluginDecorator.h"

PluginDecorator::PluginDecorator(std::unique_ptr<juce::AudioPluginInstance> plugin)
	: plugin(std::move(plugin)) {
	jassert(this->plugin);

	for (int i = 0; i < this->paramCCList.size(); i++) {
		this->paramCCList[i] = -1;
	}

	this->initFlag = true;

	this->syncBusesFromPlugin();

	this->initFlag = false;
}

bool PluginDecorator::canPluginAddBus(bool isInput) const {
	return this->plugin->canAddBus(isInput);
}

bool PluginDecorator::canPluginRemoveBus(bool isInput) const {
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
	if (paramIndex < 0 || paramIndex >= this->getPluginParamList().size()) { return; }

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

void PluginDecorator::setParamCCListenning(int paramIndex) {
	if (paramIndex < 0 || paramIndex >= this->getPluginParamList().size()) {
		this->paramListenningCC = -1;
		return;
	}

	auto currentCC = this->getParamCCConnection(paramIndex);
	if (currentCC > -1) {
		this->paramCCList[currentCC] = -1;
	}

	this->paramListenningCC = paramIndex;
}

void PluginDecorator::setMIDICCIntercept(bool midiCCShouldIntercept) {
	this->midiCCShouldIntercept = midiCCShouldIntercept;
}

bool PluginDecorator::getMIDICCIntercept() const {
	return this->midiCCShouldIntercept;
}

const juce::String PluginDecorator::getName() const {
	return this->plugin->getName();
}

juce::StringArray PluginDecorator::getAlternateDisplayNames() const {
	return this->plugin->getAlternateDisplayNames();
}

void PluginDecorator::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	this->plugin->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
}

void PluginDecorator::releaseResources() {
	this->plugin->releaseResources();
}

void PluginDecorator::memoryWarningReceived() {
	this->plugin->memoryWarningReceived();
}

void PluginDecorator::processBlock(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	PluginDecorator::filterMIDIMessage(this->midiChannel, midiMessages);
	this->parseMIDICC(midiMessages);
	PluginDecorator::interceptMIDICCMessage(this->midiCCShouldIntercept, midiMessages);

	this->plugin->processBlock(buffer, midiMessages);

	PluginDecorator::interceptMIDIMessage(this->midiShouldOutput, midiMessages);
}

void PluginDecorator::processBlock(
	juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) {
	PluginDecorator::filterMIDIMessage(this->midiChannel, midiMessages);
	this->parseMIDICC(midiMessages);
	PluginDecorator::interceptMIDICCMessage(this->midiCCShouldIntercept, midiMessages);

	this->plugin->processBlock(buffer, midiMessages);

	PluginDecorator::interceptMIDIMessage(this->midiShouldOutput, midiMessages);
}

void PluginDecorator::processBlockBypassed(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	this->plugin->processBlockBypassed(buffer, midiMessages);
}

void PluginDecorator::processBlockBypassed(
	juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) {
	this->plugin->processBlockBypassed(buffer, midiMessages);
}

bool PluginDecorator::canAddBus(bool isInput) const {
	return true;
}

bool PluginDecorator::canRemoveBus(bool isInput) const {
	return true;
}

bool PluginDecorator::supportsDoublePrecisionProcessing() const {
	return this->plugin->supportsDoublePrecisionProcessing();
}

double PluginDecorator::getTailLengthSeconds() const {
	return this->plugin->getTailLengthSeconds();
}

bool PluginDecorator::acceptsMidi() const {
	return this->plugin->acceptsMidi();
}

bool PluginDecorator::producesMidi() const {
	return this->plugin->producesMidi();
}

bool PluginDecorator::supportsMPE() const {
	return this->plugin->supportsMPE();
}

bool PluginDecorator::isMidiEffect() const {
	return this->plugin->isMidiEffect();
}

void PluginDecorator::reset() {
	this->juce::AudioProcessor::reset();
	this->plugin->reset();
}

juce::AudioProcessorParameter* PluginDecorator::getBypassParameter() const {
	return this->plugin->getBypassParameter();
}

void PluginDecorator::setNonRealtime(bool isNonRealtime) noexcept {
	this->juce::AudioProcessor::setNonRealtime(isNonRealtime);
	this->plugin->setNonRealtime(isNonRealtime);
}

juce::AudioProcessorEditor* PluginDecorator::createEditor() {
	return this->plugin->createEditorIfNeeded();
}

bool PluginDecorator::hasEditor() const {
	return this->plugin->hasEditor();
}

void PluginDecorator::refreshParameterList() {
	this->juce::AudioProcessor::refreshParameterList();
	this->plugin->refreshParameterList();
}

int PluginDecorator::getNumPrograms() {
	return this->plugin->getNumPrograms();
}

int PluginDecorator::getCurrentProgram() {
	return this->plugin->getCurrentProgram();
}

void PluginDecorator::setCurrentProgram(int index) {
	this->plugin->setCurrentProgram(index);
}

const juce::String PluginDecorator::getProgramName(int index) {
	return this->plugin->getProgramName(index);
}

void PluginDecorator::changeProgramName(int index, const juce::String& newName) {
	this->plugin->changeProgramName(index, newName);
}

void PluginDecorator::getStateInformation(juce::MemoryBlock& destData) {
	this->plugin->getStateInformation(destData);
}

void PluginDecorator::setStateInformation(const void* data, int sizeInBytes) {
	this->plugin->setStateInformation(data, sizeInBytes);
}

void PluginDecorator::setCurrentProgramStateInformation(const void* data, int sizeInBytes) {
	this->plugin->setCurrentProgramStateInformation(data, sizeInBytes);
}

void PluginDecorator::processorLayoutsChanged() {
	this->updatePluginBuses();
}

void PluginDecorator::addListener(juce::AudioProcessorListener* newListener) {
	this->plugin->addListener(newListener);
}

void PluginDecorator::removeListener(juce::AudioProcessorListener* listenerToRemove) {
	this->plugin->removeListener(listenerToRemove);
}

void PluginDecorator::setPlayHead(juce::AudioPlayHead* newPlayHead) {
	this->juce::AudioProcessor::setPlayHead(newPlayHead);
	this->plugin->setPlayHead(newPlayHead);
}

juce::int32 PluginDecorator::getAAXPluginIDForMainBusConfig(
	const juce::AudioChannelSet& mainInputLayout,
	const juce::AudioChannelSet& mainOutputLayout,
	bool idForAudioSuite) const {
	return this->plugin->getAAXPluginIDForMainBusConfig(
		mainInputLayout, mainOutputLayout, idForAudioSuite);
}

juce::AudioProcessor::CurveData PluginDecorator::getResponseCurve(
	juce::AudioProcessor::CurveData::Type type) const {
	return this->plugin->getResponseCurve(type);
}

void PluginDecorator::updateTrackProperties(
	const juce::AudioProcessor::TrackProperties& properties) {
	this->plugin->updateTrackProperties(properties);
}

void PluginDecorator::numChannelsChanged() {
	//this->updatePluginBuses();
}

void PluginDecorator::numBusesChanged() {
	this->syncBusesNumFromPlugin();
}

void PluginDecorator::updatePluginBuses() {
	if (this->initFlag) { return; }

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

void PluginDecorator::syncBusesFromPlugin() {
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
			this->removeBus(true);
		}
	}
	else if (inputBusNumDefer < 0) {
		for (int i = 0; i < -inputBusNumDefer; i++) {
			this->addBus(true);
		}
	}
	if (outputBusNumDefer > 0) {
		for (int i = 0; i < outputBusNumDefer; i++) {
			this->removeBus(false);
		}
	}
	else if (outputBusNumDefer < 0) {
		for (int i = 0; i < -outputBusNumDefer; i++) {
			this->addBus(false);
		}
	}

	this->setBusesLayout(pluginBusesLayout);
}

void PluginDecorator::syncBusesNumFromPlugin() {
	if (this->initFlag) { return; }

	auto currentBusesLayout = this->getBusesLayout();
	auto pluginBusesLayout = this->plugin->getBusesLayout();

	if (pluginBusesLayout == currentBusesLayout) {
		return;
	}

	int inputBusNumDefer =
		currentBusesLayout.inputBuses.size() - pluginBusesLayout.inputBuses.size();
	int outputBusNumDefer =
		currentBusesLayout.outputBuses.size() - pluginBusesLayout.outputBuses.size();

	bool flagChanged = false;

	if ((inputBusNumDefer > 0 && (!this->canPluginAddBus(true)))
		|| (inputBusNumDefer < 0 && (!this->canPluginRemoveBus(true)))) {
		currentBusesLayout.inputBuses.resize(pluginBusesLayout.inputBuses.size());
		flagChanged = true;
	}

	if ((outputBusNumDefer > 0 && (!this->canPluginAddBus(false)))
		|| (outputBusNumDefer < 0 && (!this->canPluginRemoveBus(false)))) {
		currentBusesLayout.outputBuses.resize(pluginBusesLayout.outputBuses.size());
		flagChanged = true;
	}

	if (flagChanged) {
		this->setBusesLayout(currentBusesLayout);
	}
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
	for (auto i : midiMessages) {
		/** Get Message */
		auto message = i.getMessage();
		if (!message.isController()) { continue; }

		/** Auto Link Param */
		int paramListenningCC = this->paramListenningCC;
		this->paramListenningCC = -1;
		if (paramListenningCC > -1) {
			this->paramCCList[message.getControllerNumber()] = paramListenningCC;
		}

		/** Set Param Value */
		int paramIndex = this->paramCCList[message.getControllerNumber()];
		if (paramIndex > -1) {
			auto param = this->plugin->getParameters()[paramIndex];
			if (param) {
				param->beginChangeGesture();

				param->setValueNotifyingHost(message.getControllerValue() / 127.f);

				param->endChangeGesture();
			}
		}
	}
}

#include "PluginDecorator.h"

PluginDecorator::PluginDecorator(std::unique_ptr<juce::AudioPluginInstance> plugin)
	: plugin(std::move(plugin)) {
	jassert(this->plugin);

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
	this->plugin->processBlock(buffer, midiMessages);
}

void PluginDecorator::processBlock(
	juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) {
	PluginDecorator::filterMIDIMessage(this->midiChannel, midiMessages);
	this->plugin->processBlock(buffer, midiMessages);
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

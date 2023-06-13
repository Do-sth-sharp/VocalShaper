#include "MainGraph.h"

#include "PlayPosition.h"
#include "CloneableSourceManager.h"

MainGraph::MainGraph() {
	/** The Main Audio IO Node */
	this->audioInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
	this->audioOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

	/** The Main MIDI Input Node */
	this->midiInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));
}

MainGraph::~MainGraph() {
	for (auto& i : this->instrumentNodeList) {
		if (auto processor = i->getProcessor()) {
			if (auto editor = processor->getActiveEditor()) {
				delete editor;
			}
		}
	}
}

void MainGraph::setAudioLayout(int inputChannelNum, int outputChannelNum) {
	/** Create Buses Layout */
	juce::AudioProcessorGraph::BusesLayout busLayout;
	busLayout.inputBuses.add(juce::AudioChannelSet::discreteChannels(inputChannelNum));
	busLayout.outputBuses.add(juce::AudioChannelSet::discreteChannels(outputChannelNum));

	/** Set Layout Of Main Graph */
	this->setBusesLayout(busLayout);

	/** Set Layout Of Input Node */
	juce::AudioProcessorGraph::BusesLayout inputLayout = busLayout;
	inputLayout.outputBuses = inputLayout.inputBuses;
	this->audioInputNode->getProcessor()->setBusesLayout(inputLayout);

	/** Set Layout Of Output Node */
	juce::AudioProcessorGraph::BusesLayout outputLayout = busLayout;
	outputLayout.inputBuses = outputLayout.outputBuses;
	this->audioOutputNode->getProcessor()->setBusesLayout(outputLayout);

	/** Auto Remove Connections */
	this->removeIllegalAudioI2SrcConnections();
	this->removeIllegalAudioI2TrkConnections();
	this->removeIllegalAudioTrk2OConnections();
}

void MainGraph::setMIDIMessageHook(const std::function<void(const juce::MidiMessage&)> hook) {
	juce::ScopedWriteLock locker(this->hookLock);
	this->midiHook = hook;
}

void MainGraph::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) {
	/** Play Head */
	if (auto position = PlayPosition::getInstance()) {
		position->setSampleRate(sampleRate);
	}

	/** Source */
	CloneableSourceManager::getInstance()->setSampleRate(sampleRate);

	/** Current Graph */
	this->juce::AudioProcessorGraph::prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
}

void MainGraph::setPlayHead(juce::AudioPlayHead* newPlayHead) {
	this->juce::AudioProcessorGraph::setPlayHead(newPlayHead);

	/** Audio Source */
	for (auto& i : this->audioSourceNodeList) {
		auto src = i->getProcessor();
		src->setPlayHead(PlayPosition::getInstance());
	}

	/** Instrument */
	for (auto& i : this->instrumentNodeList) {
		auto instr = i->getProcessor();
		instr->setPlayHead(PlayPosition::getInstance());
	}

	/** Track */
	for (auto& i : this->trackNodeList) {
		auto track = i->getProcessor();
		track->setPlayHead(PlayPosition::getInstance());
	}
}

void MainGraph::processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) {
	/** Call MIDI Hook */
	{
		juce::ScopedReadLock locker(this->hookLock);
		if (this->midiHook) {
			for (auto m : midi) {
				juce::MessageManager::callAsync(
					[mes = m.getMessage(), hook = this->midiHook] {
						hook(mes);
					});
			}
		}
	}

	/** Process Audio Block */
	this->juce::AudioProcessorGraph::processBlock(audio, midi);

	/** Add Position */
	PlayPosition::getInstance()->next(audio.getNumSamples());
}

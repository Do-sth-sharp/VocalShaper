#include "MainGraph.h"

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

	/** Add Sequencer And Mixer To Audio Graph */
	this->sequencer = this->addNode(std::make_unique<Sequencer>());
	this->mixer = this->addNode(std::make_unique<Mixer>());

	/** Add MIDI Connection */
	this->addConnection(
		{ {this->midiInputNode->nodeID, this->midiChannelIndex},
		{this->sequencer->nodeID, this->midiChannelIndex} });
	this->addConnection(
		{ {this->midiInputNode->nodeID, this->midiChannelIndex},
		{this->mixer->nodeID, this->midiChannelIndex} });
	this->addConnection(
		{ {this->sequencer->nodeID, this->midiChannelIndex},
		{this->mixer->nodeID, this->midiChannelIndex} });

	/** Create Default Tracks And Channels */
	this->addSequencerOutputBus();
	this->getMixer()->insertTrack();

	/**
	 * TODO Fix Bugs
	 * The sequencer only has input channels and without output channels.
	 * Then the audio data will be cleared after process the block in sequencer.
	 * I don't find the reason now, so I bypassed it.
	 */
	this->sequencer->setBypassed(true);
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

	/** Set Layout Of Sequencer And Mixer */
	auto sequencer = this->getSequencer();
	if (sequencer) {
		sequencer->setInputChannels(busLayout.inputBuses);
	}
	auto mixer = this->getMixer();
	if (mixer) {
		mixer->setInputDeviceChannels(busLayout.inputBuses);
		mixer->setOutputChannels(busLayout.outputBuses);
	}

	/** Link Audio Channels Of Sequencer And Mixer */
	this->removeIllegalConnections();

	int mixerSeqChannelNum = 0;
	if (mixer && sequencer) {
		mixerSeqChannelNum = mixer->getSequencerChannelNum();
		jassert(mixerSeqChannelNum == sequencer->getTotalNumOutputChannels());
	}
	
	for (int i = 0; i < inputChannelNum; i++) {
		this->addConnection(
			{ {this->audioInputNode->nodeID, i},
			{this->sequencer->nodeID, i} });
		this->addConnection(
			{ {this->audioInputNode->nodeID, i},
			{this->mixer->nodeID, mixerSeqChannelNum + i} });
	}
	for (int i = 0; i < outputChannelNum; i++) {
		this->addConnection(
			{ {this->mixer->nodeID, i},
			{this->audioOutputNode->nodeID, i} });
	}
}

void MainGraph::setMIDIMessageHook(const std::function<void(const juce::MidiMessage&)> hook) {
	juce::ScopedWriteLock locker(this->hookLock);
	this->midiHook = hook;
}

Mixer* MainGraph::getMixer() const {
	return dynamic_cast<Mixer*>(this->mixer->getProcessor());
}

Sequencer* MainGraph::getSequencer() const {
	return dynamic_cast<Sequencer*>(this->sequencer->getProcessor());
}

void MainGraph::addSequencerOutputBus(const juce::AudioChannelSet& type) {
	/** Get Sequencer And Mixer */
	auto mixer = this->getMixer();
	auto sequencer = this->getSequencer();

	if (mixer && sequencer) {
		/** Sequencer Channel Num Should Match */
		int seqChannelNum = sequencer->getTotalNumOutputChannels();
		jassert(seqChannelNum == mixer->getSequencerChannelNum());

		/** Add Bus */
		sequencer->addOutputBus(type);
		mixer->addSequencerBus(type);

		/** Connect Channel */
		int newChannelNum = type.size();
		for (int i = 0; i < newChannelNum; i++) {
			this->addConnection({ {this->sequencer->nodeID, seqChannelNum + i},
				{this->mixer->nodeID, seqChannelNum + i} });
		}
	}
}

void MainGraph::removeSequencerOutputBus() {
	/** Get Sequencer And Mixer */
	auto mixer = this->getMixer();
	auto sequencer = this->getSequencer();

	if (mixer && sequencer) {
		/** Sequencer Channel Num Should Match */
		int seqChannelNum = sequencer->getTotalNumOutputChannels();
		jassert(seqChannelNum == mixer->getSequencerChannelNum());

		/** Sequencer Bus Num */
		int seqBusNum = mixer->getSequencerBusNum();
		if (seqBusNum > 1) {
			/** Remove Bus */
			sequencer->removeOutputBus();
			mixer->removeSequencerBus();

			/** Disconnect Channel */
			this->removeIllegalConnections();
		}
	}
}

void MainGraph::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) {
	/** Current Graph */
	this->juce::AudioProcessorGraph::prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);

	/** Mixer */
	auto mixer = this->getMixer();
	if (mixer) {
		mixer->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
	}

	/** Sequencer */
	auto sequencer = this->getSequencer();
	if (sequencer) {
		sequencer->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
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
}

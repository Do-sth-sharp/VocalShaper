#include "MainGraph.h"

#include "../misc/PlayPosition.h"
#include "../misc/Renderer.h"
#include "../source/CloneableSourceManager.h"
#include "../AudioCore.h"
#include "SourceRecordProcessor.h"

MainGraph::MainGraph() {
	/** The Main Audio IO Node */
	this->audioInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
	this->audioOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

	/** The Main MIDI IO Node */
	this->midiInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));
	this->midiOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode));

	/** The Source Recorder Node */
	this->recorderNode = this->addNode(std::make_unique<SourceRecordProcessor>());

	/** Link MIDI Input To Recorder */
	this->addConnection({ {this->midiInputNode->nodeID, this->midiChannelIndex},
		{this->recorderNode->nodeID, this->midiChannelIndex} });
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
	this->removeIllegalAudioI2TrkConnections();
	this->removeIllegalAudioTrk2OConnections();

	/** Remove Recorder Input */
	{
		int inputChannels =
			this->recorderNode->getProcessor()->getTotalNumInputChannels();
		for (int i = 0; i < inputChannels; i++) {
			this->removeConnection(
				{ {this->audioInputNode->nodeID, i},
				{this->recorderNode->nodeID, i} });
		}
	}

	/** Set Layout of Recorder */
	this->recorderNode->getProcessor()->setBusesLayout(inputLayout);

	/** Add Recorder Input */
	{
		for (int i = 0; i < inputChannelNum; i++) {
			this->addConnection(
				{ {this->audioInputNode->nodeID, i},
				{this->recorderNode->nodeID, i} });
		}
	}
}

void MainGraph::setMIDIMessageHook(
	const std::function<void(const juce::MidiMessage&, bool)> hook) {
	juce::ScopedWriteLock locker(this->hookLock);
	this->midiHook = hook;
}

void MainGraph::setMIDIOutput(juce::MidiOutput* output) {
	juce::ScopedWriteLock locker(this->midiLock);
	this->midiOutput = output;
}

void MainGraph::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) {
	/** Play Head */
	if (auto position = dynamic_cast<PlayPosition*>(this->getPlayHead())) {
		position->setSampleRate(sampleRate);
	}

	/** Source */
	CloneableSourceManager::getInstance()->prepareToPlay(
		sampleRate, maximumExpectedSamplesPerBlock);

	/** Renderer */
	Renderer::getInstance()->updateSampleRateAndBufferSize(
		sampleRate, maximumExpectedSamplesPerBlock);

	/** Current Graph */
	this->juce::AudioProcessorGraph::prepareToPlay(
		sampleRate, maximumExpectedSamplesPerBlock);
}

void MainGraph::setPlayHead(juce::AudioPlayHead* newPlayHead) {
	this->juce::AudioProcessorGraph::setPlayHead(newPlayHead);

	/** Recorder */
	this->recorderNode->getProcessor()->setPlayHead(newPlayHead);

	/** Audio Source */
	for (auto& i : this->audioSourceNodeList) {
		auto src = i->getProcessor();
		src->setPlayHead(newPlayHead);
	}

	/** Instrument */
	for (auto& i : this->instrumentNodeList) {
		auto instr = i->getProcessor();
		instr->setPlayHead(newPlayHead);
	}

	/** Track */
	for (auto& i : this->trackNodeList) {
		auto track = i->getProcessor();
		track->setPlayHead(newPlayHead);
	}
}

double MainGraph::getTailLengthSeconds() const {
	double result = 0;
	for (auto& t : this->audioSourceNodeList) {
		result = std::max(t->getProcessor()->getTailLengthSeconds(), result);
	}
	return result;
}

SourceRecordProcessor* MainGraph::getRecorder() const {
	return dynamic_cast<SourceRecordProcessor*>(this->recorderNode->getProcessor());
}

void MainGraph::processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) {
	/** Render State */
	bool isRendering = Renderer::getInstance()->getRendering();

	/** Call MIDI Hook */
	if(!isRendering) {
		juce::ScopedReadLock locker(this->hookLock);
		if (this->midiHook) {
			for (auto m : midi) {
				juce::MessageManager::callAsync(
					[mes = m.getMessage(), hook = this->midiHook] {
						hook(mes, true);
					});
			}
		}
	}

	/** Transport MMC */
	if (!isRendering) {
		for (auto m : midi) {
			auto mes = m.getMessage();
			if (mes.isMidiMachineControlMessage()) {
				switch (mes.getMidiMachineControlCommand())
				{
				case juce::MidiMessage::MidiMachineControlCommand::mmc_play:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->play(); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_pause:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->pause(); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_stop:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->stop(); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_rewind:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->rewind(); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_recordStart:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->record(true); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_recordStop:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->record(false); });
					continue;
				}
			}
		}
	}

	/** Truncate Input */
	if (isRendering) {
		audio.setSize(audio.getNumChannels(), audio.getNumSamples(), false, true, true);
		midi.clear();
	}

	/** Process Audio Block */
	this->juce::AudioProcessorGraph::processBlock(audio, midi);

	/** Truncate Output */
	if (isRendering) {
		audio.setSize(audio.getNumChannels(), audio.getNumSamples(), false, true, true);
		midi.clear();
	}

	/** MIDI Output */
	if (!isRendering) {
		juce::ScopedReadLock locker(this->midiLock);
		if (this->midiOutput) {
			/** Send Message */
			this->midiOutput->sendBlockOfMessagesNow(midi);

			/** Call MIDI Hook */
			if (this->midiHook) {
				for (auto m : midi) {
					juce::MessageManager::callAsync(
						[mes = m.getMessage(), hook = this->midiHook] {
							hook(mes, false);
						});
				}
			}
		}
	}

	/** Add Position */
	if (auto position = dynamic_cast<PlayPosition*>(this->getPlayHead())) {
		position->next(audio.getNumSamples());
	}
}

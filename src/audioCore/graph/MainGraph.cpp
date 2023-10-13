#include "MainGraph.h"

#include "../misc/PlayPosition.h"
#include "../misc/Renderer.h"
#include "../source/CloneableSourceManager.h"
#include "../AudioCore.h"
#include "../Utils.h"
#include "SourceRecordProcessor.h"
#include <VSP4.h>

using namespace org::vocalsharp::vocalshaper;

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

void MainGraph::clearGraph() {
	auto recorder = dynamic_cast<SourceRecordProcessor*>(this->recorderNode->getProcessor());
	if (recorder) {
		recorder->clearGraph();
	}

	for (auto& i : this->midiI2InstrConnectionList) {
		this->removeConnection(i);
	}
	this->midiI2InstrConnectionList.clear();

	for (auto& i : this->midiSrc2InstrConnectionList) {
		this->removeConnection(i);
	}
	this->midiSrc2InstrConnectionList.clear();

	for (auto& i : this->midiSrc2TrkConnectionList) {
		this->removeConnection(i);
	}
	this->midiSrc2TrkConnectionList.clear();

	for (auto& i : this->audioSrc2TrkConnectionList) {
		this->removeConnection(i);
	}
	this->audioSrc2TrkConnectionList.clear();

	for (auto& i : this->audioInstr2TrkConnectionList) {
		this->removeConnection(i);
	}
	this->audioInstr2TrkConnectionList.clear();

	for (auto& i : this->midiI2TrkConnectionList) {
		this->removeConnection(i);
	}
	this->midiI2TrkConnectionList.clear();

	for (auto& i : this->audioI2TrkConnectionList) {
		this->removeConnection(i);
	}
	this->audioI2TrkConnectionList.clear();

	for (auto& i : this->audioTrk2TrkConnectionList) {
		this->removeConnection(i);
	}
	this->audioTrk2TrkConnectionList.clear();

	for (auto& i : this->audioTrk2OConnectionList) {
		this->removeConnection(i);
	}
	this->audioTrk2OConnectionList.clear();

	for (auto& i : this->midiTrk2OConnectionList) {
		this->removeConnection(i);
	}
	this->midiTrk2OConnectionList.clear();

	for (auto& i : this->trackNodeList) {
		this->removeNode(i->nodeID);
	}
	this->trackNodeList.clear();

	for (auto& i : this->instrumentNodeList) {
		this->removeNode(i->nodeID);
	}
	this->instrumentNodeList.clear();

	for (auto& i : this->audioSourceNodeList) {
		this->removeNode(i->nodeID);
	}
	this->audioSourceNodeList.clear();
}

bool MainGraph::parse(const google::protobuf::Message* data) {
	auto mes = dynamic_cast<const vsp4::MainGraph*>(data);
	if (!mes) { return false; }

	this->clearGraph();

	auto& seqTracks = mes->seqtracks();
	for (auto& i : seqTracks) {
		this->insertSource(-1, utils::getChannelSet(static_cast<utils::TrackType>(i.type())));
		if (auto sourceNode = this->audioSourceNodeList.getLast()) {
			sourceNode->setBypassed(i.bypassed());
			if (auto source = dynamic_cast<SeqSourceProcessor*>(sourceNode->getProcessor())) {
				if (!source->parse(&i)) { return false; }
			}
		}
	}

	auto& instrs = mes->instrs();
	for (auto& i : instrs) {
		this->insertInstrument(-1, utils::getChannelSet(static_cast<utils::TrackType>(i.info().decoratortype())));
		if (auto instrNode = this->instrumentNodeList.getLast()) {
			instrNode->setBypassed(i.bypassed());
			if (auto instr = dynamic_cast<PluginDecorator*>(instrNode->getProcessor())) {
				if (!instr->parse(&i)) { return false; }
			}
		}
	}

	auto& mixTracks = mes->mixertracks();
	for (auto& i : mixTracks) {
		this->insertTrack(-1, utils::getChannelSet(static_cast<utils::TrackType>(i.type())));
		if (auto trackNode = this->trackNodeList.getLast()) {
			trackNode->setBypassed(i.bypassed());
			if (auto track = dynamic_cast<Track*>(trackNode->getProcessor())) {
				if (!track->parse(&i)) { return false; }
			}
		}
	}

	auto& connections = mes->connections();

	auto& midiI2Instr = connections.midii2instr();
	for (auto& i : midiI2Instr) {
		this->setMIDII2InstrConnection(i.dst());
	}

	auto& midiSrc2Instr = connections.midisrc2instr();
	for (auto& i : midiSrc2Instr) {
		this->setMIDISrc2InstrConnection(i.src(), i.dst());
	}

	auto& midiSrc2Track = connections.midisrc2track();
	for (auto& i : midiSrc2Track) {
		this->setMIDISrc2TrkConnection(i.src(), i.dst());
	}

	auto& audioSrc2Track = connections.audiosrc2track();
	for (auto& i : audioSrc2Track) {
		this->setAudioSrc2TrkConnection(i.src(), i.dst(), i.srcchannel(), i.dstchannel());
	}

	auto& audioInstr2Track = connections.audioinstr2track();
	for (auto& i : audioInstr2Track) {
		this->setAudioInstr2TrkConnection(i.src(), i.dst(), i.srcchannel(), i.dstchannel());
	}

	auto& midiI2Track = connections.midii2track();
	for (auto& i : midiI2Track) {
		this->setMIDII2TrkConnection(i.dst());
	}

	auto& audioI2Track = connections.audioi2track();
	for (auto& i : audioI2Track) {
		this->setAudioI2TrkConnection(i.dst(), i.srcchannel(), i.dstchannel());
	}

	auto& audioTrack2O = connections.audiotrack2o();
	for (auto& i : audioTrack2O) {
		this->setAudioTrk2OConnection(i.src(), i.srcchannel(), i.dstchannel());
	}

	auto& audioTrack2Track = connections.audiotrack2track();
	for (auto& i : audioTrack2Track) {
		this->setAudioTrk2TrkConnection(i.src(), i.dst(), i.srcchannel(), i.dstchannel());
	}

	auto& midiTrack2O = connections.miditrack2o();
	for (auto& i : midiTrack2O) {
		this->setMIDITrk2OConnection(i.src());
	}

	if (!dynamic_cast<SourceRecordProcessor*>(
		this->recorderNode->getProcessor())->parse(&(mes->recorder()))) {
		return false;
	}

	return true;
}

std::unique_ptr<google::protobuf::Message> MainGraph::serialize() const {
	auto mes = std::make_unique<vsp4::MainGraph>();

	auto seqTracks = mes->mutable_seqtracks();
	for (auto& i : this->audioSourceNodeList) {
		if (auto track = dynamic_cast<SeqSourceProcessor*>(i->getProcessor())) {
			auto tmes = track->serialize();
			if (!dynamic_cast<vsp4::SeqTrack*>(tmes.get())) { return nullptr; }
			dynamic_cast<vsp4::SeqTrack*>(tmes.get())->set_bypassed(i->isBypassed());
			seqTracks->AddAllocated(dynamic_cast<vsp4::SeqTrack*>(tmes.release()));
		}
	}

	auto instrs = mes->mutable_instrs();
	for (auto& i : this->instrumentNodeList) {
		if (auto instr = dynamic_cast<PluginDecorator*>(i->getProcessor())) {
			auto imes = instr->serialize();
			if (!dynamic_cast<vsp4::Plugin*>(imes.get())) { return nullptr; }
			dynamic_cast<vsp4::Plugin*>(imes.get())->set_bypassed(i->isBypassed());
			instrs->AddAllocated(dynamic_cast<vsp4::Plugin*>(imes.release()));
		}
	}

	auto mixTracks = mes->mutable_mixertracks();
	for (auto& i : this->trackNodeList) {
		if (auto track = dynamic_cast<Track*>(i->getProcessor())) {
			auto tmes = track->serialize();
			if (!dynamic_cast<vsp4::MixerTrack*>(tmes.get())) { return nullptr; }
			dynamic_cast<vsp4::MixerTrack*>(tmes.get())->set_bypassed(i->isBypassed());
			mixTracks->AddAllocated(dynamic_cast<vsp4::MixerTrack*>(tmes.release()));
		}
	}

	auto connections = mes->mutable_connections();
	
	auto midiI2Instr = connections->mutable_midii2instr();
	for (auto& i : this->midiI2InstrConnectionList) {
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		if (!dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::MIDIInputConnection>();
		cmes->set_dst(this->findInstr(dynamic_cast<PluginDecorator*>(dstNode->getProcessor())));

		midiI2Instr->AddAllocated(cmes.release());
	}

	auto midiSrc2Instr = connections->mutable_midisrc2instr();
	for (auto& i : this->midiSrc2InstrConnectionList) {
		auto srcNode = this->getNodeForId(i.source.nodeID);
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		if (!srcNode || !dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::MIDISendConnection>();
		cmes->set_src(this->findSource(dynamic_cast<SeqSourceProcessor*>(srcNode->getProcessor())));
		cmes->set_dst(this->findInstr(dynamic_cast<PluginDecorator*>(dstNode->getProcessor())));

		midiSrc2Instr->AddAllocated(cmes.release());
	}

	auto midiSrc2Track = connections->mutable_midisrc2track();
	for (auto& i : this->midiSrc2TrkConnectionList) {
		auto srcNode = this->getNodeForId(i.source.nodeID);
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		if (!srcNode || !dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::MIDISendConnection>();
		cmes->set_src(this->findSource(dynamic_cast<SeqSourceProcessor*>(srcNode->getProcessor())));
		cmes->set_dst(this->findTrack(dynamic_cast<Track*>(dstNode->getProcessor())));

		midiSrc2Track->AddAllocated(cmes.release());
	}

	auto audioSrc2Track = connections->mutable_audiosrc2track();
	for (auto& i : this->audioSrc2TrkConnectionList) {
		auto srcNode = this->getNodeForId(i.source.nodeID);
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		int srcChannel = i.source.channelIndex;
		int dstChannel = i.destination.channelIndex;
		if (!srcNode || !dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::AudioSendConnection>();
		cmes->set_src(this->findSource(dynamic_cast<SeqSourceProcessor*>(srcNode->getProcessor())));
		cmes->set_dst(this->findTrack(dynamic_cast<Track*>(dstNode->getProcessor())));
		cmes->set_srcchannel(srcChannel);
		cmes->set_dstchannel(dstChannel);

		audioSrc2Track->AddAllocated(cmes.release());
	}

	auto audioInstr2Track = connections->mutable_audioinstr2track();
	for (auto& i : this->audioInstr2TrkConnectionList) {
		auto srcNode = this->getNodeForId(i.source.nodeID);
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		int srcChannel = i.source.channelIndex;
		int dstChannel = i.destination.channelIndex;
		if (!srcNode || !dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::AudioSendConnection>();
		cmes->set_src(this->findInstr(dynamic_cast<PluginDecorator*>(srcNode->getProcessor())));
		cmes->set_dst(this->findTrack(dynamic_cast<Track*>(dstNode->getProcessor())));
		cmes->set_srcchannel(srcChannel);
		cmes->set_dstchannel(dstChannel);

		audioInstr2Track->AddAllocated(cmes.release());
	}

	auto midiI2Track = connections->mutable_midii2track();
	for (auto& i : this->midiI2TrkConnectionList) {
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		if (!dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::MIDIInputConnection>();
		cmes->set_dst(this->findTrack(dynamic_cast<Track*>(dstNode->getProcessor())));

		midiI2Track->AddAllocated(cmes.release());
	}

	auto audioI2Track = connections->mutable_audioi2track();
	for (auto& i : this->audioI2TrkConnectionList) {
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		int srcChannel = i.source.channelIndex;
		int dstChannel = i.destination.channelIndex;
		if (!dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::AudioInputConnection>();
		cmes->set_dst(this->findTrack(dynamic_cast<Track*>(dstNode->getProcessor())));
		cmes->set_srcchannel(srcChannel);
		cmes->set_dstchannel(dstChannel);

		audioI2Track->AddAllocated(cmes.release());
	}

	auto audioTrack2O = connections->mutable_audiotrack2o();
	for (auto& i : this->audioTrk2OConnectionList) {
		auto srcNode = this->getNodeForId(i.source.nodeID);
		int srcChannel = i.source.channelIndex;
		int dstChannel = i.destination.channelIndex;
		if (!srcNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::AudioOutputConnection>();
		cmes->set_src(this->findTrack(dynamic_cast<Track*>(srcNode->getProcessor())));
		cmes->set_srcchannel(srcChannel);
		cmes->set_dstchannel(dstChannel);

		audioTrack2O->AddAllocated(cmes.release());
	}

	auto audioTrack2Track = connections->mutable_audiotrack2track();
	for (auto& i : this->audioTrk2TrkConnectionList) {
		auto srcNode = this->getNodeForId(i.source.nodeID);
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		int srcChannel = i.source.channelIndex;
		int dstChannel = i.destination.channelIndex;
		if (!srcNode || !dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::AudioSendConnection>();
		cmes->set_src(this->findTrack(dynamic_cast<Track*>(srcNode->getProcessor())));
		cmes->set_dst(this->findTrack(dynamic_cast<Track*>(dstNode->getProcessor())));
		cmes->set_srcchannel(srcChannel);
		cmes->set_dstchannel(dstChannel);

		audioTrack2Track->AddAllocated(cmes.release());
	}

	auto midiTrack2O = connections->mutable_miditrack2o();
	for (auto& i : this->midiTrk2OConnectionList) {
		auto srcNode = this->getNodeForId(i.source.nodeID);
		if (!srcNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::MIDIOutputConnection>();
		cmes->set_src(this->findTrack(dynamic_cast<Track*>(srcNode->getProcessor())));

		midiTrack2O->AddAllocated(cmes.release());
	}

	auto recorder = dynamic_cast<SourceRecordProcessor*>(this->recorderNode->getProcessor())->serialize();
	if (!dynamic_cast<vsp4::Recorder*>(recorder.get())) { return nullptr; }
	mes->set_allocated_recorder(dynamic_cast<vsp4::Recorder*>(recorder.release()));

	return std::unique_ptr<google::protobuf::Message>(mes.release());
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
		if (this->midiHook) {
			for (auto m : midi) {
				juce::MessageManager::callAsync(
					[mes = m.getMessage(), hook = this->midiHook] {
						hook(mes, false);
					});
			}
		}
	}

	/** Add Position */
	if (auto position = dynamic_cast<PlayPosition*>(this->getPlayHead())) {
		position->next(audio.getNumSamples());
	}
}

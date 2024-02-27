#include "SeqSourceProcessor.h"
#include "../misc/PlayPosition.h"
#include "../misc/AudioLock.h"
#include "../misc/VMath.h"
#include "../AudioConfig.h"
#include "../uiCallback/UICallback.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

SeqSourceProcessor::SeqSourceProcessor(const juce::AudioChannelSet& type)
	: audioChannels(type) {
	/** Set Channel Layout */
	juce::AudioProcessorGraph::BusesLayout layout;
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(type.size()));
	layout.outputBuses.add(
		juce::AudioChannelSet::discreteChannels(type.size()));
	this->setBusesLayout(layout);

	/** The Main Audio IO Node Of The Track */
	this->audioInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
	this->audioOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

	/** The Main MIDI IO Node Of The Track */
	this->midiInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));
	this->midiOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode));

	/** Set Audio IO Node Channel Num */
	this->audioInputNode->getProcessor()->setBusesLayout(layout);
	this->audioOutputNode->getProcessor()->setBusesLayout(layout);

	/** Connect IO Node */
	this->addConnection({ { this->midiInputNode->nodeID, this->midiChannelIndex },
			{ this->midiOutputNode->nodeID, this->midiChannelIndex } });
	for (int i = 0; i < type.size(); i++) {
		this->addConnection({ { this->audioInputNode->nodeID, i },
			{ this->audioOutputNode->nodeID, i } });
	}

	/** Default Color */
	this->trackColor = utils::getDefaultColour();
}

int SeqSourceProcessor::addSeq(const SourceList::SeqBlock& block) {
	return this->srcs.add(block);
}

void SeqSourceProcessor::removeSeq(int index) {
	this->srcs.remove(index);
}

int SeqSourceProcessor::getSeqNum() const {
	return this->srcs.size();
}

const SourceList::SeqBlock SeqSourceProcessor::getSeq(int index) const {
	return this->srcs.get(index);
}

void SeqSourceProcessor::setTrackName(const juce::String& name) {
	this->trackName = name;
}

const juce::String SeqSourceProcessor::getTrackName() const {
	return this->trackName;
}

void SeqSourceProcessor::setTrackColor(const juce::Colour& color) {
	this->trackColor = color;
}

const juce::Colour SeqSourceProcessor::getTrackColor() const {
	return this->trackColor;
}

const juce::AudioChannelSet& SeqSourceProcessor::getAudioChannelSet() const {
	return this->audioChannels;
}

void SeqSourceProcessor::closeAllNote() {
	this->noteCloseFlag = true;
}

void SeqSourceProcessor::setInstr(std::unique_ptr<juce::AudioPluginInstance> processor,
	const juce::String& identifier) {
	/** Check Processor */
	if (!processor) { jassertfalse; return; }

	/** Add Node */
	if (auto ptrNode = this->prepareInstr()) {
		/** Set Plugin */
		ptrNode->setPlugin(std::move(processor), identifier);

		/** Callback */
		UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);/**< TODO */
	}
	else {
		jassertfalse;
	}
}

PluginDecorator::SafePointer SeqSourceProcessor::prepareInstr() {
	/** Remove Current Instr */
	this->removeInstr();

	/** Add Node */
	if (auto ptrNode = this->addNode(
		std::make_unique<PluginDecorator>(true, this->audioChannels))) {
		/** Set Instr */
		this->instr = ptrNode;

		/** Get Decorator */
		auto decorator = dynamic_cast<PluginDecorator*>(ptrNode->getProcessor());

		/** Prepare To Play */
		decorator->setPlayHead(this->getPlayHead());
		decorator->prepareToPlay(this->getSampleRate(), this->getBlockSize());

		/** Connect IO */
		this->addConnection({ { this->midiInputNode->nodeID, this->midiChannelIndex },
			{ ptrNode->nodeID, this->midiChannelIndex } });
		this->addConnection({ { ptrNode->nodeID, this->midiChannelIndex },
			{ this->midiOutputNode->nodeID, this->midiChannelIndex } });
		for (int i = 0; i < this->audioChannels.size(); i++) {
			this->addConnection({ { this->audioInputNode->nodeID, i },
				{ ptrNode->nodeID, i } });
			this->addConnection({ { ptrNode->nodeID, i },
				{ this->audioOutputNode->nodeID, i } });
		}

		/** Callback */
		UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);/**< TODO */

		return PluginDecorator::SafePointer{ decorator };
	}
	else {
		jassertfalse;
		return nullptr;
	}
}

void SeqSourceProcessor::removeInstr() {
	if (auto ptrNode = this->instr) {
		/** Remove Instr */
		this->instr = nullptr;

		/** Disconnect IO */
		this->removeConnection({ { this->midiInputNode->nodeID, this->midiChannelIndex },
			{ ptrNode->nodeID, this->midiChannelIndex } });
		this->removeConnection({ { ptrNode->nodeID, this->midiChannelIndex },
			{ this->midiOutputNode->nodeID, this->midiChannelIndex } });
		for (int i = 0; i < this->audioChannels.size(); i++) {
			this->removeConnection({ { this->audioInputNode->nodeID, i },
				{ ptrNode->nodeID, i } });
			this->removeConnection({ { ptrNode->nodeID, i },
				{ this->audioOutputNode->nodeID, i } });
		}

		/** Remove Node */
		this->removeNode(ptrNode->nodeID);

		/** Callback */
		UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);/**< TODO */
	}
}

PluginDecorator* SeqSourceProcessor::getInstrProcessor() const {
	return dynamic_cast<PluginDecorator*>(this->instr->getProcessor());
}

void SeqSourceProcessor::setInstrumentBypass(bool bypass) {
	SeqSourceProcessor::setInstrumentBypass(PluginDecorator::SafePointer{
			dynamic_cast<PluginDecorator*>(this->instr->getProcessor()) }, bypass);
}

bool SeqSourceProcessor::getInstrumentBypass() const {
	return SeqSourceProcessor::getInstrumentBypass(PluginDecorator::SafePointer{
			dynamic_cast<PluginDecorator*>(this->instr->getProcessor()) });
}

void SeqSourceProcessor::setInstrumentBypass(PluginDecorator::SafePointer instr, bool bypass) {
	if (instr) {
		if (auto bypassParam = instr->getBypassParameter()) {
			bypassParam->setValueNotifyingHost(bypass ? 1.0f : 0.0f);

			/** Callback */
			UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);
		}
	}
}

bool SeqSourceProcessor::getInstrumentBypass(PluginDecorator::SafePointer instr) {
	if (instr) {
		if (auto bypassParam = instr->getBypassParameter()) {
			return !juce::approximatelyEqual(bypassParam->getValue(), 0.0f);
		}
	}
	return false;
}

double SeqSourceProcessor::getSourceLength() const {
	return std::max(this->getMIDILength(), this->getAudioLength());
}

double SeqSourceProcessor::getMIDILength() const {
	if (!this->midiData) { return 0; }
	return this->midiData->getLastTimestamp() + AudioConfig::getMidiTail();
}

double SeqSourceProcessor::getAudioLength() const {
	if (!this->audioData) { return 0; }
	return this->audioData->getNumSamples() / this->audioSampleRate;
}

void SeqSourceProcessor::initAudio(double sampleRate, int channelNum, int sampleNum) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Clear Audio Source */
	this->resampleSource = nullptr;
	this->memSource = nullptr;
	this->audioData = nullptr;

	/** Create Buffer */
	this->audioData = std::make_unique<juce::AudioSampleBuffer>(channelNum, sampleNum);
	vMath::zeroAllAudioData(*(this->audioData.get()));
	this->audioSampleRate = sampleRate;

	/** Update Resample Source */
	this->updateAudioResampler();
}

void SeqSourceProcessor::initMIDI() {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Create MIDI Data */
	this->midiData = std::make_unique<juce::MidiFile>();
}

void SeqSourceProcessor::setAudio(double sampleRate, const juce::AudioSampleBuffer& data) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Clear Audio Source */
	this->resampleSource = nullptr;
	this->memSource = nullptr;
	this->audioData = nullptr;

	/** Create Buffer */
	this->audioData = std::make_unique<juce::AudioSampleBuffer>(data);
	this->audioSampleRate = sampleRate;

	/** Update Resample Source */
	this->updateAudioResampler();
}

void SeqSourceProcessor::setMIDI(const juce::MidiFile& data) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Create MIDI Data */
	this->midiData = std::make_unique<juce::MidiFile>(data);
}

const std::tuple<double, juce::AudioSampleBuffer> SeqSourceProcessor::getAudio() const {
	/** Lock */
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	/** Check Data */
	if (!this->audioData) {
		return { 0, juce::AudioSampleBuffer{} };
	}

	/** Copy Data */
	return { this->audioSampleRate, *(this->audioData.get()) };
}

const juce::MidiFile SeqSourceProcessor::getMIDI() const {
	/** Lock */
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	/** Check Data */
	if (!this->midiData) {
		return juce::MidiFile{};
	}

	/** Copy Data */
	return *(this->midiData.get());
}

void SeqSourceProcessor::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	this->prepareMIDIPlay(sampleRate, maximumExpectedSamplesPerBlock);
	this->prepareAudioPlay(sampleRate, maximumExpectedSamplesPerBlock);
}

void SeqSourceProcessor::processBlock(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	/** Check Buffer Is Empty */
	if (buffer.getNumSamples() <= 0) { return; }

	/** Clear Audio Channel */
	auto dspBlock = juce::dsp::AudioBlock<float>(buffer).getSubsetChannelBlock(
		0, buffer.getNumChannels());
	dspBlock.fill(0);

	/** Clear MIDI Buffer */
	midiMessages.clear();

	/** Close Note */
	if (this->noteCloseFlag) {
		this->noteCloseFlag = false;

		for (auto& i : this->activeNoteSet) {
			midiMessages.addEvent(
				juce::MidiMessage::noteOff(std::get<0>(i), std::get<1>(i)), 0);
		}
	}

	/** Get Play Head */
	auto playHead = dynamic_cast<PlayPosition*>(this->getPlayHead());
	if (!playHead) { return; }

	/** Get Current Position */
	juce::Optional<juce::AudioPlayHead::PositionInfo> position = playHead->getPosition();
	if (!position) { return; }

	/** Check Play State */
	if (!position->getIsPlaying()) { return; }

	/** Get Time */
	double startTime = position->getTimeInSeconds().orFallback(-1);
	double sampleRate = this->getSampleRate();
	double duration = buffer.getNumSamples() / sampleRate;
	double endTime = startTime + duration;

	int startTimeInSample = position->getTimeInSamples().orFallback(-1);
	int durationInSample = buffer.getNumSamples();
	int endTimeInSample = startTimeInSample + durationInSample;

	/** Copy Source Data */
	{
		/** Find Hot Block */
		auto index = this->srcs.match(startTime, endTime);

		for (int i = std::get<0>(index);
			i <= std::get<1>(index) && i < this->srcs.size() && i >= 0; i++) {
			/** Get Block */
			auto [blockStartTime, blockEndTime, blockOffset] = this->srcs.getUnchecked(i);
			int blockStartTimeInSample = std::floor(blockStartTime * sampleRate);
			int blockEndTimeInSample = std::floor(blockEndTime * sampleRate);
			int blockOffsetInSample = std::floor(blockOffset * sampleRate);

			/** Caculate Time */
			int blockLengthInSample = std::floor(this->getSourceLength() * sampleRate);
			int dataStartTimeInSample = blockStartTimeInSample + std::max(blockOffsetInSample, 0);
			int dataEndTimeInSample =
				std::min(blockEndTimeInSample, blockStartTimeInSample + blockOffsetInSample + blockLengthInSample);

			if (dataEndTimeInSample > dataStartTimeInSample) {
				int hotStartTimeInSample = std::max(startTimeInSample, dataStartTimeInSample);
				int hotEndTimeInSample = std::min(endTimeInSample, dataEndTimeInSample);

				if (hotEndTimeInSample > hotStartTimeInSample) {
					int dataTimeInSample = blockStartTimeInSample + blockOffsetInSample;
					int bufferOffsetInSample = hotStartTimeInSample - startTimeInSample;
					int hotOffsetInSample = hotStartTimeInSample - dataTimeInSample;
					int hotLengthInSample = hotEndTimeInSample - hotStartTimeInSample;

					/** Read Data */
					this->readAudioData(buffer, bufferOffsetInSample,
						hotOffsetInSample, hotLengthInSample);
					this->readMIDIData(midiMessages,
						dataTimeInSample - startTimeInSample,
						hotOffsetInSample,
						hotEndTimeInSample - dataTimeInSample);
				}
			}
		}
	}

	/** Set Note State */
	for (auto i : midiMessages) {
		auto mes = i.getMessage();
		if (mes.isNoteOn(false)) {
			this->activeNoteSet.insert({ mes.getChannel(), mes.getNoteNumber() });
		}
		else if (mes.isNoteOff(true)) {
			this->activeNoteSet.erase({ mes.getChannel(), mes.getNoteNumber() });
		}
	}

	/** Process Graph */
	this->juce::AudioProcessorGraph::processBlock(buffer, midiMessages);
}

double SeqSourceProcessor::getTailLengthSeconds() const {
	int size = this->srcs.size();
	return (size > 0) ? std::get<1>(this->srcs.getUnchecked(size - 1)) : 0;
}

void SeqSourceProcessor::clearGraph() {
	this->setTrackName(juce::String{});
	this->setTrackColor(juce::Colour{});

	this->srcs.clearGraph();
}

bool SeqSourceProcessor::parse(const google::protobuf::Message* data) {
	auto mes = dynamic_cast<const vsp4::SeqTrack*>(data);
	if (!mes) { return false; }

	auto& info = mes->info();
	this->setTrackName(info.name());
	this->setTrackColor(juce::Colour{ info.color() });

	auto& sources = mes->sources();
	if (!this->srcs.parse(&sources)) { return false; }

	if (mes->has_instr()) {
		auto& instr = mes->instr();

		if (auto plugin = this->prepareInstr()) {
			SeqSourceProcessor::setInstrumentBypass(PluginDecorator::SafePointer{ plugin }, instr.bypassed());
			if (!plugin->parse(&instr)) { return false; }
		}
	}

	return true;
}

std::unique_ptr<google::protobuf::Message> SeqSourceProcessor::serialize() const {
	auto mes = std::make_unique<vsp4::SeqTrack>();

	mes->set_type(static_cast<vsp4::TrackType>(utils::getTrackType(this->audioChannels)));
	auto info = mes->mutable_info();
	info->set_name(this->getTrackName().toStdString());
	info->set_color(this->getTrackColor().getARGB());

	auto srcs = this->srcs.serialize();
	if (!dynamic_cast<vsp4::SourceInstanceList*>(srcs.get())) { return nullptr; }
	mes->set_allocated_sources(dynamic_cast<vsp4::SourceInstanceList*>(srcs.release()));

	if (this->instr) {
		if (auto plugin = dynamic_cast<PluginDecorator*>(this->instr->getProcessor())) {
			if (auto item = plugin->serialize()) {
				if (auto plu = dynamic_cast<vsp4::Plugin*>(item.get())) {
					plu->set_bypassed(SeqSourceProcessor::getInstrumentBypass(
						PluginDecorator::SafePointer{ plugin }));

					mes->set_allocated_instr(dynamic_cast<vsp4::Plugin*>(item.release()));
				}
			}
			else {
				return nullptr;
			}
		}
	}

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

void SeqSourceProcessor::prepareAudioPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	/** Lock */
	juce::ScopedWriteLock sourceLock(audioLock::getSourceLock());

	/** Update Audio Sample Rate */
	if (this->resampleSource) {
		this->resampleSource->setResamplingRatio(this->audioSampleRate / sampleRate);
		this->resampleSource->prepareToPlay(maximumExpectedSamplesPerBlock, sampleRate);
	}
}

void SeqSourceProcessor::prepareMIDIPlay(
	double /*sampleRate*/, int /*maximumExpectedSamplesPerBlock*/) {
	/** Nothing To Do */
}

void SeqSourceProcessor::readAudioData(
	juce::AudioBuffer<float>& buffer, int bufferOffset,
	int dataOffset, int length) const {
	/** Check Source */
	if (!this->resampleSource) { return; }
	if (buffer.getNumSamples() <= 0 || length <= 0) { return; }

	/** Get Data */
	if (this->resampleSource && this->memSource) {
		this->memSource->setNextReadPosition(
			(int64_t)(dataOffset * this->resampleSource->getResamplingRatio()));
		int startSample = bufferOffset;
		this->resampleSource->getNextAudioBlock(juce::AudioSourceChannelInfo{
			&buffer, startSample,
				std::min(buffer.getNumSamples() - startSample, length) });
	}
}

void SeqSourceProcessor::readMIDIData(
	juce::MidiBuffer& buffer, int baseTime,
	int startTime, int endTime) const {
	/** Check Source */
	if (!this->midiData) { return; }

	/** Get MIDI Data */
	juce::MidiMessageSequence total;
	for (int i = 0; i < this->midiData->getNumTracks(); i++) {
		if (auto track = this->midiData->getTrack(i)) {
			total.addSequence(*track, 0,
				startTime / this->getSampleRate(), endTime / this->getSampleRate());
		}
	}

	/** Copy Data */
	for (auto i : total) {
		auto& message = i->message;
		double time = message.getTimeStamp();
		buffer.addEvent(message,
			std::floor((time + baseTime / this->getSampleRate()) * this->getSampleRate()));
	}
}

void SeqSourceProcessor::updateAudioResampler() {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Check Audio Data */
	if (!this->audioData) { return; }

	/** Remove Resample Source */
	this->resampleSource = nullptr;

	/** Create Audio Source */
	this->memSource = std::make_unique<juce::MemoryAudioSource>(
		*(this->audioData.get()), false, false);
	auto resSource = std::make_unique<juce::ResamplingAudioSource>(
		this->memSource.get(), false, this->audioData->getNumChannels());

	/** Set Sample Rate */
	resSource->setResamplingRatio(this->audioSampleRate / this->getSampleRate());
	resSource->prepareToPlay(this->getBlockSize(), this->getSampleRate());

	/** Set Resample Source */
	this->resampleSource = std::move(resSource);
}

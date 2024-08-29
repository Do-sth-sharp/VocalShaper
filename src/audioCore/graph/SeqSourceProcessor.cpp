#include "SeqSourceProcessor.h"
#include "../misc/PlayPosition.h"
#include "../misc/AudioLock.h"
#include "../misc/VMath.h"
#include "../source/SourceManager.h"
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

	/** TODO Synth Thread */

	/** Set Level Size */
	this->outputLevels.resize(type.size());

	/** Default Color */
	this->trackColor = utils::getDefaultColour();
}

SeqSourceProcessor::~SeqSourceProcessor() {
	this->releaseAudio();
	this->releaseMIDI();
}

void SeqSourceProcessor::updateIndex(int index) {
	this->index = index;
	this->srcs.updateIndex(index);

	/** TODO Synth Index */

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqChanged, index);
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

bool SeqSourceProcessor::splitSeq(int index, double time) {
	return this->srcs.split(index, time);
}

bool SeqSourceProcessor::stickSeqWithNext(int index) {
	return this->srcs.stickWithNext(index);
}

int SeqSourceProcessor::resetSeqTime(
	int index, const SourceList::SeqBlock& block) {
	return this->srcs.resetTime(index, block);
}

void SeqSourceProcessor::setTrackName(const juce::String& name) {
	this->trackName = name;

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqChanged, this->index);
}

const juce::String SeqSourceProcessor::getTrackName() const {
	return this->trackName;
}

void SeqSourceProcessor::setTrackColor(const juce::Colour& color) {
	this->trackColor = color;

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqChanged, this->index);
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
	juce::ScopedWriteLock pluginLocker(audioLock::getPluginLock());

	/** Check Processor */
	if (!processor) { jassertfalse; return; }

	/** Add Node */
	if (auto ptrNode = this->prepareInstr()) {
		/** Set Plugin */
		ptrNode->setPlugin(std::move(processor), identifier);

		/** Callback */
		UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, this->index);
	}
	else {
		jassertfalse;
	}
}

PluginDecorator::SafePointer SeqSourceProcessor::prepareInstr() {
	juce::ScopedWriteLock pluginLocker(audioLock::getPluginLock());

	/** Remove Current Instr */
	this->removeInstr();

	/** Add Node */
	if (auto ptrNode = this->addNode(
		std::make_unique<PluginDecorator>(this, true, this->audioChannels))) {
		/** Set Instr */
		this->instr = ptrNode;

		/** Get Decorator */
		auto decorator = dynamic_cast<PluginDecorator*>(ptrNode->getProcessor());

		/** Prepare To Play */
		decorator->setPlayHead(this->getPlayHead());
		decorator->prepareToPlay(this->getSampleRate(), this->getBlockSize());

		/** Connect IO */
		if (!(this->instrOffline)) {
			this->linkInstr();
		}
		else {
			this->unlinkInstr();
		}

		/** Callback */
		UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, this->index);

		return PluginDecorator::SafePointer{ decorator };
	}
	else {
		jassertfalse;
		return nullptr;
	}
}

void SeqSourceProcessor::removeInstr() {
	juce::ScopedWriteLock pluginLocker(audioLock::getPluginLock());

	if (auto ptrNode = this->instr) {
		/** Remove Instr */
		this->instr = nullptr;

		/** Disconnect IO */
		this->unlinkInstr();

		/** Remove Node */
		this->removeNode(ptrNode->nodeID);

		/** Callback */
		UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, this->index);
	}
}

PluginDecorator* SeqSourceProcessor::getInstrProcessor() const {
	if (this->instr) {
		return dynamic_cast<PluginDecorator*>(this->instr->getProcessor());
	}
	return nullptr;
}

void SeqSourceProcessor::setInstrumentBypass(bool bypass) {
	juce::ScopedWriteLock pluginLocker(audioLock::getPluginLock());

	if (this->instr) {
		SeqSourceProcessor::setInstrumentBypass(PluginDecorator::SafePointer{
			dynamic_cast<PluginDecorator*>(this->instr->getProcessor()) }, bypass);
	}
}

bool SeqSourceProcessor::getInstrumentBypass() const {
	if (this->instr) {
		return SeqSourceProcessor::getInstrumentBypass(PluginDecorator::SafePointer{
			dynamic_cast<PluginDecorator*>(this->instr->getProcessor()) });
	}
	return false;
}

void SeqSourceProcessor::setInstrumentBypass(PluginDecorator::SafePointer instr, bool bypass) {
	juce::ScopedWriteLock pluginLocker(audioLock::getPluginLock());

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

void SeqSourceProcessor::setInstrOffline(bool offline) {
	juce::ScopedWriteLock pluginLocker(audioLock::getPluginLock());

	if (offline) {
		/** Unlink Channels */
		this->unlinkInstr();

		/** Set Flag */
		this->instrOffline = offline;
	}
	else {
		/** Set Flag */
		this->instrOffline = offline;

		/** Link Channels */
		this->linkInstr();
	}

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, this->index);
}

bool SeqSourceProcessor::getInstrOffline() const {
	return this->instrOffline;
}

bool SeqSourceProcessor::isSynthRunning() const {
	/** TODO */
	return false;
}

void SeqSourceProcessor::startSynth() {
	/** TODO */
}

uint64_t SeqSourceProcessor::getAudioRef() const {
	return this->audioSourceRef;
}

uint64_t SeqSourceProcessor::getMIDIRef() const {
	return this->midiSourceRef;
}

void SeqSourceProcessor::applyAudio() {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	this->releaseAudio();
	this->audioSourceRef = SourceManager::getInstance()->applySource(
		SourceManager::SourceType::Audio);

	/** Callback */
	auto callback = [ptr = SafePointer{ this }] {
		if (ptr) {
			ptr->invokeDataCallbacks();
		}
		};
	SourceManager::getInstance()->setCallback(this->audioSourceRef,
		SourceManager::SourceType::Audio, callback);
}

void SeqSourceProcessor::applyMIDI() {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	this->releaseMIDI();
	this->midiSourceRef = SourceManager::getInstance()->applySource(
		SourceManager::SourceType::MIDI);
	this->currentMIDITrack = 0;

	/** Callback */
	auto callback = [ptr = SafePointer{ this }] {
		if (ptr) {
			ptr->invokeDataCallbacks();
		}
		};
	SourceManager::getInstance()->setCallback(this->midiSourceRef,
		SourceManager::SourceType::MIDI, callback);
}

void SeqSourceProcessor::releaseAudio() {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	if (this->audioSourceRef > 0) {
		SourceManager::getInstance()->releaseSource(this->audioSourceRef);
		this->audioSourceRef = 0;
	}
}

void SeqSourceProcessor::releaseMIDI() {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	if (this->midiSourceRef > 0) {
		SourceManager::getInstance()->releaseSource(this->midiSourceRef);
		this->midiSourceRef = 0;
	}
	this->currentMIDITrack = 0;
}

void SeqSourceProcessor::applyAudioIfNeed() {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	if (this->audioSourceRef == 0) {
		this->applyAudio();
	}
}

void SeqSourceProcessor::applyMIDIIfNeed() {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	if (this->midiSourceRef == 0) {
		this->applyMIDI();
	}
}

void SeqSourceProcessor::initAudio(double sampleRate, double length) {
	this->applyAudio();
	SourceManager::getInstance()->initAudio(this->audioSourceRef,
		this->audioChannels.size(), sampleRate, length);
}

void SeqSourceProcessor::initMIDI() {
	this->applyMIDI();
	SourceManager::getInstance()->initMIDI(this->midiSourceRef);
}

const juce::String SeqSourceProcessor::getAudioFileName() const {
	juce::String name = this->getAudioName();
	if (name.isEmpty()) { name = this->trackName; }
	if (name.isEmpty()) { name = juce::String{ this->index }; }
	
	auto [extension, metaData, bitDepth, quality] = this->getAudioFormat();
	if (!utils::getAudioFormatsSupported(true).contains("*" + extension)) {
		extension.clear();
	}
	if (extension.isEmpty()) {
		extension = utils::getAudioFormatsSupported(true)[0]
			.trimCharactersAtStart("*");
	}

	return utils::getLegalFileName(name) + extension;
}

const juce::String SeqSourceProcessor::getMIDIFileName() const {
	juce::String name = this->getMIDIName();
	if (name.isEmpty()) { name = this->trackName; }
	if (name.isEmpty()) { name = juce::String{ this->index }; }

	juce::String extension = utils::getMidiFormatsSupported(true)[0]
		.trimCharactersAtStart("*");
	return utils::getLegalFileName(name) + extension;
}

const juce::String SeqSourceProcessor::getAudioName() const {
	return SourceManager::getInstance()->getFileName(
		this->audioSourceRef, SourceManager::SourceType::Audio);
}

const juce::String SeqSourceProcessor::getMIDIName() const {
	return SourceManager::getInstance()->getFileName(
		this->midiSourceRef, SourceManager::SourceType::MIDI);
}

void SeqSourceProcessor::setCurrentMIDITrack(int trackIndex) {
	this->currentMIDITrack = trackIndex;

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqDataRefChanged, this->index);
}

int SeqSourceProcessor::getCurrentMIDITrack() const {
	return this->currentMIDITrack;
}

int SeqSourceProcessor::getTotalMIDITrackNum() const {
	return SourceManager::getInstance()->getMIDITrackNum(this->midiSourceRef);
}

void SeqSourceProcessor::setRecording(bool recording) {
	if (recording) {
		this->applyAudioIfNeed();
		this->applyMIDIIfNeed();
		SourceManager::getInstance()->prepareAudioRecord(this->audioSourceRef,
			this->audioChannels.size());
		SourceManager::getInstance()->prepareMIDIRecord(this->midiSourceRef);
	}
	this->recordingFlag = recording;

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqRecChanged, this->index);
}

bool SeqSourceProcessor::getRecording() const {
	return this->recordingFlag;
}

void SeqSourceProcessor::setMute(bool mute) {
	this->isMute = mute;

	/** Close All Note */
	if (mute) {
		this->closeAllNote();
	}

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqMuteChanged, this->index);
}

bool SeqSourceProcessor::getMute() const {
	return this->isMute;
}

const juce::Array<float> SeqSourceProcessor::getOutputLevels() const {
	juce::ScopedReadLock locker(audioLock::getLevelMeterLock());
	return this->outputLevels;
}

void SeqSourceProcessor::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	this->juce::AudioProcessorGraph::prepareToPlay(
		sampleRate, maximumExpectedSamplesPerBlock);
	SourceManager::getInstance()->prepareMIDIPlay(this->midiSourceRef);
	SourceManager::getInstance()->prepareAudioPlay(this->audioSourceRef);
}

void SeqSourceProcessor::processBlock(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	/** Check Buffer Is Empty */
	if (buffer.getNumChannels() <= 0) { return; }
	if (buffer.getNumSamples() <= 0) { return; }

	/** Clear Audio Channel */
	auto dspBlock = juce::dsp::AudioBlock<float>(buffer).getSubsetChannelBlock(
		0, buffer.getNumChannels());
	dspBlock.fill(0);

	/** Play Flag */
	bool isPlaying = true;

	/** Get Play Head */
	auto playHead = dynamic_cast<PlayPosition*>(this->getPlayHead());
	if (!playHead) { isPlaying = false; }

	/** Get Current Position */
	juce::Optional<juce::AudioPlayHead::PositionInfo> position = playHead->getPosition();
	if (!position) { isPlaying = false; }

	/** Check Play State */
	if (!position->getIsPlaying()) { isPlaying = false; }

	/** Clear MIDI Buffer */
	if ((isPlaying && position->getIsRecording()) || (!this->recordingFlag)) {
		midiMessages.clear();
	}

	if (isPlaying && !(this->isMute)) {
		/** Get Time */
		double startTime = position->getTimeInSeconds().orFallback(-1);
		double sampleRate = this->getSampleRate();
		double duration = buffer.getNumSamples() / sampleRate;
		double endTime = startTime + duration;

		int startTimeInSample = position->getTimeInSamples().orFallback(-1);
		int durationInSample = buffer.getNumSamples();
		int endTimeInSample = startTimeInSample + durationInSample;

		int sourceLengthInSample = std::floor(this->getSourceLength() * sampleRate);

		/** Find Hot Block */
		auto index = this->srcs.match(startTime, endTime);

		/** Copy Source Data */
		for (int i = std::get<0>(index);
			i <= std::get<1>(index) && i < this->srcs.size() && i >= 0; i++) {
			/** Get Block */
			auto [blockStartTime, blockEndTime, sourceOffset] = this->srcs.getUnchecked(i);
			int blockStartTimeInSample = std::floor(blockStartTime * sampleRate);
			int blockEndTimeInSample = std::floor(blockEndTime * sampleRate);
			int sourceOffsetInSample = std::floor(sourceOffset * sampleRate);

			/** Caculate Time */
			int sourceStartTimeInSample = sourceOffsetInSample;
			int sourceEndTimeInSample = sourceStartTimeInSample + sourceLengthInSample;
			int dataStartTimeInSample = std::max(blockStartTimeInSample, sourceStartTimeInSample);
			int dataEndTimeInSample = std::min(blockEndTimeInSample, sourceEndTimeInSample);

			if (dataEndTimeInSample > dataStartTimeInSample) {
				int hotStartTimeInSample = std::max(startTimeInSample, dataStartTimeInSample);
				int hotEndTimeInSample = std::min(endTimeInSample, dataEndTimeInSample);
				int hotLengthInSample = hotEndTimeInSample - hotStartTimeInSample;

				if (hotLengthInSample > 0) {
					int bufferOffsetInSample = hotStartTimeInSample - startTimeInSample;
					int sourceOffsetInSample = hotStartTimeInSample - sourceStartTimeInSample;

					/** Read Data */
					this->readAudioData(buffer, bufferOffsetInSample,
						sourceOffsetInSample, hotLengthInSample);
					this->readMIDIData(midiMessages, sourceOffsetInSample,
						hotStartTimeInSample, hotEndTimeInSample);
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

	/** Close Note */
	if (this->noteCloseFlag) {
		this->noteCloseFlag = false;

		for (auto& i : this->activeNoteSet) {
			midiMessages.addEvent(
				juce::MidiMessage::noteOff(std::get<0>(i), std::get<1>(i)), 0);
		}
	}

	/** Process Graph */
	if (this->instr && !(this->instrOffline)) {
		this->juce::AudioProcessorGraph::processBlock(buffer, midiMessages);
	}

	/** Process Mute */
	if (this->isMute) {
		vMath::zeroAllAudioData(buffer);
	}

	/** Update Level Meter */
	for (int i = 0; i < buffer.getNumChannels() && i < this->outputLevels.size(); i++) {
		this->outputLevels.getReference(i) =
			buffer.getRMSLevel(i, 0, buffer.getNumSamples());
	}
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
	this->setInstrOffline(mes->offline());

	if (!mes->audiosrc().empty()) {
		this->applyAudio();
	}
	if (!mes->midisrc().empty()) {
		this->applyMIDI();
	}
	this->setCurrentMIDITrack(mes->miditrack());

	this->setRecording(mes->recording());
	this->setMute(mes->muted());

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
	mes->set_offline(this->getInstrOffline());

	if (this->isAudioValid()) {
		juce::String name = this->getAudioFileName();
		mes->set_audiosrc(name.toStdString());
	}
	if (this->isMIDIValid()) {
		juce::String name = this->getMIDIFileName();
		mes->set_midisrc(name.toStdString());
	}
	mes->set_miditrack(this->getCurrentMIDITrack());

	mes->set_recording(this->getRecording());
	mes->set_muted(this->getMute());

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

void SeqSourceProcessor::readAudioData(
	juce::AudioBuffer<float>& buffer, int bufferOffset,
	int dataOffset, int length) const {
	SourceManager::getInstance()->readAudioData(this->audioSourceRef,
		buffer, bufferOffset, dataOffset, length);
}

void SeqSourceProcessor::readMIDIData(
	juce::MidiBuffer& buffer, int baseTime,
	int startTime, int endTime) const {
	double sampleRate = this->getSampleRate();
	SourceManager::getInstance()->readMIDIData(this->midiSourceRef,
		buffer, baseTime / sampleRate, startTime / sampleRate, endTime / sampleRate,
		this->currentMIDITrack);
}

void SeqSourceProcessor::writeAudioData(juce::AudioBuffer<float>& buffer, int offset) {
	SourceManager::getInstance()->writeAudioData(this->audioSourceRef,
		buffer, offset, this->audioChannels.size());
}

void SeqSourceProcessor::writeMIDIData(const juce::MidiBuffer& buffer, int offset) {
	SourceManager::getInstance()->writeMIDIData(this->midiSourceRef,
		buffer, offset, this->currentMIDITrack);
}

bool SeqSourceProcessor::isAudioSaved() const {
	return SourceManager::getInstance()->isSaved(
		this->audioSourceRef, SourceManager::SourceType::Audio);
}

bool SeqSourceProcessor::isMIDISaved() const {
	return SourceManager::getInstance()->isSaved(
		this->midiSourceRef, SourceManager::SourceType::MIDI);
}

bool SeqSourceProcessor::isAudioValid() const {
	return SourceManager::getInstance()->isValid(
		this->audioSourceRef, SourceManager::SourceType::Audio);
}

bool SeqSourceProcessor::isMIDIValid() const {
	return SourceManager::getInstance()->isValid(
		this->midiSourceRef, SourceManager::SourceType::MIDI);
}

double SeqSourceProcessor::getSourceLength() const {
	return std::max(this->getMIDILength(), this->getAudioLength());
}

double SeqSourceProcessor::getMIDILength() const {
	return SourceManager::getInstance()->getLength(
		this->midiSourceRef, SourceManager::SourceType::MIDI);
}

double SeqSourceProcessor::getAudioLength() const {
	return SourceManager::getInstance()->getLength(
		this->audioSourceRef, SourceManager::SourceType::Audio);
}

const SeqSourceProcessor::AudioFormat SeqSourceProcessor::getAudioFormat() const {
	return SourceManager::getInstance()->getAudioFormat(this->audioSourceRef);
}

void SeqSourceProcessor::linkInstr() {
	if (auto ptrNode = this->instr) {
		/** Unlink Main IO */
		this->removeConnection({ { this->midiInputNode->nodeID, this->midiChannelIndex },
			{ this->midiOutputNode->nodeID, this->midiChannelIndex } });
		for (int i = 0; i < this->audioChannels.size(); i++) {
			this->removeConnection({ { this->audioInputNode->nodeID, i },
				{ this->audioOutputNode->nodeID, i } });
		}

		/** Link Instr IO */
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
	}
}

void SeqSourceProcessor::unlinkInstr() {
	if (auto ptrNode = this->instr) {
		/** Unlink Instr IO */
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

		/** Link Main IO */
		this->addConnection({ { this->midiInputNode->nodeID, this->midiChannelIndex },
			{ this->midiOutputNode->nodeID, this->midiChannelIndex } });
		for (int i = 0; i < this->audioChannels.size(); i++) {
			this->addConnection({ { this->audioInputNode->nodeID, i },
				{ this->audioOutputNode->nodeID, i } });
		}
	}
}

void SeqSourceProcessor::invokeDataCallbacks() const {
	UICallbackAPI<int>::invoke(UICallbackType::SeqDataRefChanged, this->index);
	UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, this->index);
}

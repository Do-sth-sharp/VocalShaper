#include "SeqSourceProcessor.h"
#include "../misc/PlayPosition.h"
#include "../misc/AudioLock.h"
#include "../misc/VMath.h"
#include "../misc/SourceIO.h"
#include "../misc/SynthThread.h"
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

	/** Synth Thread */
	this->synthThread = std::make_unique<SynthThread>(this);

	/** Set Level Size */
	this->outputLevels.resize(type.size());

	/** Default Color */
	this->trackColor = utils::getDefaultColour();
}

void SeqSourceProcessor::updateIndex(int index) {
	this->index = index;
	this->srcs.updateIndex(index);

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
		std::make_unique<PluginDecorator>(true, this->audioChannels))) {
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
		if (!(this->instrOffline)) {
			this->unlinkInstr();
		}

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
	return this->synthThread->isThreadRunning();
}

void SeqSourceProcessor::startSynth() {
	if (auto thread = dynamic_cast<SynthThread*>(this->synthThread.get())) {
		thread->synthNow();
	}
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

void SeqSourceProcessor::initAudio(double sampleRate, double length) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Clear Audio Source */
	this->resampleSource = nullptr;
	this->memSource = nullptr;
	this->audioData = nullptr;

	/** Create Buffer */
	this->audioData = std::make_unique<juce::AudioSampleBuffer>(
		this->audioChannels.size(), (int)std::ceil(length * sampleRate));
	vMath::zeroAllAudioData(*(this->audioData.get()));
	this->audioSampleRate = sampleRate;

	/** Update Resample Source */
	this->updateAudioResampler();

	/** Update Name */
	this->audioName = juce::String{};

	/** Set Flag */
	this->audioChanged();

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqDataRefChanged, this->index);
	UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, this->index);
}

void SeqSourceProcessor::initMIDI() {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Create MIDI Data */
	this->midiData = std::make_unique<juce::MidiFile>();
	this->midiData->addTrack(juce::MidiMessageSequence{});

	/** Update Name */
	this->midiName = juce::String{};

	/** Set Flag */
	this->midiChanged();

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqDataRefChanged, this->index);
	UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, this->index);
}

void SeqSourceProcessor::setAudio(double sampleRate,
	const juce::AudioSampleBuffer& data, const juce::String& name) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Clear Audio Source */
	this->resampleSource = nullptr;
	this->memSource = nullptr;
	this->audioData = nullptr;

	/** Create Buffer */
	this->audioData = std::make_unique<juce::AudioSampleBuffer>(data);
	this->audioSampleRate = sampleRate;
	this->audioName = name;

	/** Update Resample Source */
	this->updateAudioResampler();

	/** Set Flag */
	this->audioChanged();

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqDataRefChanged, this->index);
	UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, this->index);
}

void SeqSourceProcessor::setMIDI(
	const juce::MidiFile& data, const juce::String& name) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Create MIDI Data */
	this->midiData = std::make_unique<juce::MidiFile>(data);
	this->midiName = name;

	/** Set Flag */
	this->midiChanged();

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqDataRefChanged, this->index);
	UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, this->index);
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

void SeqSourceProcessor::saveAudio(const juce::String& path) const {
	/** Get Path */
	juce::String savePath = path;
	if (savePath.isEmpty()) {
		savePath = utils::getProjectDir().getChildFile(
			this->getAudioFileName()).getFullPathName();
	}

	/** Get Pointer */
	auto ptr = SeqSourceProcessor::SafePointer{
		const_cast<SeqSourceProcessor*>(this) };

	/** Save */
	SourceIO::getInstance()->addTask(
		{ SourceIO::TaskType::Write, ptr, savePath, false });
}

void SeqSourceProcessor::saveMIDI(const juce::String& path) const {
	/** Get Path */
	juce::String savePath = path;
	if (savePath.isEmpty()) {
		savePath = utils::getProjectDir().getChildFile(
			this->getMIDIFileName()).getFullPathName();
	}

	/** Get Pointer */
	auto ptr = SeqSourceProcessor::SafePointer{
		const_cast<SeqSourceProcessor*>(this) };

	/** Save */
	SourceIO::getInstance()->addTask(
		{ SourceIO::TaskType::Write, ptr, savePath, false });
}

void SeqSourceProcessor::loadAudio(const juce::String& path) {
	/** Get Pointer */
	auto ptr = SeqSourceProcessor::SafePointer{ this };

	/** Load */
	SourceIO::getInstance()->addTask(
		{ SourceIO::TaskType::Read, ptr, path, false });
}

void SeqSourceProcessor::loadMIDI(const juce::String& path, bool getTempo) {
	/** Get Pointer */
	auto ptr = SeqSourceProcessor::SafePointer{ this };

	/** Load */
	SourceIO::getInstance()->addTask(
		{ SourceIO::TaskType::Read, ptr, path, getTempo });
}

const juce::String SeqSourceProcessor::getAudioFileName() const {
	juce::String name = this->audioName;
	if (name.isEmpty()) { name = this->trackName; }
	if (name.isEmpty()) { name = juce::String{ this->index }; }
	
	juce::String extension = utils::getAudioFormatsSupported(true)[0]
		.trimCharactersAtStart("*");
	return utils::getLegalFileName(name) + extension;
}

const juce::String SeqSourceProcessor::getMIDIFileName() const {
	juce::String name = this->midiName;
	if (name.isEmpty()) { name = this->trackName; }
	if (name.isEmpty()) { name = juce::String{ this->index }; }

	juce::String extension = utils::getMidiFormatsSupported(true)[0]
		.trimCharactersAtStart("*");
	return utils::getLegalFileName(name) + extension;
}

const juce::String SeqSourceProcessor::getAudioName() const {
	return this->audioName;
}

const juce::String SeqSourceProcessor::getMIDIName() const {
	return this->midiName;
}

void SeqSourceProcessor::audioChanged() {
	this->audioSavedFlag = false;
}

void SeqSourceProcessor::midiChanged() {
	this->midiSavedFlag = false;
}

void SeqSourceProcessor::audioSaved() {
	this->audioSavedFlag = true;
}

void SeqSourceProcessor::midiSaved() {
	this->midiSavedFlag = true;
}

bool SeqSourceProcessor::isAudioSaved() const {
	return this->audioSavedFlag;
}

bool SeqSourceProcessor::isMIDISaved() const {
	return this->midiSavedFlag;
}

bool SeqSourceProcessor::isAudioValid() const {
	return (bool)this->audioData;
}

bool SeqSourceProcessor::isMIDIValid() const {
	return (bool)this->midiData;
}

void SeqSourceProcessor::setRecording(bool recording) {
	if (recording) {
		this->prepareRecord();
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
	if (!(this->instrOffline)) {
		this->juce::AudioProcessorGraph::processBlock(buffer, midiMessages);
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
		juce::String path = utils::getProjectDir()
			.getChildFile(mes->audiosrc()).getFullPathName();
		this->loadAudio(path);
	}
	if (!mes->midisrc().empty()) {
		juce::String path = utils::getProjectDir()
			.getChildFile(mes->midisrc()).getFullPathName();
		this->loadMIDI(path, false);
	}

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

	if (this->audioData) {
		juce::String name = this->getAudioFileName();
		juce::String path = utils::getProjectDir()
			.getChildFile(name).getFullPathName();

		mes->set_audiosrc(name.toStdString());
		this->saveAudio(path);
	}
	if (this->midiData) {
		juce::String name = this->getMIDIFileName();
		juce::String path = utils::getProjectDir()
			.getChildFile(name).getFullPathName();

		mes->set_midisrc(name.toStdString());
		this->saveMIDI(path);
	}

	mes->set_recording(this->getRecording());
	mes->set_muted(this->getMute());

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
			std::floor((time - baseTime / this->getSampleRate()) * this->getSampleRate()));
	}
}

void SeqSourceProcessor::writeAudioData(juce::AudioBuffer<float>& buffer, int offset) {
	/** Get Time */
	int srcLength = buffer.getNumSamples();
	int srcStartSample = offset;
	//int bufferStartSample = 0;
	/*if (srcStartSample < 0) {
		bufferStartSample -= srcStartSample;
		srcLength -= bufferStartSample;
		srcStartSample = 0;
	}*/

	/** Prepare Resampling */
	this->prepareAudioRecord();
	double resampleRatio = this->getSampleRate() / this->audioSampleRate;
	int channelNum = std::min(buffer.getNumChannels(), this->audioData->getNumChannels());

	/** Increase Source Length */
	int endLength = offset + buffer.getNumSamples();
	int trueEndLength = std::ceil(endLength / resampleRatio);
	if (trueEndLength > this->audioData->getNumSamples()) {
		this->audioData->setSize(this->audioData->getNumChannels(),
			this->audioData->getNumSamples() + this->recordInitLength * this->audioSampleRate,
			true, true, true);
		this->updateAudioResampler();
	}

	/** Copy Data Resampled */
	utils::bufferOutputResampledFixed(*(this->audioData.get()), buffer,
		this->recordBuffer, this->recordBufferTemp,
		resampleRatio, channelNum, this->audioSampleRate,
		0, srcStartSample, srcLength);

	/** Set Flag */
	this->audioChanged();
}

void SeqSourceProcessor::writeMIDIData(const juce::MidiBuffer& buffer, int offset) {
	/** Prepare Write */
	this->prepareMIDIRecord();

	/** Write To The Last Track */
	if (auto track = const_cast<juce::MidiMessageSequence*>(
		this->midiData->getTrack(this->midiData->getNumTracks() - 1))) {
		for (const auto& m : buffer) {
			double timeStamp = (m.samplePosition + offset) / this->getSampleRate();
			if (timeStamp >= 0) {
				auto mes = m.getMessage();
				mes.setTimeStamp(timeStamp);
				track->addEvent(mes);
			}
		}

		/** Set Flag */
		this->midiChanged();
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

void SeqSourceProcessor::prepareRecord() {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	this->prepareAudioRecord();
	this->prepareMIDIRecord();
}

void SeqSourceProcessor::prepareAudioRecord() {
	if (!this->audioData) {
		this->prepareAudioData(this->recordInitLength);
	}
}

void SeqSourceProcessor::prepareMIDIRecord() {
	if ((!this->midiData) || (this->midiData->getNumTracks() <= 0)) {
		this->prepareMIDIData();
	}
}

void SeqSourceProcessor::prepareAudioData(double length) {
	double sampleRate = 0;

	{
		juce::ScopedReadLock locker(audioLock::getSourceLock());
		sampleRate = this->audioSampleRate;
		if (sampleRate <= 0) {
			sampleRate = this->getSampleRate();
		}
	}

	this->initAudio(sampleRate, length);
}

void SeqSourceProcessor::prepareMIDIData() {
	this->initMIDI();
}

void SeqSourceProcessor::linkInstr() {
	if (auto ptrNode = this->instr) {
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
	}
}

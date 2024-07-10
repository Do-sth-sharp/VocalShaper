#include "SourceManager.h"
#include "../AudioConfig.h"
#include "../misc/AudioLock.h"
#include "../misc/VMath.h"
#include "../Utils.h"

uint64_t SourceManager::applySource(SourceType type) {
	/** Create Item */
	auto ptr = std::make_shared<SourceItem>();
	ptr->type = type;
	
	/** Get Reference */
	auto ref = reinterpret_cast<uint64_t>(ptr.get());

	/** Insert List */
	this->sources.insert(std::make_pair(ref, ptr));

	/** Return */
	return ref;
}

void SourceManager::releaseSource(uint64_t ref) {
	this->sources.erase(ref);
}

const juce::String SourceManager::getFileName(uint64_t ref, SourceType type) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, type)) {
		return ptr->fileName;
	}
	return "";
}

void SourceManager::changed(uint64_t ref, SourceType type) {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, type)) {
		ptr->changed();
	}
}

void SourceManager::saved(uint64_t ref, SourceType type) {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, type)) {
		ptr->saved();
	}
}

bool SourceManager::isSaved(uint64_t ref, SourceType type) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, type)) {
		return ptr->isSaved();
	}
	return true;
}

bool SourceManager::isValid(uint64_t ref, SourceType type) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, type)) {
		switch (type) {
		case SourceManager::SourceType::MIDI:
			return ptr->midiData != nullptr;
		case SourceManager::SourceType::Audio:
			return ptr->audioData != nullptr;
		}
	}
	return false;
}

int SourceManager::getMIDITrackNum(uint64_t ref) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock()); 
	
	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		if (auto data = ptr->midiData.get()) {
			return data->getNumTracks();
		}
	}
	return 0;
}

double SourceManager::getLength(uint64_t ref, SourceType type) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock()); 
	
	if (auto ptr = this->getSource(ref, type)) {
		switch (type) {
		case SourceManager::SourceType::MIDI:
			if (auto data = ptr->midiData.get()) {
				return ptr->midiData->getLastTimestamp() + AudioConfig::getMidiTail();
			}
			break;
		case SourceManager::SourceType::Audio:
			if (auto data = ptr->audioData.get()) {
				return ptr->audioData->getNumSamples() / ptr->audioSampleRate;
			}
			break;
		}
	}
	return 0;
}

void SourceManager::initAudio(uint64_t ref, int channelNum, double sampleRate, double length,
	int blockSize, double playSampleRate) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		ptr->initAudio(channelNum, sampleRate, length,
			blockSize, playSampleRate);
	}
}

void SourceManager::initMIDI(uint64_t ref) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		ptr->initMIDI();
	}
}

void SourceManager::setAudio(uint64_t ref, double sampleRate, const juce::AudioSampleBuffer& data, const juce::String& name,
	int blockSize, double playSampleRate) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		ptr->setAudio(sampleRate, data, name,
			blockSize, playSampleRate);
	}
}

void SourceManager::setMIDI(uint64_t ref, const juce::MidiFile& data, const juce::String& name) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		ptr->setMIDI(data, name);
	}
}

const std::tuple<double, juce::AudioSampleBuffer> SourceManager::getAudio(uint64_t ref) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		return ptr->getAudio();
	}

	return { 0, juce::AudioSampleBuffer {} };
}

const juce::MidiMessageSequence SourceManager::getMIDI(uint64_t ref, int trackIndex) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		return ptr->getMIDI(trackIndex);
	}

	return {};
}

const juce::MidiFile SourceManager::getMIDIFile(uint64_t ref) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		return ptr->getMIDIFile();
	}

	return {};
}

void SourceManager::prepareAudioPlay(uint64_t ref, int blockSize, double sampleRate) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		ptr->prepareAudioPlay(blockSize, sampleRate);
	}
}

void SourceManager::prepareMIDIPlay(uint64_t ref) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		ptr->prepareMIDIPlay();
	}
}

void SourceManager::prepareAudioRecord(uint64_t ref, int channelNum,
	int blockSize, double playSampleRate) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		ptr->prepareAudioRecord(channelNum, blockSize, playSampleRate);
	}
}

void SourceManager::prepareMIDIRecord(uint64_t ref) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		ptr->prepareMIDIRecord();
	}
}

void SourceManager::readAudioData(uint64_t ref, juce::AudioBuffer<float>& buffer, int bufferOffset,
	int dataOffset, int length) const {
	if (auto ptr = this->getSourceFast(ref, SourceType::Audio)) {
		ptr->readAudioData(buffer, bufferOffset, dataOffset, length);
	}
}

void SourceManager::readMIDIData(uint64_t ref, juce::MidiBuffer& buffer, double baseTime,
	double startTime, double endTime, int trackIndex, double sampleRate) const {
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		ptr->readMIDIData(buffer, baseTime, startTime, endTime, trackIndex, sampleRate);
	}
}

void SourceManager::writeAudioData(uint64_t ref, juce::AudioBuffer<float>& buffer, int offset,
	int trackChannelNum, int blockSize, double sampleRate) {
	if (auto ptr = this->getSourceFast(ref, SourceType::Audio)) {
		ptr->writeAudioData(buffer, offset, trackChannelNum, blockSize, sampleRate);
	}
}

void SourceManager::writeMIDIData(uint64_t ref, const juce::MidiBuffer& buffer, int offset, int trackIndex, double sampleRate) {
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		ptr->writeMIDIData(buffer, offset, trackIndex, sampleRate);
	}
}

void SourceManager::sampleRateChanged(double sampleRate, int blockSize) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	
	this->sampleRate = sampleRate;
	this->blockSize = blockSize;
	
	for (auto it = this->sources.begin(); it != this->sources.end(); it++) {
		auto srcPtr = it->second.get();
		/** TODO Change Sample Rate */
	}
}

void SourceManager::SourceItem::initAudio(
	int channelNum, double sampleRate, double length,
	int blockSize, double playSampleRate) {
	/** Clear Audio Source */
	this->resampleSource = nullptr;
	this->memSource = nullptr;
	this->audioData = nullptr;

	/** Create Buffer */
	this->audioData = std::make_unique<juce::AudioSampleBuffer>(
		channelNum, (int)std::ceil(length * sampleRate));
	vMath::zeroAllAudioData(*(this->audioData.get()));
	this->audioSampleRate = sampleRate;

	/** Update Resample Source */
	this->updateAudioResampler(blockSize, playSampleRate);

	/** Update Name */
	this->fileName = juce::String{};

	/** Set Flag */
	this->changed();
}

void SourceManager::SourceItem::initMIDI() {
	/** Create MIDI Data */
	this->midiData = std::make_unique<juce::MidiFile>();
	this->midiData->addTrack(juce::MidiMessageSequence{});

	/** Update Name */
	this->fileName = juce::String{};

	/** Set Flag */
	this->changed();
}

void SourceManager::SourceItem::setAudio(
	double sampleRate, const juce::AudioSampleBuffer& data, const juce::String& name,
	int blockSize, double playSampleRate) {
	/** Clear Audio Source */
	this->resampleSource = nullptr;
	this->memSource = nullptr;
	this->audioData = nullptr;

	/** Create Buffer */
	this->audioData = std::make_unique<juce::AudioSampleBuffer>(data);
	this->audioSampleRate = sampleRate;
	this->fileName = name;

	/** Update Resample Source */
	this->updateAudioResampler(blockSize, playSampleRate);

	/** Set Flag */
	this->changed();
}

void SourceManager::SourceItem::setMIDI(
	const juce::MidiFile& data, const juce::String& name) {
	/** Create MIDI Data */
	this->midiData = std::make_unique<juce::MidiFile>(data);
	this->fileName = name;

	/** Set Flag */
	this->changed();
}

const std::tuple<double, juce::AudioSampleBuffer> SourceManager::SourceItem::getAudio() const {
	/** Check Data */
	if (!this->audioData) {
		return { 0, juce::AudioSampleBuffer{} };
	}

	/** Copy Data */
	return { this->audioSampleRate, *(this->audioData.get()) };
}

const juce::MidiMessageSequence SourceManager::SourceItem::getMIDI(int trackIndex) const {
	/** Check Data */
	if (this->midiData) {
		if (auto track = this->midiData->getTrack(trackIndex)) {
			return juce::MidiMessageSequence{ *track };
		}
	}

	/** Default Result */
	return juce::MidiMessageSequence{};
}

const juce::MidiFile SourceManager::SourceItem::getMIDIFile() const {
	/** Check Data */
	if (!this->midiData) {
		return juce::MidiFile{};
	}

	/** Copy Data */
	return *(this->midiData.get());
}

void SourceManager::SourceItem::changed() {
	this->savedFlag = false;
}

void SourceManager::SourceItem::saved() {
	this->savedFlag = true;
}

bool SourceManager::SourceItem::isSaved() const {
	return this->savedFlag;
}

void SourceManager::SourceItem::prepareAudioPlay(int blockSize, double sampleRate) {
	if (this->resampleSource) {
		this->resampleSource->setResamplingRatio(this->audioSampleRate / sampleRate);
		this->resampleSource->prepareToPlay(blockSize, sampleRate);
	}
}

void SourceManager::SourceItem::prepareMIDIPlay() {
	/** Nothing To Do */
}

void SourceManager::SourceItem::prepareAudioRecord(int channelNum,
	int blockSize, double playSampleRate) {
	if (!this->audioData) {
		this->prepareAudioData(this->recordInitLength, channelNum, blockSize, playSampleRate);
	}
}

void SourceManager::SourceItem::prepareMIDIRecord() {
	if ((!this->midiData) || (this->midiData->getNumTracks() <= 0)) {
		this->prepareMIDIData();
	}
}

void SourceManager::SourceItem::readAudioData(
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

void SourceManager::SourceItem::readMIDIData(
	juce::MidiBuffer& buffer, double baseTime,
	double startTime, double endTime, int trackIndex, double sampleRate) const {
	/** Check Source */
	if (!this->midiData) { return; }

	/** Get MIDI Data */
	juce::MidiMessageSequence total;
	if (auto track = this->midiData->getTrack(trackIndex)) {
		total.addSequence(*track, 0, startTime, endTime);
	}

	/** Copy Data */
	for (auto i : total) {
		auto& message = i->message;
		double time = message.getTimeStamp();
		buffer.addEvent(message,
			std::floor((time - baseTime) * sampleRate));
	}
}

void SourceManager::SourceItem::writeAudioData(
	juce::AudioBuffer<float>& buffer, int offset,
	int trackChannelNum, int blockSize, double sampleRate) {
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
	this->prepareAudioRecord(trackChannelNum, blockSize, sampleRate);
	double resampleRatio = sampleRate / this->audioSampleRate;
	int channelNum = std::min(buffer.getNumChannels(), this->audioData->getNumChannels());

	/** Increase Source Length */
	int endLength = offset + buffer.getNumSamples();
	int trueEndLength = std::ceil(endLength / resampleRatio);
	if (trueEndLength > this->audioData->getNumSamples()) {
		this->audioData->setSize(this->audioData->getNumChannels(),
			this->audioData->getNumSamples() + this->recordInitLength * this->audioSampleRate,
			true, true, true);
		this->updateAudioResampler(blockSize, sampleRate);
	}

	/** Copy Data Resampled */
	utils::bufferOutputResampledFixed(*(this->audioData.get()), buffer,
		this->recordBuffer, this->recordBufferTemp,
		resampleRatio, channelNum, this->audioSampleRate,
		0, srcStartSample, srcLength);

	/** Set Flag */
	this->changed();
}

void SourceManager::SourceItem::writeMIDIData(
	const juce::MidiBuffer& buffer, int offset, int trackIndex, double sampleRate) {
	/** Prepare Write */
	this->prepareMIDIRecord();

	/** Write To The Last Track */
	if (auto track = const_cast<juce::MidiMessageSequence*>(
		this->midiData->getTrack(trackIndex))) {
		for (const auto& m : buffer) {
			double timeStamp = (m.samplePosition + offset) / sampleRate;
			if (timeStamp >= 0) {
				auto mes = m.getMessage();
				mes.setTimeStamp(timeStamp);
				track->addEvent(mes);
			}
		}

		/** Set Flag */
		this->changed();
	}
}

void SourceManager::SourceItem::updateAudioResampler(
	int blockSize, double sampleRate) {
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
	resSource->setResamplingRatio(this->audioSampleRate / sampleRate);
	resSource->prepareToPlay(blockSize, sampleRate);

	/** Set Resample Source */
	this->resampleSource = std::move(resSource);
}

void SourceManager::SourceItem::prepareAudioData(double length, int channelNum,
	int blockSize, double playSampleRate) {
	double sampleRate = this->audioSampleRate;
	if (sampleRate <= 0) {
		sampleRate = playSampleRate;
	}

	this->initAudio(channelNum, sampleRate, length, blockSize, playSampleRate);
}

void SourceManager::SourceItem::prepareMIDIData() {
	this->initMIDI();
}

SourceManager::SourceItem* 
SourceManager::getSource(uint64_t ref, SourceType type) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (ref == 0) { return nullptr; }

	auto it = this->sources.find(ref);
	if (it != this->sources.end()) {
		if (it->second->type == type) {
			return it->second.get();
		}
	}

	return nullptr;
}

SourceManager::SourceItem*
SourceManager::getSourceFast(uint64_t ref, SourceType type) const {
	if (ref == 0) { return nullptr; }

	if (auto ptr = reinterpret_cast<SourceManager::SourceItem*>(ref)) {
		if (ptr->type == type) {
			return ptr;
		}
	}

	return nullptr;
}

SourceManager* SourceManager::getInstance() {
	return SourceManager::instance ? SourceManager::instance 
		: (SourceManager::instance = new SourceManager{});
}

void SourceManager::releaseInstance() {
	if (SourceManager::instance) {
		delete SourceManager::instance;
		SourceManager::instance = nullptr;
	}
}

SourceManager* SourceManager::instance = nullptr;

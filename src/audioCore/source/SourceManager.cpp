#include "SourceManager.h"
#include "../misc/AudioLock.h"

uint64_t SourceManager::applySource(SourceType type) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Create Item */
	auto ptr = std::make_shared<SourceItem>(type);
	ptr->setSampleRate(this->blockSize, this->sampleRate);
	
	/** Get Reference */
	auto ref = reinterpret_cast<uint64_t>(ptr.get());

	/** Insert List */
	this->sources.insert(std::make_pair(ref, ptr));

	/** Return */
	return ref;
}

void SourceManager::releaseSource(uint64_t ref) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	this->sources.erase(ref);
}

const juce::String SourceManager::getFileName(uint64_t ref, SourceType type) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, type)) {
		return ptr->getFileName();
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
			return ptr->midiValid();
		case SourceManager::SourceType::Audio:
			return ptr->audioValid();
		}
	}
	return false;
}

int SourceManager::getMIDITrackNum(uint64_t ref) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock()); 
	
	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		return ptr->getMIDITrackNum();
	}
	return 0;
}

double SourceManager::getLength(uint64_t ref, SourceType type) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock()); 
	
	if (auto ptr = this->getSource(ref, type)) {
		switch (type) {
		case SourceManager::SourceType::MIDI:
			return ptr->getMIDILength();
		case SourceManager::SourceType::Audio:
			return ptr->getAudioLength();
		}
	}
	return 0;
}

void SourceManager::initAudio(uint64_t ref, int channelNum, double sampleRate, double length) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		ptr->initAudio(channelNum, sampleRate, length);
	}
}

void SourceManager::initMIDI(uint64_t ref) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		ptr->initMIDI();
	}
}

void SourceManager::setAudio(uint64_t ref, double sampleRate, const juce::AudioSampleBuffer& data, const juce::String& name) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		ptr->setAudio(sampleRate, data, name);
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

void SourceManager::prepareAudioPlay(uint64_t ref) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		ptr->prepareAudioPlay();
	}
}

void SourceManager::prepareMIDIPlay(uint64_t ref) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		ptr->prepareMIDIPlay();
	}
}

void SourceManager::prepareAudioRecord(uint64_t ref, int channelNum) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		ptr->prepareAudioRecord(channelNum);
	}
}

void SourceManager::prepareMIDIRecord(uint64_t ref) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		ptr->prepareMIDIRecord();
	}
}

void SourceManager::setCallback(
	uint64_t ref, SourceType type,
	const ChangedCallback& callback) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, type)) {
		ptr->setCallback(callback);
	}
}

void SourceManager::readAudioData(uint64_t ref, juce::AudioBuffer<float>& buffer, int bufferOffset,
	int dataOffset, int length) const {
	if (auto ptr = this->getSourceFast(ref, SourceType::Audio)) {
		ptr->readAudioData(buffer, bufferOffset, dataOffset, length);
	}
}

void SourceManager::readMIDIData(uint64_t ref, juce::MidiBuffer& buffer, double baseTime,
	double startTime, double endTime, int trackIndex) const {
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		ptr->readMIDIData(buffer, baseTime, startTime, endTime, trackIndex);
	}
}

void SourceManager::writeAudioData(uint64_t ref, juce::AudioBuffer<float>& buffer, int offset,
	int trackChannelNum) {
	if (auto ptr = this->getSourceFast(ref, SourceType::Audio)) {
		ptr->writeAudioData(buffer, offset, trackChannelNum);
	}
}

void SourceManager::writeMIDIData(uint64_t ref, const juce::MidiBuffer& buffer, int offset, int trackIndex) {
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		ptr->writeMIDIData(buffer, offset, trackIndex);
	}
}

void SourceManager::sampleRateChanged(double sampleRate, int blockSize) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	
	this->sampleRate = sampleRate;
	this->blockSize = blockSize;
	
	for (auto it = this->sources.begin(); it != this->sources.end(); it++) {
		auto srcPtr = it->second.get();
		srcPtr->setSampleRate(blockSize, sampleRate);
	}
}

SourceItem* SourceManager::getSource(uint64_t ref, SourceType type) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (ref == 0) { return nullptr; }

	auto it = this->sources.find(ref);
	if (it != this->sources.end()) {
		if (it->second->getType() == type) {
			return it->second.get();
		}
	}

	return nullptr;
}

SourceItem* SourceManager::getSourceFast(uint64_t ref, SourceType type) const {
	if (ref == 0) { return nullptr; }

	if (auto ptr = reinterpret_cast<SourceItem*>(ref)) {
		if (ptr->getType() == type) {
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

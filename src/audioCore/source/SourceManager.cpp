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
		default:
			break;
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

bool SourceManager::isMIDITrackEmpty(uint64_t ref, int track) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		return ptr->isMIDITrackEmpty(track);
	}
	return true;
}

double SourceManager::getLength(uint64_t ref, SourceType type) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock()); 
	
	if (auto ptr = this->getSource(ref, type)) {
		switch (type) {
		case SourceManager::SourceType::MIDI:
			return ptr->getMIDILength();
		case SourceManager::SourceType::Audio:
			return ptr->getAudioLength();
		default:
			break;
		}
	}
	return 0;
}

void SourceManager::initAudio(uint64_t ref, const juce::String& name,
	int channelNum, double sampleRate, double length) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		ptr->initAudio(name, channelNum, sampleRate, length);
	}
}

void SourceManager::initMIDI(uint64_t ref, const juce::String& name) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		ptr->initMIDI(name);
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

void SourceManager::setAudio(uint64_t ref, const juce::String& name) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		ptr->setAudio(name);
	}
}

void SourceManager::setMIDI(uint64_t ref, const juce::String& name) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		ptr->setMIDI(name);
	}
}

const std::tuple<double, juce::AudioSampleBuffer> SourceManager::getAudio(uint64_t ref) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		return ptr->getAudio();
	}

	return { 0, juce::AudioSampleBuffer {} };
}

const juce::MidiMessageSequence SourceManager::makeMIDITrack(uint64_t ref, int trackIndex) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		return ptr->makeMIDITrack(trackIndex);
	}

	return {};
}

const juce::MidiFile SourceManager::makeMIDIFile(uint64_t ref) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		return ptr->makeMIDIFile();
	}

	return {};
}

void SourceManager::writeAudio(uint64_t ref,
	AudioWriteType type, const juce::AudioSampleBuffer& buffer,
	double startTime, double length, double sampleRate) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		ptr->writeAudio(type, buffer, startTime, length, sampleRate);
	}
}

void SourceManager::writeMIDI(uint64_t ref,
	MIDIWriteType type, const juce::MidiMessageSequence& sequence,
	double startTime, double length, int track) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, SourceType::MIDI)) {
		ptr->writeMIDI(type, sequence, startTime, length, track);
	}
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

void SourceManager::setCallback(
	uint64_t ref, SourceType type,
	const ChangedCallback& callback) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, type)) {
		ptr->setCallback(callback);
	}
}

void SourceManager::setAudioFormat(uint64_t ref, const AudioFormat& format) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		ptr->setAudioFormat(format);
	}
}

const SourceManager::AudioFormat SourceManager::getAudioFormat(uint64_t ref) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		return ptr->getAudioFormat();
	}
	return AudioFormat{};
}

double SourceManager::getAudioSampleRate(uint64_t ref) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSource(ref, SourceType::Audio)) {
		return ptr->getAudioSampleRate();
	}
	return 0;
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

int SourceManager::getMIDINoteNum(uint64_t ref, int track) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->getMIDINoteNum(track);
	}
	return 0;
}

int SourceManager::getMIDIPitchWheelNum(uint64_t ref, int track) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->getMIDIPitchWheelNum(track);
	}
	return 0;
}

int SourceManager::getMIDIAfterTouchNum(uint64_t ref, int track) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->getMIDIAfterTouchNum(track);
	}
	return 0;
}

int SourceManager::getMIDIChannelPressureNum(uint64_t ref, int track) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->getMIDIChannelPressureNum(track);
	}
	return 0;
}

const std::set<uint8_t> SourceManager::getMIDIControllerNumbers(uint64_t ref, int track) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->getMIDIControllerNumbers(track);
	}
	return {};
}

int SourceManager::getMIDIControllerNum(uint64_t ref, int track, uint8_t number) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->getMIDIControllerNum(track, number);
	}
	return 0;
}

int SourceManager::getMIDIMiscNum(uint64_t ref, int track) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->getMIDIMiscNum(track);
	}
	return 0;
}

const SourceMIDITemp::Note SourceManager::getMIDINote(uint64_t ref, int track, int index) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->getMIDINote(track, index);
	}
	return {};
}

const SourceMIDITemp::IntParam SourceManager::getMIDIPitchWheel(uint64_t ref, int track, int index) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->getMIDIPitchWheel(track, index);
	}
	return {};
}

const SourceMIDITemp::AfterTouch SourceManager::getMIDIAfterTouch(uint64_t ref, int track, int index) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->getMIDIAfterTouch(track, index);
	}
	return {};
}

const SourceMIDITemp::IntParam SourceManager::getMIDIChannelPressure(uint64_t ref, int track, int index) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->getMIDIChannelPressure(track, index);
	}
	return {};
}

const SourceMIDITemp::Controller SourceManager::getMIDIController(uint64_t ref, int track, uint8_t number, int index) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->getMIDIController(track, number, index);
	}
	return {};
}

const SourceMIDITemp::Misc SourceManager::getMIDIMisc(uint64_t ref, int track, int index) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->getMIDIMisc(track, index);
	}
	return {};
}

const juce::Array<SourceMIDITemp::Note> SourceManager::getMIDINoteList(uint64_t ref, int track) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		int num = ptr->getMIDINoteNum(track);
		juce::Array<SourceMIDITemp::Note> list;
		list.ensureStorageAllocated(num);

		for (int i = 0; i < num; i++) {
			list.add(ptr->getMIDINote(track, i));
		}

		return list;
	}
	return {};
}

const juce::Array<SourceMIDITemp::IntParam> SourceManager::getMIDIPitchWheelList(uint64_t ref, int track) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		int num = ptr->getMIDIPitchWheelNum(track);
		juce::Array<SourceMIDITemp::IntParam> list;
		list.ensureStorageAllocated(num);

		for (int i = 0; i < num; i++) {
			list.add(ptr->getMIDIPitchWheel(track, i));
		}

		return list;
	}
	return {};
}

const juce::Array<SourceMIDITemp::AfterTouch> SourceManager::getMIDIAfterTouchList(uint64_t ref, int track) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		int num = ptr->getMIDIAfterTouchNum(track);
		juce::Array<SourceMIDITemp::AfterTouch> list;
		list.ensureStorageAllocated(num);

		for (int i = 0; i < num; i++) {
			list.add(ptr->getMIDIAfterTouch(track, i));
		}

		return list;
	}
	return {};
}

const juce::Array<SourceMIDITemp::IntParam> SourceManager::getMIDIChannelPressureList(uint64_t ref, int track) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		int num = ptr->getMIDIChannelPressureNum(track);
		juce::Array<SourceMIDITemp::IntParam> list;
		list.ensureStorageAllocated(num);

		for (int i = 0; i < num; i++) {
			list.add(ptr->getMIDIChannelPressure(track, i));
		}

		return list;
	}
	return {};
}

const juce::Array<SourceMIDITemp::Controller> SourceManager::getMIDIControllerList(uint64_t ref, int track, uint8_t number) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		int num = ptr->getMIDIControllerNum(track, number);
		juce::Array<SourceMIDITemp::Controller> list;
		list.ensureStorageAllocated(num);

		for (int i = 0; i < num; i++) {
			list.add(ptr->getMIDIController(track, number, i));
		}

		return list;
	}
	return {};
}

const juce::Array<SourceMIDITemp::Misc> SourceManager::getMIDIMiscList(uint64_t ref, int track) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		int num = ptr->getMIDIMiscNum(track);
		juce::Array<SourceMIDITemp::Misc> list;
		list.ensureStorageAllocated(num);

		for (int i = 0; i < num; i++) {
			list.add(ptr->getMIDIMisc(track, i));
		}

		return list;
	}
	return {};
}

int SourceManager::addNote(uint64_t ref,
	int track, double startTime, double endTime, uint8_t channel,
	uint8_t pitch, uint8_t vel, const juce::String& lyrics,
	int oldIndex, int oldNoteOffIndex) {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->addNote(
			track, startTime, endTime,
			channel, pitch, vel, lyrics,
			oldIndex, oldNoteOffIndex);
	}
	return -1;
}

int SourceManager::setNoteTime(uint64_t ref, int track, int index,
	double startTime, double endTime, int oldIndex, int oldNoteOffIndex) {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->setNoteTime(
			track, index, startTime, endTime, oldIndex, oldNoteOffIndex);
	}
	return -1;
}

bool SourceManager::setNoteChannel(uint64_t ref, int track, int index, uint8_t channel) {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->setNoteChannel(
			track, index, channel);
	}
	return false;
}

bool SourceManager::setNotePitch(uint64_t ref, int track, int index, uint8_t pitch) {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->setNotePitch(
			track, index, pitch);
	}
	return false;
}

bool SourceManager::setNoteVelocity(uint64_t ref, int track, int index, uint8_t vel) {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->setNoteVelocity(
			track, index, vel);
	}
	return false;
}

bool SourceManager::setNoteLyrics(uint64_t ref, int track, int index, const juce::String& lyrics) {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->setNoteLyrics(
			track, index, lyrics);
	}
	return false;
}

bool SourceManager::removeNote(uint64_t ref, int track, int index) {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	if (auto ptr = this->getSourceFast(ref, SourceType::MIDI)) {
		return ptr->removeNote(track, index);
	}
	return false;
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

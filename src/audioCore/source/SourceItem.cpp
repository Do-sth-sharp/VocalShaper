#include "SourceItem.h"
#include "SourceInternalPool.h"
#include "../misc/VMath.h"
#include "../AudioConfig.h"
#include "../Utils.h"

SourceItem::SourceItem(SourceType type)
	: type(type) {}

SourceItem::~SourceItem() {
	this->releaseContainer();
}

void SourceItem::initAudio(
	const juce::String& name,
	int channelNum, double sampleRate, double length) {
	/** Check Type */
	if (this->type != SourceType::Audio) { return; }

	/** Clear Audio Source */
	this->resampleSource = nullptr;
	this->memSource = nullptr;

	/** Remove Old Source */
	this->releaseContainer();

	/** Create Audio Source */
	this->container = SourceInternalPool::getInstance()->create(name, this->type);
	if (this->container) {
		this->container->initAudioData(channelNum, sampleRate, length);
	}

	/** Update Resample Source */
	this->updateAudioResampler();

	/** Callback */
	this->invokeCallback();
}

void SourceItem::initMIDI(const juce::String& name) {
	/** Check Type */
	if (this->type != SourceType::MIDI) { return; }

	/** Remove Old Source */
	this->releaseContainer();

	/** Create MIDI Source */
	this->container = SourceInternalPool::getInstance()->create(name, this->type);
	if (this->container) {
		this->container->initMidiData();
	}

	/** Callback */
	this->invokeCallback();
}

void SourceItem::setAudio(
	double sampleRate, const juce::AudioSampleBuffer& data, const juce::String& name) {
	/** Check Type */
	if (this->type != SourceType::Audio) { return; }

	/** Clear Audio Source */
	this->resampleSource = nullptr;
	this->memSource = nullptr;

	/** Remove Old Source */
	this->releaseContainer();

	/** Create Audio Source */
	this->container = SourceInternalPool::getInstance()->add(name, this->type);
	if (this->container) {
		this->container->setAudio(sampleRate, data);
	}

	/** Update Resample Source */
	this->updateAudioResampler();

	/** Callback */
	this->invokeCallback();
}

void SourceItem::setMIDI(
	const juce::MidiFile& data, const juce::String& name) {
	/** Check Type */
	if (this->type != SourceType::MIDI) { return; }

	/** Remove Old Source */
	this->releaseContainer();

	/** Create MIDI Source */
	this->container = SourceInternalPool::getInstance()->add(name, this->type);
	if (this->container) {
		this->container->setMIDI(data);
	}

	/** Callback */
	this->invokeCallback();
}

void SourceItem::setAudio(const juce::String& name) {
	/** Check Type */
	if (this->type != SourceType::Audio) { return; }

	/** Clear Audio Source */
	this->resampleSource = nullptr;
	this->memSource = nullptr;

	/** Remove Old Source */
	this->releaseContainer();

	/** Get Audio Source */
	this->container = SourceInternalPool::getInstance()->add(name, this->type);

	/** Update Resample Source */
	this->updateAudioResampler();

	/** Callback */
	this->invokeCallback();
}

void SourceItem::setMIDI(const juce::String& name) {
	/** Check Type */
	if (this->type != SourceType::MIDI) { return; }

	/** Remove Old Source */
	this->releaseContainer();

	/** Create MIDI Source */
	this->container = SourceInternalPool::getInstance()->add(name, this->type);

	/** Callback */
	this->invokeCallback();
}

const std::tuple<double, juce::AudioSampleBuffer> SourceItem::getAudio() const {
	/** Check Data */
	if (!this->audioValid()) {
		return { 0, juce::AudioSampleBuffer{} };
	}

	/** Copy Data */
	return { this->container->getAudioSampleRate(), *(this->container->getAudioData()) };
}

const juce::MidiMessageSequence SourceItem::makeMIDITrack(int trackIndex) const {
	/** Check Data */
	if (!this->midiValid()) {
		return juce::MidiMessageSequence{};
	}

	/** Make Data */
	return this->container->makeMIDITrack(trackIndex);
}

const juce::MidiFile SourceItem::makeMIDIFile() const {
	/** Check Data */
	if (!this->midiValid()) {
		return juce::MidiFile{};
	}

	/** Make Data */
	return this->container->makeMIDIFile();
}

void SourceItem::writeAudio(AudioWriteType type, const juce::AudioSampleBuffer& buffer,
	double startTime, double length, double sampleRate) {
	/** Check Type */
	if (this->type != SourceType::Audio) { return; }

	/** Init or Fork */
	if (this->container) {
		this->forkIfNeed();
	}
	else {
		this->initAudio(juce::String{},
			buffer.getNumChannels(), sampleRate, startTime + length);
	}

	/** Write Data */
	this->container->writeAudio(type, buffer, startTime, length, sampleRate);

	/** Resampler */
	this->updateAudioResampler();

	/** Callback */
	this->invokeCallback();
}

void SourceItem::writeMIDI(MIDIWriteType type, const juce::MidiMessageSequence& sequence,
	double startTime, double length, int track) {
	/** Check Type */
	if (this->type != SourceType::MIDI) { return; }

	/** Init or Fork */
	if (this->container) {
		this->forkIfNeed();
	}
	else {
		this->initMIDI(juce::String{});
	}

	/** Write Data */
	this->container->writeMIDI(
		type, sequence, startTime, length, track);

	/** Callback */
	this->invokeCallback();
}

void SourceItem::changed() {
	if (!this->container) { return; }
	this->container->changed();
}

void SourceItem::saved() {
	if (!this->container) { return; }
	this->container->saved();
}

bool SourceItem::isSaved() const {
	if (!this->container) { return false; }

	return this->container->isSaved();
}

void SourceItem::prepareAudioPlay() {
	/** Check Data */
	if (!this->audioValid()) {
		return;
	}

	/** Update Resample Source */
	if (this->resampleSource) {
		this->resampleSource->setResamplingRatio(this->container->getAudioSampleRate() / this->playSampleRate);
		this->resampleSource->prepareToPlay(this->blockSize, this->playSampleRate);
	}
}

void SourceItem::prepareMIDIPlay() {
	/** Check Data */
	if (!this->midiValid()) {
		return;
	}

	/** Nothing To Do */
}

void SourceItem::setSampleRate(int blockSize, double sampleRate) {
	this->playSampleRate = sampleRate;
	this->blockSize = blockSize;

	this->updateAudioResampler();
}

SourceItem::SourceType SourceItem::getType() const {
	return this->type;
}

const juce::String SourceItem::getFilePath() const {
	if (!this->container) { return {}; }
	return this->container->getPath();
}

void SourceItem::setLastSavePath(const juce::String& path) {
	if (!this->container) { return; }
	return this->container->setLastSavePath(path);
}

bool SourceItem::midiValid() const {
	return !(this->type != SourceType::MIDI || !this->container);
}

bool SourceItem::audioValid() const {
	return !(this->type != SourceType::Audio || !this->container || !this->container->getAudioData());
}

int SourceItem::getMIDITrackNum() const {
	if (!this->midiValid()) { return 0; }

	return this->container->getMIDITrackNum();
}

bool SourceItem::isMIDITrackEmpty(int track) const {
	if (!this->midiValid()) { return true; }

	return this->container->isMIDITrackEmpty(track);
}

double SourceItem::getMIDILength() const {
	if (!this->midiValid()) { return 0; }

	return this->container->getMIDILength() + AudioConfig::getMidiTail();
}

double SourceItem::getAudioLength() const {
	if (!this->audioValid()) { return 0; }

	return this->container->getAudioData()->getNumSamples() / this->container->getAudioSampleRate();
}

void SourceItem::setCallback(const ChangedCallback& callback) {
	this->callback = callback;
}

void SourceItem::invokeCallback() const {
	if (this->callback) {
		juce::MessageManager::callAsync(this->callback);
	}
}

void SourceItem::setAudioFormat(const AudioFormat& format) {
	if (this->type != SourceType::Audio || !this->container) { return; }
	this->container->setAudioFormat(format);
}

const SourceItem::AudioFormat SourceItem::getAudioFormat() const {
	if (this->type != SourceType::Audio || !this->container) { return {}; }
	return this->container->getAudioFormat();
}

double SourceItem::getAudioSampleRate() const {
	if (this->type != SourceType::Audio || !this->container) { return 0; }
	return this->container->getAudioSampleRate();
}

void SourceItem::forkIfNeed() {
	if (this->container && this->container.use_count() > 2) {
		/** Clear Audio Source */
		if (this->type == SourceType::Audio) {
			this->resampleSource = nullptr;
			this->memSource = nullptr;
		}

		/** Fork Source */
		auto path = this->container->getPath();
		this->container = SourceInternalPool::getInstance()->fork(path);
		SourceInternalPool::getInstance()->checkSourceReleased(path);

		/** Update Resample Source */
		if (this->type == SourceType::Audio) {
			this->updateAudioResampler();
		}

		/** Callback */
		this->invokeCallback();
	}
}

void SourceItem::readAudioData(
	juce::AudioBuffer<float>& buffer, int bufferOffset,
	int dataOffset, int length) const {
	/** Check Source */
	if (!this->audioValid()) { return; }
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

void SourceItem::readMIDIData(
	juce::MidiBuffer& buffer, double baseTime,
	double startTime, double endTime, int trackIndex) const {
	/** Check Source */
	if (!this->midiValid()) { return; }

	/** Get MIDI Data */
	juce::MidiMessageSequence temp;
	this->container->findMIDIMessages(
		trackIndex, startTime, endTime, temp,
		this->playbackMIDIIndexTemp);

	/** Copy Data */
	for (auto i : temp) {
		auto& message = i->message;
		double time = message.getTimeStamp();
		buffer.addEvent(message,
			std::floor((time - baseTime) * this->playSampleRate));
	}
}

int SourceItem::getMIDINoteNum(int track) const {
	if (!this->container) { return 0; }
	return this->container->getMIDINoteNum(track);
}

int SourceItem::getMIDIPitchWheelNum(int track) const {
	if (!this->container) { return 0; }
	return this->container->getMIDIPitchWheelNum(track);
}

int SourceItem::getMIDIAfterTouchNum(int track) const {
	if (!this->container) { return 0; }
	return this->container->getMIDIAfterTouchNum(track);
}

int SourceItem::getMIDIChannelPressureNum(int track) const {
	if (!this->container) { return 0; }
	return this->container->getMIDIChannelPressureNum(track);
}

const std::set<uint8_t> SourceItem::getMIDIControllerNumbers(int track) const {
	if (!this->container) { return {}; }
	return this->container->getMIDIControllerNumbers(track);
}

int SourceItem::getMIDIControllerNum(int track, uint8_t number) const {
	if (!this->container) { return 0; }
	return this->container->getMIDIControllerNum(track, number);
}

int SourceItem::getMIDIMiscNum(int track) const {
	if (!this->container) { return 0; }
	return this->container->getMIDIMiscNum(track);
}

const SourceMIDITemp::Note SourceItem::getMIDINote(int track, int index) const {
	if (!this->container) { return {}; }
	return this->container->getMIDINote(track, index);
}

const SourceMIDITemp::IntParam SourceItem::getMIDIPitchWheel(int track, int index) const {
	if (!this->container) { return {}; }
	return this->container->getMIDIPitchWheel(track, index);
}

const SourceMIDITemp::AfterTouch SourceItem::getMIDIAfterTouch(int track, int index) const {
	if (!this->container) { return {}; }
	return this->container->getMIDIAfterTouch(track, index);
}

const SourceMIDITemp::IntParam SourceItem::getMIDIChannelPressure(int track, int index) const {
	if (!this->container) { return {}; }
	return this->container->getMIDIChannelPressure(track, index);
}

const SourceMIDITemp::Controller SourceItem::getMIDIController(int track, uint8_t number, int index) const {
	if (!this->container) { return {}; }
	return this->container->getMIDIController(track, number, index);
}

const SourceMIDITemp::Misc SourceItem::getMIDIMisc(int track, int index) const {
	if (!this->container) { return {}; }
	return this->container->getMIDIMisc(track, index);
}

int SourceItem::addNote(
	int track, double startTime, double endTime, uint8_t channel,
	uint8_t pitch, uint8_t vel, const juce::String& lyrics,
	int oldIndex, int oldNoteOffIndex) {
	if (!this->container) { return -1; }

	int result = this->container->addNote(
		track, startTime, endTime, channel,
		pitch, vel, lyrics, oldIndex, oldNoteOffIndex);

	if (result >= 0) {
		this->invokeCallback();
	}
	return result;
}

int SourceItem::setNoteTime(int track, int index,
	double startTime, double endTime, int oldIndex, int oldNoteOffIndex) {
	if (!this->container) { return -1; }

	int result = this->container->setNoteTime(
		track, index, startTime, endTime, oldIndex, oldNoteOffIndex);

	if (result >= 0) {
		this->invokeCallback();
	}
	return result;
}

bool SourceItem::setNoteChannel(int track, int index, uint8_t channel) {
	if (!this->container) { return false; }

	bool result = this->container->setNoteChannel(
		track, index, channel);

	if (result) {
		this->invokeCallback();
	}
	return result;
}

bool SourceItem::setNotePitch(int track, int index, uint8_t pitch) {
	if (!this->container) { return false; }

	bool result = this->container->setNotePitch(
		track, index, pitch);

	if (result) {
		this->invokeCallback();
	}
	return result;
}

bool SourceItem::setNoteVelocity(int track, int index, uint8_t vel) {
	if (!this->container) { return false; }

	bool result = this->container->setNoteVelocity(
		track, index, vel);

	if (result) {
		this->invokeCallback();
	}
	return result;
}

bool SourceItem::setNoteLyrics(int track, int index, const juce::String& lyrics) {
	if (!this->container) { return false; }

	bool result = this->container->setNoteLyrics(
		track, index, lyrics);

	if (result) {
		this->invokeCallback();
	}
	return result;
}

bool SourceItem::removeNote(int track, int index) {
	if (!this->container) { return false; }

	bool result = this->container->removeNote(
		track, index);

	if (result) {
		this->invokeCallback();
	}
	return result;
}

void SourceItem::updateAudioResampler() {
	/** Check Audio Data */
	if (!this->audioValid()) { return; }

	/** Remove Resample Source */
	this->resampleSource = nullptr;

	/** Create Audio Source */
	auto audioData = this->container->getAudioData();
	this->memSource = std::make_unique<juce::MemoryAudioSource>(
		*(audioData), false, false);
	auto resSource = std::make_unique<juce::ResamplingAudioSource>(
		this->memSource.get(), false, audioData->getNumChannels());

	/** Set Sample Rate */
	resSource->setResamplingRatio(this->container->getAudioSampleRate() / this->playSampleRate);
	resSource->prepareToPlay(this->blockSize, this->playSampleRate);

	/** Set Resample Source */
	this->resampleSource = std::move(resSource);
}

void SourceItem::releaseContainer() {
	if (this->container) {
		auto path = this->container->getPath();
		this->container = nullptr;
		SourceInternalPool::getInstance()->checkSourceReleased(path);
	}
}

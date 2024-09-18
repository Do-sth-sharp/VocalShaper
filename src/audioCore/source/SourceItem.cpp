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
	int channelNum, double sampleRate, double length) {
	/** Check Type */
	if (this->type != SourceType::Audio) { return; }

	/** Clear Audio Source */
	this->resampleSource = nullptr;
	this->memSource = nullptr;

	/** Remove Old Source */
	this->releaseContainer();

	/** Create Audio Source */
	this->container = SourceInternalPool::getInstance()->create(this->type);
	if (this->container) {
		this->container->initAudioData(channelNum, sampleRate, length);
	}

	/** Update Resample Source */
	this->updateAudioResampler();

	/** Callback */
	this->invokeCallback();
}

void SourceItem::initMIDI() {
	/** Check Type */
	if (this->type != SourceType::MIDI) { return; }

	/** Remove Old Source */
	this->releaseContainer();

	/** Create MIDI Source */
	this->container = SourceInternalPool::getInstance()->create(this->type);
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

const juce::MidiMessageSequence SourceItem::getMIDI(int trackIndex) const {
	/** Check Data */
	if (!this->midiValid()) {
		return juce::MidiMessageSequence{};
	}

	/** Copy Data */
	if (auto track = this->container->getMidiData()->getTrack(trackIndex)) {
		return juce::MidiMessageSequence{ *track };
	}

	return juce::MidiMessageSequence{};
}

const juce::MidiFile SourceItem::getMIDIFile() const {
	/** Check Data */
	if (!this->midiValid()) {
		return juce::MidiFile{};
	}

	/** Copy Data */
	return *(this->container->getMidiData());
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

void SourceItem::prepareAudioRecord(int channelNum) {
	/** Check Data */
	if (this->type != SourceType::Audio) {
		return;
	}

	/** Init Audio */
	if (!this->container || !this->container->getAudioData()) {
		this->prepareAudioData(this->recordInitLength, channelNum);
	}

	/** Check For Fork */
	this->forkIfNeed();
}

void SourceItem::prepareMIDIRecord() {
	/** Check Data */
	if (this->type != SourceType::MIDI) {
		return;
	}

	/** Init MIDI */
	if (!this->container || !this->container->getMidiData()
		|| this->container->getMidiData()->getNumTracks() == 0) {
		this->prepareMIDIData();
	}

	/** Check For Fork */
	this->forkIfNeed();
}

void SourceItem::setSampleRate(int blockSize, double sampleRate) {
	this->playSampleRate = sampleRate;
	this->blockSize = blockSize;

	this->updateAudioResampler();
}

SourceItem::SourceType SourceItem::getType() const {
	return this->type;
}

const juce::String SourceItem::getFileName() const {
	if (!this->container) { return {}; }
	return this->container->getName();
}

bool SourceItem::midiValid() const {
	return !(this->type != SourceType::MIDI || !this->container || !this->container->getMidiData());
}

bool SourceItem::audioValid() const {
	return !(this->type != SourceType::Audio || !this->container || !this->container->getAudioData());
}

int SourceItem::getMIDITrackNum() const {
	if (!this->midiValid()) { return 0; }

	return this->container->getMidiData()->getNumTracks();
}

double SourceItem::getMIDILength() const {
	if (!this->midiValid()) { return 0; }

	return this->container->getMidiData()->getLastTimestamp() + AudioConfig::getMidiTail();
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

void SourceItem::forkIfNeed() {
	if (this->container && this->container.use_count() > 2) {
		/** Clear Audio Source */
		if (this->type == SourceType::Audio) {
			this->resampleSource = nullptr;
			this->memSource = nullptr;
		}

		/** Fork Source */
		auto name = this->container->getName();
		this->container = SourceInternalPool::getInstance()->fork(name);
		SourceInternalPool::getInstance()->checkSourceReleased(name);

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
	juce::MidiMessageSequence total;
	if (auto track = this->container->getMidiData()->getTrack(trackIndex)) {
		total.addSequence(*track, 0, startTime, endTime);
	}

	/** Copy Data */
	for (auto i : total) {
		auto& message = i->message;
		double time = message.getTimeStamp();
		buffer.addEvent(message,
			std::floor((time - baseTime) * this->playSampleRate));
	}
}

void SourceItem::writeAudioData(
	juce::AudioBuffer<float>& buffer, int offset,
	int trackChannelNum) {
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
	this->prepareAudioRecord(trackChannelNum);
	if (!this->audioValid()) { return; }
	auto audioData = this->container->getAudioData();
	double audioSampleRate = this->container->getAudioSampleRate();

	double resampleRatio = this->playSampleRate / audioSampleRate;
	int channelNum = std::min(buffer.getNumChannels(), audioData->getNumChannels());

	/** Increase Source Length */
	int endLength = offset + buffer.getNumSamples();
	int trueEndLength = std::ceil(endLength / resampleRatio);
	if (trueEndLength > audioData->getNumSamples()) {
		audioData->setSize(audioData->getNumChannels(),
			audioData->getNumSamples() + this->recordInitLength * audioSampleRate,
			true, true, true);
		this->updateAudioResampler();
	}

	/** Copy Data Resampled */
	utils::bufferOutputResampledFixed(*(audioData), buffer,
		this->recordBuffer, this->recordBufferTemp,
		resampleRatio, channelNum, audioSampleRate,
		0, srcStartSample, srcLength);

	/** Set Flag */
	this->container->changed();
}

void SourceItem::writeMIDIData(
	const juce::MidiBuffer& buffer, int offset, int trackIndex) {
	/** Prepare Write */
	this->prepareMIDIRecord();
	if (!this->midiValid()) { return; }

	/** Write To The Last Track */
	if (auto track = const_cast<juce::MidiMessageSequence*>(
		this->container->getMidiData()->getTrack(trackIndex))) {
		for (const auto& m : buffer) {
			double timeStamp = (m.samplePosition + offset) / this->playSampleRate;
			if (timeStamp >= 0) {
				auto mes = m.getMessage();
				mes.setTimeStamp(timeStamp);
				track->addEvent(mes);
			}
		}

		/** Set Flag */
		this->container->changed();
	}
}

int SourceItem::getMIDINoteNum(int track) const {
	if (!this->container) { return 0; }
	return this->container->getMIDINoteNum(track);
}

int SourceItem::getMIDISustainPedalNum(int track) const {
	if (!this->container) { return 0; }
	return this->container->getMIDISustainPedalNum(track);
}

int SourceItem::getMIDISostenutoPedalNum(int track) const {
	if (!this->container) { return 0; }
	return this->container->getMIDISostenutoPedalNum(track);
}

int SourceItem::getMIDISoftPedalNum(int track) const {
	if (!this->container) { return 0; }
	return this->container->getMIDISoftPedalNum(track);
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

const SourceMIDITemp::Pedal SourceItem::getMIDISustainPedal(int track, int index) const {
	if (!this->container) { return {}; }
	return this->container->getMIDISustainPedal(track, index);
}

const SourceMIDITemp::Pedal SourceItem::getMIDISostenutoPedal(int track, int index) const {
	if (!this->container) { return {}; }
	return this->container->getMIDISustainPedal(track, index);
}

const SourceMIDITemp::Pedal SourceItem::getMIDISoftPedal(int track, int index) const {
	if (!this->container) { return {}; }
	return this->container->getMIDISoftPedal(track, index);
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

void SourceItem::prepareAudioData(double length, int channelNum) {
	double sampleRate = 0;
	if (this->container) {
		sampleRate = this->container->getAudioSampleRate();
	}
	if (sampleRate <= 0) {
		sampleRate = playSampleRate;
	}

	this->initAudio(channelNum, sampleRate, length);
}

void SourceItem::prepareMIDIData() {
	this->initMIDI();
}

void SourceItem::releaseContainer() {
	if (this->container) {
		auto name = this->container->getName();
		this->container = nullptr;
		SourceInternalPool::getInstance()->checkSourceReleased(name);
	}
}

#include "SourceItem.h"
#include "../misc/VMath.h"
#include "../Utils.h"

void SourceItem::initAudio(
	int channelNum, double sampleRate, double length) {
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
	this->updateAudioResampler();

	/** Update Name */
	this->fileName = juce::String{};

	/** Set Flag */
	this->changed();
}

void SourceItem::initMIDI() {
	/** Create MIDI Data */
	this->midiData = std::make_unique<juce::MidiFile>();
	this->midiData->addTrack(juce::MidiMessageSequence{});

	/** Update Name */
	this->fileName = juce::String{};

	/** Set Flag */
	this->changed();
}

void SourceItem::setAudio(
	double sampleRate, const juce::AudioSampleBuffer& data, const juce::String& name) {
	/** Clear Audio Source */
	this->resampleSource = nullptr;
	this->memSource = nullptr;
	this->audioData = nullptr;

	/** Create Buffer */
	this->audioData = std::make_unique<juce::AudioSampleBuffer>(data);
	this->audioSampleRate = sampleRate;
	this->fileName = name;

	/** Update Resample Source */
	this->updateAudioResampler();

	/** Set Flag */
	this->changed();
}

void SourceItem::setMIDI(
	const juce::MidiFile& data, const juce::String& name) {
	/** Create MIDI Data */
	this->midiData = std::make_unique<juce::MidiFile>(data);
	this->fileName = name;

	/** Set Flag */
	this->changed();
}

const std::tuple<double, juce::AudioSampleBuffer> SourceItem::getAudio() const {
	/** Check Data */
	if (!this->audioData) {
		return { 0, juce::AudioSampleBuffer{} };
	}

	/** Copy Data */
	return { this->audioSampleRate, *(this->audioData.get()) };
}

const juce::MidiMessageSequence SourceItem::getMIDI(int trackIndex) const {
	/** Check Data */
	if (this->midiData) {
		if (auto track = this->midiData->getTrack(trackIndex)) {
			return juce::MidiMessageSequence{ *track };
		}
	}

	/** Default Result */
	return juce::MidiMessageSequence{};
}

const juce::MidiFile SourceItem::getMIDIFile() const {
	/** Check Data */
	if (!this->midiData) {
		return juce::MidiFile{};
	}

	/** Copy Data */
	return *(this->midiData.get());
}

void SourceItem::changed() {
	this->savedFlag = false;
}

void SourceItem::saved() {
	this->savedFlag = true;
}

bool SourceItem::isSaved() const {
	return this->savedFlag;
}

void SourceItem::prepareAudioPlay() {
	if (this->resampleSource) {
		this->resampleSource->setResamplingRatio(this->audioSampleRate / this->playSampleRate);
		this->resampleSource->prepareToPlay(this->blockSize, this->playSampleRate);
	}
}

void SourceItem::prepareMIDIPlay() {
	/** Nothing To Do */
}

void SourceItem::prepareAudioRecord(int channelNum) {
	if (!this->audioData) {
		this->prepareAudioData(this->recordInitLength, channelNum);
	}
}

void SourceItem::prepareMIDIRecord() {
	if ((!this->midiData) || (this->midiData->getNumTracks() <= 0)) {
		this->prepareMIDIData();
	}
}

void SourceItem::setSampleRate(int blockSize, double sampleRate) {
	this->playSampleRate = sampleRate;
	this->blockSize = blockSize;

	this->updateAudioResampler();
}

void SourceItem::readAudioData(
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

void SourceItem::readMIDIData(
	juce::MidiBuffer& buffer, double baseTime,
	double startTime, double endTime, int trackIndex) const {
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
	double resampleRatio = this->playSampleRate / this->audioSampleRate;
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
	this->changed();
}

void SourceItem::writeMIDIData(
	const juce::MidiBuffer& buffer, int offset, int trackIndex) {
	/** Prepare Write */
	this->prepareMIDIRecord();

	/** Write To The Last Track */
	if (auto track = const_cast<juce::MidiMessageSequence*>(
		this->midiData->getTrack(trackIndex))) {
		for (const auto& m : buffer) {
			double timeStamp = (m.samplePosition + offset) / this->playSampleRate;
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

void SourceItem::updateAudioResampler() {
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
	resSource->setResamplingRatio(this->audioSampleRate / this->playSampleRate);
	resSource->prepareToPlay(this->blockSize, this->playSampleRate);

	/** Set Resample Source */
	this->resampleSource = std::move(resSource);
}

void SourceItem::prepareAudioData(double length, int channelNum) {
	double sampleRate = this->audioSampleRate;
	if (sampleRate <= 0) {
		sampleRate = playSampleRate;
	}

	this->initAudio(channelNum, sampleRate, length);
}

void SourceItem::prepareMIDIData() {
	this->initMIDI();
}

#include "RecordTemp.h"
#include "VMath.h"

#define AUDIO_BUFFER_MIN 48000 * 30

RecordTemp::RecordTemp() {
	/** Init Audio Buffer */
	this->clearAudio();
}

void RecordTemp::setInputSampleRate(double sampleRate) {
	juce::GenericScopedLock locker(this->lock);
	this->sampleRate = sampleRate;
	this->clearAll();
}

void RecordTemp::setInputChannelNum(int channels) {
	juce::GenericScopedLock locker(this->lock);
	this->audioBuffer.setSize(channels,
		std::max(this->audioBuffer.getNumSamples(), AUDIO_BUFFER_MIN), true, true, true);
}

void RecordTemp::recordData(double timeSec,
	const juce::AudioBuffer<float>& buffer, const juce::MidiBuffer& midiMessages) {
	/** Lock */
	juce::GenericScopedLock locker(this->lock);

	/** Set Start Time */
	if (timeSec < this->startTime) {
		this->clearAll();
	}
	if (this->startTime < 0) {
		this->startTime = timeSec;
	}

	/** Collect MIDI Message */
	if (this->recordMIDI) {
		for (auto i : midiMessages) {
			auto message = i.getMessage();
			message.setTimeStamp((timeSec - this->startTime) + message.getTimeStamp() / this->sampleRate);
			this->midiBuffer.addEvent(message);
		}
	}
	
	/** Write Audio Buffer */
	if (this->recordAudio) {
		uint64_t startSample = (timeSec - this->startTime) * this->sampleRate;
		uint64_t endSample = startSample + buffer.getNumSamples();
		if (this->tryToEnsureAudioBufferSamplesAllocated(endSample)) {
			int channelNum = std::min(buffer.getNumChannels(), this->audioBuffer.getNumChannels());
			for (int i = 0; i < channelNum; i++) {
				vMath::copyAudioData(this->audioBuffer, buffer,
					(int)startSample, 0, i, i, buffer.getNumSamples());
			}
		}
	}
}

void RecordTemp::setRecordMIDI(bool recordMIDI) {
	juce::GenericScopedLock locker(this->lock);
	this->recordMIDI = recordMIDI;
	if (!recordMIDI) {
		this->clearMIDI();
	}
}

void RecordTemp::setRecordAudio(bool recordAudio) {
	juce::GenericScopedLock locker(this->lock);
	this->recordAudio = recordAudio;
	if (!recordAudio) {
		this->clearAudio();
	}
}

bool RecordTemp::isRecordMIDI() const {
	juce::GenericScopedLock locker(this->lock);
	return this->recordMIDI;
}

bool RecordTemp::isRecordAudio() const {
	juce::GenericScopedLock locker(this->lock);
	return this->recordAudio;
}

void RecordTemp::clearAll() {
	juce::GenericScopedLock locker(this->lock);
	this->clearAudio();
	this->clearMIDI();
	this->startTime = -1;
}

void RecordTemp::clearMIDI() {
	juce::GenericScopedLock locker(this->lock);
	this->midiBuffer.clear();
}

void RecordTemp::clearAudio() {
	juce::GenericScopedLock locker(this->lock);
	this->audioBuffer.setSize(
		this->audioBuffer.getNumChannels(),
		AUDIO_BUFFER_MIN, false, true, true);
}

double RecordTemp::getSampleRate() const {
	juce::GenericScopedLock locker(this->lock);
	return this->sampleRate;
}

double RecordTemp::getStartTime() const {
	juce::GenericScopedLock locker(this->lock);
	return this->startTime;
}

const juce::MidiMessageSequence RecordTemp::getMIDIData() const {
	juce::GenericScopedLock locker(this->lock);
	return this->midiBuffer;
}

const juce::AudioSampleBuffer RecordTemp::getAudioData() const {
	juce::GenericScopedLock locker(this->lock);
	return this->audioBuffer;
}

const RecordTemp::DataPacked RecordTemp::getDataPacked() const {
	juce::GenericScopedLock locker(this->lock);
	return { this->sampleRate, this->startTime, this->midiBuffer, this->audioBuffer };
}

bool RecordTemp::tryToEnsureAudioBufferSamplesAllocated(uint64_t sampleNum) {
	while (this->audioBuffer.getNumSamples() < sampleNum) {
		uint64_t num = (uint64_t)(this->audioBuffer.getNumSamples()) * 2;
		if (num > INT_MAX) {
			this->audioBuffer.setSize(this->audioBuffer.getNumChannels(),
				INT_MAX, false, true, true);
			break;
		}

		this->audioBuffer.setSize(this->audioBuffer.getNumChannels(),
			std::max((int)num, AUDIO_BUFFER_MIN), false, true, true);
	}

	return this->audioBuffer.getNumSamples() >= sampleNum;
}

RecordTemp* RecordTemp::getInstance() {
	return RecordTemp::instance
		? RecordTemp::instance : (RecordTemp::instance = new RecordTemp());
}

void RecordTemp::releaseInstance() {
	if (RecordTemp::instance) {
		delete RecordTemp::instance;
		RecordTemp::instance = nullptr;
	}
}

RecordTemp* RecordTemp::instance = nullptr;

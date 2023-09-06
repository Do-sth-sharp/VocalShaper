#include "CloneableAudioSource.h"

#include "../Utils.h"

double CloneableAudioSource::getSourceSampleRate() const {
	return this->sourceSampleRate;
}

void CloneableAudioSource::readData(juce::AudioBuffer<float>& buffer, double bufferDeviation,
	double dataDeviation, double length) const {
	if (buffer.getNumSamples() <= 0 || length <= 0) { return; }

	juce::ScopedTryReadLock locker(this->lock);
	if (locker.isLocked()) {
		if (this->source && this->memorySource) {
			this->memorySource->setNextReadPosition(std::floor(dataDeviation * this->sourceSampleRate));
			int startSample = (int)std::floor(bufferDeviation * this->getSampleRate());
			this->source->getNextAudioBlock(juce::AudioSourceChannelInfo{
				&buffer, startSample,
					std::min(buffer.getNumSamples() - startSample, (int)std::ceil(length * this->getSampleRate()))});/**< Ceil then min with buffer size to ensure audio data fill the last point in buffer */
		}
	}
}

int CloneableAudioSource::getChannelNum() const {
	return this->buffer.getNumChannels();
}

juce::ReadWriteLock& CloneableAudioSource::getRecorderLock() const {
	return this->lock;
}

bool CloneableAudioSource::clone(const CloneableSource* src) {
	juce::ScopedWriteLock locker(this->lock);

	/** Check Source Type */
	auto ptrSrc = dynamic_cast<const CloneableAudioSource*>(src);
	if (!ptrSrc) { return false; }

	/** Clear Audio Source */
	this->source = nullptr;
	this->memorySource = nullptr;

	/** Copy Data */
	this->buffer = ptrSrc->buffer;
	this->sourceSampleRate = ptrSrc->sourceSampleRate;

	/** Create Audio Source */
	this->memorySource = std::make_unique<juce::MemoryAudioSource>(this->buffer, false, false);
	auto source = std::make_unique<juce::ResamplingAudioSource>(this->memorySource.get(), false, this->buffer.getNumChannels());

	/** Set Sample Rate */
	source->setResamplingRatio(this->sourceSampleRate / this->getSampleRate());

	this->source = std::move(source);
	return true;
}

bool CloneableAudioSource::load(const juce::File& file) {
	juce::ScopedWriteLock locker(this->lock);

	/** Create Audio Reader */
	auto audioReader = utils::createAudioReader(file);
	if (!audioReader) { return false; }

	/** Clear Audio Source */
	this->source = nullptr;
	this->memorySource = nullptr;

	/** Copy Data */
	this->buffer = juce::AudioSampleBuffer{ (int)audioReader->numChannels, (int)audioReader->lengthInSamples };
	audioReader->read(&(this->buffer), 0, audioReader->lengthInSamples, 0, true, true);
	this->sourceSampleRate = audioReader->sampleRate;

	/** Create Audio Source */
	this->memorySource = std::make_unique<juce::MemoryAudioSource>(this->buffer, false, false);
	auto source = std::make_unique<juce::ResamplingAudioSource>(this->memorySource.get(), false, this->buffer.getNumChannels());

	/** Set Sample Rate */
	source->setResamplingRatio(this->sourceSampleRate / this->getSampleRate());
	source->prepareToPlay(this->getBufferSize(), this->getSampleRate());

	this->source = std::move(source);
	return true;
}

bool CloneableAudioSource::save(const juce::File& file) const {
	juce::ScopedReadLock locker(this->lock);

	if (!this->source) { return false; }

	/** Create Audio Writer */
	auto audioWriter = utils::createAudioWriter(file, this->sourceSampleRate,
		juce::AudioChannelSet::canonicalChannelSet(this->buffer.getNumChannels()));
	if (!audioWriter) { return false; }

	/** Write Data */
	audioWriter->writeFromAudioSampleBuffer(this->buffer, 0, this->buffer.getNumSamples());

	return true;
}

double CloneableAudioSource::getLength() const {
	juce::ScopedReadLock locker(this->lock);
	return this->buffer.getNumSamples() / this->sourceSampleRate;
}

void CloneableAudioSource::sampleRateChanged() {
	juce::ScopedWriteLock locker(this->lock);
	if (this->source) {
		this->source->setResamplingRatio(this->sourceSampleRate / this->getSampleRate());
		this->source->prepareToPlay(this->getBufferSize(), this->getSampleRate());
	}
}

void CloneableAudioSource::prepareToRecord(
	int channelNum, double sampleRate, int bufferSize) {
	if (this->getSourceSampleRate() != sampleRate) {
		this->buffer.clear();
	}

	this->buffer.setSize(
		channelNum, this->buffer.getNumSamples(), true, false, true);

	this->prepareToPlay(sampleRate, bufferSize);
}

void CloneableAudioSource::writeData(
	const juce::AudioBuffer<float>& buffer, double offset)	{
	/** Get Time */
	int startSample = offset * this->getSampleRate();
	int srcStartSample = 0;
	int length = buffer.getNumSamples();
	if (startSample < 0) {
		srcStartSample -= startSample;
		length -= srcStartSample;
		startSample = 0;
	}

	/** Length Limit */
	if (startSample > this->buffer.getNumSamples() - length) {
		length = this->buffer.getNumSamples() - startSample;
	}

	/** CopyData */
	for (int i = 0; i < buffer.getNumChannels(); i++) {
		if (auto rptr = buffer.getReadPointer(i)) {
			this->buffer.copyFrom(
				i, startSample, &(rptr)[srcStartSample], length);
		}
	}
}

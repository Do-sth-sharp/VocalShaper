#include "CloneableAudioSource.h"

#include "../misc/AudioLock.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

double CloneableAudioSource::getSourceSampleRate() const {
	return this->sourceSampleRate;
}

void CloneableAudioSource::readData(juce::AudioBuffer<float>& buffer, int bufferOffset,
	int dataOffset, int length) const {
	if (this->checkRecording()) { return; }
	if (buffer.getNumSamples() <= 0 || length <= 0) { return; }

	if (this->source && this->memorySource) {
		this->memorySource->setNextReadPosition((int64_t)(dataOffset * this->source->getResamplingRatio()));
		int startSample = bufferOffset;
		this->source->getNextAudioBlock(juce::AudioSourceChannelInfo{
			&buffer, startSample,
				std::min(buffer.getNumSamples() - startSample, length) });
	}
}

int CloneableAudioSource::getChannelNum() const {
	return this->buffer.getNumChannels();
}

bool CloneableAudioSource::parse(const google::protobuf::Message* data) {
	/** Nothing To Do, Never Invoked */
	return dynamic_cast<const vsp4::Source*>(data);
}

std::unique_ptr<google::protobuf::Message> CloneableAudioSource::serialize() const {
	auto mes = std::make_unique<vsp4::Source>();

	mes->set_type(vsp4::Source_Type_AUDIO);
	mes->set_id(this->getId());
	mes->set_name(this->getName().toStdString());
	juce::String path = this->getPath();
	mes->set_path(
		juce::String{ path.isNotEmpty() ? path
		: utils::getSourceDefaultPathForAudio(
			this->getId(), this->getName()) }.toStdString());

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

std::unique_ptr<CloneableSource> CloneableAudioSource::clone() const {
	/** Lock */
	juce::ScopedTryReadLock locker(audioLock::getLock());
	if (locker.isLocked()) {
		/** Create New Source */
		auto dst = std::unique_ptr<CloneableAudioSource>();

		/** Copy Data */
		dst->buffer = this->buffer;
		dst->sourceSampleRate = this->sourceSampleRate;

		/** Create Audio Source */
		dst->memorySource = std::make_unique<juce::MemoryAudioSource>(dst->buffer, false, false);
		dst->source = std::make_unique<juce::ResamplingAudioSource>(dst->memorySource.get(), false, dst->buffer.getNumChannels());

		/** Set Sample Rate */
		dst->source->setResamplingRatio(this->sourceSampleRate / this->getSampleRate());

		/** Move Result */
		return std::move(std::unique_ptr<CloneableSource>(dst.release()));
	}
	return nullptr;
}

bool CloneableAudioSource::load(const juce::File& file) {
	/** Check Not Recording */
	if (this->checkRecording()) { return false; }

	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getLock());

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
	juce::ScopedReadLock locker(audioLock::getLock());

	/** Create Audio Writer */
	auto audioWriter = utils::createAudioWriter(file, this->sourceSampleRate,
		juce::AudioChannelSet::canonicalChannelSet(this->buffer.getNumChannels()));
	if (!audioWriter) { return false; }

	/** Write Data */
	audioWriter->writeFromAudioSampleBuffer(this->buffer, 0, this->buffer.getNumSamples());

	return true;
}

double CloneableAudioSource::getLength() const {
	juce::ScopedReadLock locker(audioLock::getLock());
	return this->buffer.getNumSamples() / this->sourceSampleRate;
}

void CloneableAudioSource::sampleRateChanged() {
	juce::ScopedWriteLock locker(audioLock::getLock());
	if (this->source) {
		this->source->setResamplingRatio(this->sourceSampleRate / this->getSampleRate());
		this->source->prepareToPlay(this->getBufferSize(), this->getSampleRate());
	}
}

void CloneableAudioSource::init(double sampleRate, int channelNum, int sampleNum) {
	/** Check Not Recording */
	if (this->checkRecording()) { return; }

	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getLock());

	/** Clear Audio Source */
	this->source = nullptr;
	this->memorySource = nullptr;

	/** Create Buffer */
	this->buffer = juce::AudioSampleBuffer{ channelNum, sampleNum };
	this->sourceSampleRate = sampleRate;

	/** Create Audio Source */
	this->memorySource = std::make_unique<juce::MemoryAudioSource>(this->buffer, false, false);
	auto source = std::make_unique<juce::ResamplingAudioSource>(this->memorySource.get(), false, this->buffer.getNumChannels());

	/** Set Sample Rate */
	source->setResamplingRatio(this->sourceSampleRate / this->getSampleRate());
	source->prepareToPlay(this->getBufferSize(), this->getSampleRate());

	this->source = std::move(source);
}

void CloneableAudioSource::prepareToRecord(
	int inputChannels, double sampleRate, int blockSize, bool /*updateOnly*/) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getLock());

	/** Clear Buffer If Sample Rate Mismatch */
	if (this->getSourceSampleRate() != sampleRate) {
		this->sourceSampleRate = sampleRate;
		this->buffer.setSize(this->buffer.getNumChannels(), 0, false, false, true);
	}

	/** Clear Audio Source */
	this->source = nullptr;
	this->memorySource = nullptr;

	/** Init Buffer */
	this->buffer.setSize(
		std::max(inputChannels, this->buffer.getNumChannels()),
		this->buffer.getNumSamples(), true, false, true);

	/** Set Flag */
	this->changed();
}

void CloneableAudioSource::recordingFinished() {
	/** Create Audio Source */
	this->memorySource = std::make_unique<juce::MemoryAudioSource>(this->buffer, false, false);
	this->source = std::make_unique<juce::ResamplingAudioSource>(this->memorySource.get(), false, this->buffer.getNumChannels());

	/** Update Resampling Ratio */
	this->sampleRateChanged();

	/** Set Flag */
	this->changed();
}

void CloneableAudioSource::writeData(
	const juce::AudioBuffer<float>& buffer, int offset)	{
	/** Lock */
	juce::ScopedTryWriteLock locker(audioLock::getLock());
	if (locker.isLocked()) {
		/** Get Time */
		int startSample = offset;
		int srcStartSample = 0;
		int length = buffer.getNumSamples();
		if (startSample < 0) {
			srcStartSample -= startSample;
			length -= srcStartSample;
			startSample = 0;
		}

		/** Increase BufferLength */
		if (startSample > this->buffer.getNumSamples() - length) {
			int newLength = startSample + length;
			this->buffer.setSize(
				this->buffer.getNumChannels(), newLength, true, false, true);
		}

		/** CopyData */
		for (int i = 0; i < buffer.getNumChannels() && i < this->buffer.getNumChannels(); i++) {
			if (auto rptr = buffer.getReadPointer(i)) {
				this->buffer.copyFrom(
					i, startSample, &(rptr)[srcStartSample], length);
			}
		}

		/** Set Flag */
		this->changed();
	}
}

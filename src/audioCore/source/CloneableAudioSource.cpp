#include "CloneableAudioSource.h"

#include "../misc/AudioLock.h"
#include "../misc/VMath.h"
#include "../uiCallback/UICallback.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

double CloneableAudioSource::getSourceSampleRate() const {
	return this->sourceSampleRate;
}

void CloneableAudioSource::readData(juce::AudioBuffer<float>& buffer, int bufferOffset,
	int dataOffset, int length) const {
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
	return true;
}

std::unique_ptr<google::protobuf::Message> CloneableAudioSource::serialize() const {
	return nullptr;
}

std::unique_ptr<CloneableSource> CloneableAudioSource::createThisType() const {
	return std::unique_ptr<CloneableSource>{ new CloneableAudioSource };
}

bool CloneableAudioSource::clone(CloneableSource* dst) const {
	/** Lock */
	juce::ScopedTryReadLock locker(audioLock::getSourceLock());
	if (locker.isLocked()) {
		if (auto dsts = dynamic_cast<CloneableAudioSource*>(dst)) {
			/** Copy Data */
			dsts->buffer = this->buffer;
			dsts->sourceSampleRate = this->sourceSampleRate;

			/** Create Audio Source */
			dsts->memorySource = std::make_unique<juce::MemoryAudioSource>(dsts->buffer, false, false);
			dsts->source = std::make_unique<juce::ResamplingAudioSource>(dsts->memorySource.get(), false, dsts->buffer.getNumChannels());

			/** Set Sample Rate */
			dsts->source->setResamplingRatio(this->sourceSampleRate / this->getSampleRate());

			/** Result */
			return true;
		}
	}
	return false;
}

bool CloneableAudioSource::load(const juce::File& file) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

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
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	/** Create Audio Writer */
	auto audioWriter = utils::createAudioWriter(file, this->sourceSampleRate,
		juce::AudioChannelSet::canonicalChannelSet(this->buffer.getNumChannels()));
	if (!audioWriter) { return false; }

	/** Write Data */
	audioWriter->writeFromAudioSampleBuffer(this->buffer, 0, this->buffer.getNumSamples());

	return true;
}

double CloneableAudioSource::getLength() const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	return this->buffer.getNumSamples() / this->sourceSampleRate;
}

void CloneableAudioSource::sampleRateChanged() {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	if (this->source) {
		this->source->setResamplingRatio(this->sourceSampleRate / this->getSampleRate());
		this->source->prepareToPlay(this->getBufferSize(), this->getSampleRate());
	}
}

void CloneableAudioSource::init(double sampleRate, int channelNum, int sampleNum) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Clear Audio Source */
	this->source = nullptr;
	this->memorySource = nullptr;

	/** Create Buffer */
	this->buffer = juce::AudioSampleBuffer{ channelNum, sampleNum };
	vMath::zeroAllAudioData(this->buffer);
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
	int /*inputChannels*/, double /*sampleRate*/, int /*blockSize*/, bool /*updateOnly*/) {
	/** Callback */
	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});
}

void CloneableAudioSource::recordingFinished() {
	/** Callback */
	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});
}

void CloneableAudioSource::writeData(
	juce::AudioBuffer<float>& buffer, int offset) {
	/** Get Time */
	int srcLength = buffer.getNumSamples();
	int srcStartSample = offset;
	int bufferStartSample = 0;
	if (srcStartSample < 0) {
		bufferStartSample -= srcStartSample;
		srcLength -= bufferStartSample;
		srcStartSample = 0;
	}

	/** Prepare Resampling */
	double resampleRatio = this->getSampleRate() / this->getSourceSampleRate();
	int channelNum = std::min(buffer.getNumChannels(), this->buffer.getNumChannels());

	/** Copy Data Resampled */
	utils::bufferOutputResampledFixed(this->buffer, buffer,
		this->recordBuffer, this->recordBufferTemp,
		resampleRatio, channelNum, this->getSourceSampleRate(),
		0, srcStartSample, srcLength);

	/** Set Flag */
	this->changed();
}

juce::AudioSampleBuffer* CloneableAudioSource::getAudioContentPtr() {
	return &(this->buffer);
}

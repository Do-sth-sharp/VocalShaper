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
	auto src = dynamic_cast<const vsp4::Source*>(data);
	if (!src) { return false; }

	this->setName(src->name());
	this->setPath(src->path());
	return true;
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

	double resampleRatio = this->getSampleRate() / this->getSourceSampleRate();
	int dstStartSample = std::floor(srcStartSample / resampleRatio);
	int dstLength = std::floor(srcLength / resampleRatio);

	/** Limit BufferLength */
	if (dstStartSample > this->buffer.getNumSamples() - dstLength) {
		dstLength = this->buffer.getNumSamples() - dstStartSample;
		if (dstLength <= 0) {
			return;
		}
	}

	/** Change Record Buffer */
	{
		/** Get Buffer Size */
		int rbSize = this->recordBuffer.getNumSamples();
		int rbChannel = this->recordBuffer.getNumChannels();
		
		/** Copy Old Data to Temp */
		if ((this->recordBufferTemp.getNumSamples() != srcLength) || (this->recordBufferTemp.getNumChannels() != rbChannel)) {
			this->recordBufferTemp.setSize(rbChannel, srcLength, true, false, true);
		}
		if (rbSize < srcLength * 3) {
			for (int i = 0; i < rbChannel; i++) {
				vMath::copyAudioData(this->recordBufferTemp, this->recordBuffer,
					srcLength - rbSize / 3, srcLength, i, i, rbSize / 3);
			}
		}
		else {
			for (int i = 0; i < rbChannel; i++) {
				vMath::copyAudioData(this->recordBufferTemp, this->recordBuffer,
					0, rbSize / 3, i, i, srcLength);
			}
		}

		/** Change Buffer Size */
		if ((rbSize != srcLength * 3) || (rbChannel != buffer.getNumChannels())) {
			this->recordBuffer.setSize(buffer.getNumChannels(), srcLength * 3, true, true, true);
		}

		/** Copy Old Data */
		for (int i = 0; i < rbChannel; i++) {
			vMath::copyAudioData(this->recordBuffer, this->recordBufferTemp,
				0, 0, i, i, srcLength);
		}

		/** Copy New Data */
		for (int i = 0; i < buffer.getNumChannels(); i++) {
			vMath::copyAudioData(this->recordBuffer, buffer,
				srcLength, 0, i, i, srcLength);
		}

		/** Fill Tails Data */
		for (int i = 0; i < buffer.getNumChannels(); i++) {
			vMath::fillAudioData(this->recordBuffer,
				buffer.getSample(i, buffer.getNumSamples() - 1),
				srcLength * 2, i, srcLength);
		}
	}

	/** Prepare Resampling */
	int channelNum = std::min(buffer.getNumChannels(), this->buffer.getNumChannels());

	juce::MemoryAudioSource memSource(this->recordBuffer, false);
	juce::ResamplingAudioSource resampleSource(&memSource, false, channelNum);

	memSource.setNextReadPosition(srcLength + bufferStartSample);
	resampleSource.setResamplingRatio(resampleRatio);
	resampleSource.prepareToPlay(dstLength, this->getSourceSampleRate());

	/** CopyData */
	int trueStartSample = dstStartSample - 1;/** To Avoid sss Noise In Buffer Head */
	if (srcLength > 0) {
		resampleSource.getNextAudioBlock(juce::AudioSourceChannelInfo{
						&(this->buffer), (trueStartSample >= 0) ? trueStartSample : 0,
						(trueStartSample >= 0) ? (dstLength + 2) : (dstLength + 1) });
	}

	/** Set Flag */
	this->changed();
}

juce::AudioSampleBuffer* CloneableAudioSource::getAudioContentPtr() {
	return &(this->buffer);
}

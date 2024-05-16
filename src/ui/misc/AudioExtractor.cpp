#include "AudioExtractor.h"
#include "MainThreadPool.h"

class AudioExtractorJob final : public juce::ThreadPoolJob {
public:
	AudioExtractorJob() = delete;
	AudioExtractorJob(const juce::AudioSampleBuffer* data,
		uint64_t pointNum, const AudioExtractor::Callback& callback);
	~AudioExtractorJob();

	void updateSizeUnsafe(const juce::AudioSampleBuffer* data,
		uint64_t pointNum);
	void stopNow();

private:
	JobStatus runJob() override;

	bool doExtract();
	void sendResult();

private:
	const juce::AudioSampleBuffer* data;
	uint64_t pointNum;
	const AudioExtractor::Callback callback;

	std::unique_ptr<AudioExtractor::Result> result = nullptr;
	void reallocResultUnsafe();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioExtractorJob)
};

AudioExtractorJob::AudioExtractorJob(const juce::AudioSampleBuffer* data,
	uint64_t pointNum, const AudioExtractor::Callback& callback)
	: ThreadPoolJob("Audio Extractor Job"),
	data(data), pointNum(pointNum), callback(callback) {
	/** Prepare Result */
	this->reallocResultUnsafe();
}

AudioExtractorJob::~AudioExtractorJob() {
	this->stopNow();
}

void AudioExtractorJob::updateSizeUnsafe(const juce::AudioSampleBuffer* data,
	uint64_t pointNum) {
	/** Change Result Memory */
	bool channelsChanged = this->data->getNumChannels() != data->getNumChannels();
	this->data = data;
	if (pointNum != this->pointNum || channelsChanged) {
		this->pointNum = pointNum;
		this->reallocResultUnsafe();
	}
}

void AudioExtractorJob::stopNow() {
	if (this->isRunning()) {
		this->signalJobShouldExit();
		while (this->isRunning());
	}
}

AudioExtractorJob::JobStatus AudioExtractorJob::runJob() {
	/** Do Extract */
	bool success = this->doExtract();
	if (success) {
		/** Send Result */
		this->sendResult();
	}

	return JobStatus::jobHasFinished;
}

bool AudioExtractorJob::doExtract() {
	if (!this->data) { return false; }
	if (!this->result) { return false; }

	/** Prepare Data */
	int sampleNum = this->data->getNumSamples();
	double clipSize = sampleNum / (double)this->pointNum;

	/** Get Each Track */
	for (int i = 0; i < this->data->getNumChannels(); i++) {
		auto dataPtr = this->data->getReadPointer(i);
		auto& channelResult = this->result->getReference(i);

		/** Get Clip Data */
		for (int j = 0; j < this->pointNum; j++) {
			float minV = 1.f, maxV = -1.f;

			if (clipSize >= 1) {
				/** Extract */
				for (int k = j * clipSize; k < std::ceil((j + 1) * clipSize); k++) {
					minV = std::min(minV, dataPtr[k]);
					maxV = std::max(maxV, dataPtr[k]);
				}
			}
			else {
				/** Interpolation */
				double dataIndex = j * clipSize;
				int dataIndexFloor = std::floor(dataIndex);
				int dataIndexCeil = std::ceil(dataIndex);
				double dataPer = dataIndex - dataIndexFloor;

				minV = maxV = dataPtr[dataIndexFloor] * (1.0 - dataPer)
					+ dataPtr[dataIndexCeil] * dataPer;
			}

			channelResult.copyFrom(&minV, (j * 2 + 0) * (int)sizeof(float), sizeof(float));
			channelResult.copyFrom(&maxV, (j * 2 + 1) * (int)sizeof(float), sizeof(float));

			/** Check Exit */
			if (this->shouldExit()) {
				return false;
			}
		}

		/** Check Exit */
		if (this->shouldExit()) {
			return false;
		}
	}

	return true;
}

void AudioExtractorJob::sendResult() {
	if (!this->result) { return; }

	auto cbFunc = [res = *(this->result), cb = this->callback] {
		cb(res);
		};
	juce::MessageManager::callAsync(cbFunc);
}

void AudioExtractorJob::reallocResultUnsafe() {
	this->result = std::make_unique<AudioExtractor::Result>();
	for (int i = 0; i < this->data->getNumChannels(); i++) {
		this->result->add(
			juce::MemoryBlock{ this->pointNum * sizeof(float) * 2 });
	}
}

void AudioExtractor::extractAsync(const void* ticket,
	const juce::AudioSampleBuffer& data, uint64_t pointNum,
	const Callback& callback) {
	/**
	 * This may cause a bug when call this while AudioExtractorJob::runJob() is running between
	 * AudioExtractorJob::doExtract() and AudioExtractorJob::sendResult().
	 * The result of current async extract will be dropped.
	 * To fix this, I add a Timer in SeqTrackContentViewer to extract again every period of time.
	 * I currently do not have a better solution.
	 */

	/** Find */
	auto it = this->templist.find(ticket);

	/** Exists */
	if (it != this->templist.end()) {
		if (auto ptrJob = dynamic_cast<AudioExtractorJob*>(it->second.job.get())) {
			ptrJob->stopNow();
			it->second.data = data;
		}
	}
	/** Add New */
	else {
		/** Create Temp */
		auto res = this->templist.insert(
			std::make_pair(ticket, DataTemp{ data, nullptr }));
		if (!res.second) { return; }
		it = res.first;

		/** Callback */
		auto cb = [this, callback, ticket](const Result& result) {
			/** Destory Ticket */
			this->destoryTicket(ticket);

			/** Callback */
			callback(result);
			};

		/** Set Job */
		it->second.job = std::make_shared<AudioExtractorJob>(
			&(it->second.data), pointNum, cb);
	}

	/** Run Job */
	MainThreadPool::getInstance()->runJob(it->second.job.get(), false);
}

void AudioExtractor::destoryTicket(const void* ticket) {
	auto it = this->templist.find(ticket);
	if (it != this->templist.end()) {
		this->templist.erase(it);
	}
}

AudioExtractor* AudioExtractor::getInstance() {
	return AudioExtractor::instance ? AudioExtractor::instance
		: (AudioExtractor::instance = new AudioExtractor{});
}

void AudioExtractor::releaseInstance() {
	if (AudioExtractor::instance) {
		delete AudioExtractor::instance;
		AudioExtractor::instance = nullptr;
	}
}

AudioExtractor* AudioExtractor::instance = nullptr;

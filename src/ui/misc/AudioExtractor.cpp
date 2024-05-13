#include "AudioExtractor.h"
#include "MainThreadPool.h"

void AudioExtractor::extractAsync(
	const AudioData& data, uint64_t pointNum,
	const Callback& callback) {
	auto job = [data, pointNum, callback] {
		AudioExtractor::extractInternal(
			std::get<1>(data), pointNum, callback);
		};
	MainThreadPool::getInstance()->runJob(job);
}

void AudioExtractor::extractInternal(
	const juce::AudioSampleBuffer& data, uint64_t pointNum,
	const Callback& callback) {
	/** Prepare Data */
	int sampleNum = data.getNumSamples();
	double clipSize = sampleNum / (double)pointNum;

	/** Result */
	Result result;

	/** Get Each Track */
	for (int i = 0; i < data.getNumChannels(); i++) {
		auto dataPtr = data.getReadPointer(i);
		juce::MemoryBlock channelResult(pointNum * sizeof(float) * 2);

		/** Get Clip Data */
		for (int j = 0; j < pointNum; j++) {
			if (clipSize >= 1) {
				/** TODO Extract */
			}
			else {
				/** TODO Interpolation */
			}
		}

		result.add(std::move(channelResult));
	}

	/** Callback */
	juce::MessageManager::callAsync(std::bind(callback, result));
}

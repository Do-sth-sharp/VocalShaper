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
		}

		result.add(std::move(channelResult));
	}

	/** Callback */
	juce::MessageManager::callAsync(std::bind(callback, result));
}

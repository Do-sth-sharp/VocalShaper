#include "AudioSampleTemp.h"

void AudioSampleTemp::setAudio(int index,
	const juce::AudioSampleBuffer& data, double sampleRate) {
	/** Lock */
	juce::ScopedWriteLock locker(this->lock);

	/** Add Block */
	if (index >= this->list.size()) {
		int addNum = index - this->list.size() + 1;
		for (int i = 0; i < addNum; i++) {
			this->list.add(std::make_unique<TempData>());
		}
	}

	/** Get Block Ptr */
	auto ptrBlock = this->list.getUnchecked(index);
	if (!ptrBlock) { return; }

	/** Write Data */
	ptrBlock->audioData = data;
	ptrBlock->sampleRate = sampleRate;
}

const juce::AudioSampleBuffer* AudioSampleTemp::getAudioData(int index) const {
	juce::ScopedReadLock locker(this->lock);
	if (auto ptrBlock = this->list.getUnchecked(index)) {
		return &(ptrBlock->audioData);
	}
	return nullptr;
}

double AudioSampleTemp::getSampleRate(int index) const {
	juce::ScopedReadLock locker(this->lock);
	if (auto ptrBlock = this->list.getUnchecked(index)) {
		return ptrBlock->sampleRate;
	}
	return 0;
}

void AudioSampleTemp::clear() {
	juce::ScopedWriteLock locker(this->lock);
	this->list.clear();
}

AudioSampleTemp* AudioSampleTemp::getInstance() {
	return AudioSampleTemp::instance ? AudioSampleTemp::instance
		: (AudioSampleTemp::instance = new AudioSampleTemp{});
}

void AudioSampleTemp::releaseInstance() {
	if (AudioSampleTemp::instance) {
		delete AudioSampleTemp::instance;
		AudioSampleTemp::instance = nullptr;
	}
}

AudioSampleTemp* AudioSampleTemp::instance = nullptr;

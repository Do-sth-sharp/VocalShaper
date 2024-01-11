#pragma once

#include <JuceHeader.h>

namespace vMath {
	enum InsType {
		Normal, SSE3, AVX2, AVX512,
		MaxNum
	};

	void setInsType(InsType type);
	InsType getInsType();
	const juce::String getInsTypeName();
	const juce::StringArray getAllInsTypeName();

	void copyAudioData(juce::AudioSampleBuffer& dst, const juce::AudioSampleBuffer& src,
		int dstStartSample, int srcStartSample, int dstChannel, int srcChannel, int length);
	void addAudioData(juce::AudioSampleBuffer& dst, const juce::AudioSampleBuffer& src,
		int dstStartSample, int srcStartSample, int dstChannel, int srcChannel, int length);
	void zeroAudioData(juce::AudioSampleBuffer& dst,
		int dstStartSample, int dstChannel, int length);
	void zeroAllAudioChannels(juce::AudioSampleBuffer& dst,
		int dstStartSample, int length);
	void zeroAllAudioDataOnChannel(juce::AudioSampleBuffer& dst, int dstChannel);
	void zeroAllAudioData(juce::AudioSampleBuffer& dst);
}

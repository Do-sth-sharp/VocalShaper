#include "VMath.h"

namespace vMath {
	static void copyDataNormal(float* dst, const float* src, int length) {
		std::memcpy(dst, src, length * sizeof(float));
	}

	static void addDataNormal(float* dst, const float* src, int length) {
		for (int i = 0; i < length; i++) {
			dst[i] = dst[i] + src[i];
		}
	}

	static void zeroDataNormal(float* dst, int length) {
		for (int i = 0; i < length; i++) {
			dst[i] = 0.f;
		}
	}

	static void averageDataNormal(float* dst, const float* src, int length) {
		for (int i = 0; i < length; i++) {
			dst[i] = (dst[i] + src[i]) / 2.f;
		}
	}

	static void copyDataSSE3(float* dst, const float* src, int length) {
		int clipSize = sizeof(__m128) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		for (int i = 0; i < clipMax; i+= clipSize) {
			__m128 data = _mm_loadu_ps(&(src[i]));
			_mm_storeu_ps(&(dst[i]), data);
		}

		copyDataNormal(&(dst[clipMax]), &(src[clipMax]), length - clipMax);
	}

	static void addDataSSE3(float* dst, const float* src, int length) {
		int clipSize = sizeof(__m128) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		for (int i = 0; i < clipMax; i += clipSize) {
			__m128 data0 = _mm_loadu_ps(&(dst[i]));
			__m128 data1 = _mm_loadu_ps(&(src[i]));
			__m128 result = _mm_add_ps(data0, data1);
			_mm_storeu_ps(&(dst[i]), result);
		}

		addDataNormal(&(dst[clipMax]), &(src[clipMax]), length - clipMax);
	}

	static void zeroDataSSE3(float* dst, int length) {
		int clipSize = sizeof(__m128) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		constexpr __m128 zeroV = { 0.f, 0.f, 0.f, 0.f };
		for (int i = 0; i < clipMax; i += clipSize) {
			_mm_storeu_ps(&(dst[i]), zeroV);
		}

		zeroDataNormal(&(dst[clipMax]), length - clipMax);
	}

	static void averageDataSSE3(float* dst, const float* src, int length) {
		int clipSize = sizeof(__m128) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		constexpr __m128 averV = { .5f, .5f, .5f, .5f };
		for (int i = 0; i < clipMax; i += clipSize) {
			__m128 data0 = _mm_loadu_ps(&(dst[i]));
			__m128 data1 = _mm_loadu_ps(&(src[i]));
			__m128 sum = _mm_add_ps(data0, data1);
			__m128 result = _mm_mul_ps(sum, averV);
			_mm_storeu_ps(&(dst[i]), result);
		}

		averageDataNormal(&(dst[clipMax]), &(src[clipMax]), length - clipMax);
	}

	static void copyDataAVX2(float* dst, const float* src, int length) {
		int clipSize = sizeof(__m256) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		for (int i = 0; i < clipMax; i += clipSize) {
			__m256 data = _mm256_loadu_ps(&(src[i]));
			_mm256_storeu_ps(&(dst[i]), data);
		}

		copyDataNormal(&(dst[clipMax]), &(src[clipMax]), length - clipMax);
	}

	static void addDataAVX2(float* dst, const float* src, int length) {
		int clipSize = sizeof(__m256) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		for (int i = 0; i < clipMax; i += clipSize) {
			__m256 data0 = _mm256_loadu_ps(&(dst[i]));
			__m256 data1 = _mm256_loadu_ps(&(src[i]));
			__m256 result = _mm256_add_ps(data0, data1);
			_mm256_storeu_ps(&(dst[i]), result);
		}

		addDataNormal(&(dst[clipMax]), &(src[clipMax]), length - clipMax);
	}

	static void zeroDataAVX2(float* dst, int length) {
		int clipSize = sizeof(__m256) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		constexpr __m256 zeroV = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
		for (int i = 0; i < clipMax; i += clipSize) {
			_mm256_storeu_ps(&(dst[i]), zeroV);
		}

		zeroDataNormal(&(dst[clipMax]), length - clipMax);
	}

	static void averageDataAVX2(float* dst, const float* src, int length) {
		int clipSize = sizeof(__m256) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		constexpr __m256 averV = { .5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f };
		for (int i = 0; i < clipMax; i += clipSize) {
			__m256 data0 = _mm256_loadu_ps(&(dst[i]));
			__m256 data1 = _mm256_loadu_ps(&(src[i]));
			__m256 sum = _mm256_add_ps(data0, data1);
			__m256 result = _mm256_mul_ps(sum, averV);
			_mm256_storeu_ps(&(dst[i]), result);
		}

		averageDataNormal(&(dst[clipMax]), &(src[clipMax]), length - clipMax);
	}

	static void copyDataAVX512(float* dst, const float* src, int length) {
		int clipSize = sizeof(__m512) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		for (int i = 0; i < clipMax; i += clipSize) {
			__m512 data = _mm512_loadu_ps(&(src[i]));
			_mm512_storeu_ps(&(dst[i]), data);
		}

		copyDataNormal(&(dst[clipMax]), &(src[clipMax]), length - clipMax);
	}

	static void addDataAVX512(float* dst, const float* src, int length) {
		int clipSize = sizeof(__m512) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		for (int i = 0; i < clipMax; i += clipSize) {
			__m512 data0 = _mm512_loadu_ps(&(dst[i]));
			__m512 data1 = _mm512_loadu_ps(&(src[i]));
			__m512 result = _mm512_add_ps(data0, data1);
			_mm512_storeu_ps(&(dst[i]), result);
		}

		addDataNormal(&(dst[clipMax]), &(src[clipMax]), length - clipMax);
	}

	static void zeroDataAVX512(float* dst, int length) {
		int clipSize = sizeof(__m512) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		constexpr __m512 zeroV = { 
			0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
		for (int i = 0; i < clipMax; i += clipSize) {
			_mm512_storeu_ps(&(dst[i]), zeroV);
		}

		zeroDataNormal(&(dst[clipMax]), length - clipMax);
	}

	static void averageDataAVX512(float* dst, const float* src, int length) {
		int clipSize = sizeof(__m512) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		constexpr __m512 averV = { 
			.5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f };
		for (int i = 0; i < clipMax; i += clipSize) {
			__m512 data0 = _mm512_loadu_ps(&(dst[i]));
			__m512 data1 = _mm512_loadu_ps(&(src[i]));
			__m512 sum = _mm512_add_ps(data0, data1);
			__m512 result = _mm512_mul_ps(sum, averV);
			_mm512_storeu_ps(&(dst[i]), result);
		}

		averageDataNormal(&(dst[clipMax]), &(src[clipMax]), length - clipMax);
	}

	static InsType type = InsType::Normal;
	static auto copyData = copyDataNormal;
	static auto addData = addDataNormal;
	static auto zeroData = zeroDataNormal;
	static auto averageData = averageDataNormal;

	void setInsType(InsType type) {
		/** Check And Fallback */
		if (type == InsType::AVX512 && !juce::SystemStats::hasAVX512F()) {
			type = InsType::AVX2;
		}
		if (type == InsType::AVX2 && !juce::SystemStats::hasAVX2()) {
			type = InsType::SSE3;
		}
		if (type == InsType::SSE3 && !juce::SystemStats::hasSSE3()) {
			type = InsType::Normal;
		}
		vMath::type = type;

		/** Set Functions */
		constexpr std::array<decltype(vMath::copyData), InsType::MaxNum> copyDataList
			= { vMath::copyDataNormal, vMath::copyDataSSE3, vMath::copyDataAVX2, vMath::copyDataAVX512 };
		constexpr std::array<decltype(vMath::addData), InsType::MaxNum> addDataList
			= { vMath::addDataNormal, vMath::addDataSSE3, vMath::addDataAVX2, vMath::addDataAVX512 };
		constexpr std::array<decltype(vMath::zeroData), InsType::MaxNum> zeroDataList
			= { vMath::zeroDataNormal, vMath::zeroDataSSE3, vMath::zeroDataAVX2, vMath::zeroDataAVX512 };
		constexpr std::array<decltype(vMath::averageData), InsType::MaxNum> averageDataList
			= { vMath::averageDataNormal, vMath::averageDataSSE3, vMath::averageDataAVX2, vMath::averageDataAVX512 };
		
		vMath::copyData = copyDataList[type];
		vMath::addData = addDataList[type];
		vMath::zeroData = zeroDataList[type];
		vMath::averageData = averageDataList[type];
	}

	InsType getInsType() {
		return vMath::type;
	}

	constexpr std::array<const char*, InsType::MaxNum> insTypeNameList{
		"Normal", "SSE3", "AVX2", "AVX512" };

	const juce::String getInsTypeName() {
		return vMath::insTypeNameList[(int)(vMath::getInsType())];
	}

	const juce::StringArray getAllInsTypeName() {
		juce::StringArray result;
		for (auto s : insTypeNameList) {
			result.add(juce::String{ s });
		}
		return result;
	}

	void copyAudioData(juce::AudioSampleBuffer& dst, const juce::AudioSampleBuffer& src,
		int dstStartSample, int srcStartSample, int dstChannel, int srcChannel, int length) {
		auto wPtr = dst.getWritePointer(dstChannel);
		auto rPtr = src.getReadPointer(srcChannel);
		if (!wPtr || !rPtr) { return; }

		copyData(&(wPtr[dstStartSample]), &(rPtr[srcStartSample]), length);
	}

	void addAudioData(juce::AudioSampleBuffer& dst, const juce::AudioSampleBuffer& src,
		int dstStartSample, int srcStartSample, int dstChannel, int srcChannel, int length) {
		auto wPtr = dst.getWritePointer(dstChannel);
		auto rPtr = src.getReadPointer(srcChannel);
		if (!wPtr || !rPtr) { return; }

		addData(&(wPtr[dstStartSample]), &(rPtr[srcStartSample]), length);
	}

	void zeroAudioData(juce::AudioSampleBuffer& dst,
		int dstStartSample, int dstChannel, int length) {
		auto wPtr = dst.getWritePointer(dstChannel);
		if (!wPtr) { return; }

		zeroData(&(wPtr[dstStartSample]), length);
	}
	void zeroAllAudioChannels(juce::AudioSampleBuffer& dst,
		int dstStartSample, int length) {
		for (int i = 0; i < dst.getNumChannels(); i++) {
			zeroAudioData(dst, dstStartSample, i, length);
		}
	}
	void zeroAllAudioDataOnChannel(juce::AudioSampleBuffer& dst, int dstChannel) {
		zeroAudioData(dst, 0, dstChannel, dst.getNumSamples());
	}
	void zeroAllAudioData(juce::AudioSampleBuffer& dst) {
		for (int i = 0; i < dst.getNumChannels(); i++) {
			zeroAllAudioDataOnChannel(dst, i);
		}
	}
}

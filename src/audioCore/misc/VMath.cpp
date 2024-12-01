#include "VMath.h"

#if (!JUCE_MSVC) && (__SSE3__ || __AVX2__ || __AVX512F__)
#include <immintrin.h>
#endif //(!JUCE_MSVC) && (__SSE3__ || __AVX2__ || __AVX512F__)

namespace vMath {
	static void copyDataNormal(float* dst, const float* src, int length) {
		std::memcpy(dst, src, length * sizeof(float));
	}

	static void addDataNormal(float* dst, const float* src, int length) {
		for (int i = 0; i < length; i++) {
			dst[i] = dst[i] + src[i];
		}
	}

	static void fillDataNormal(float* dst, float data, int length) {
		for (int i = 0; i < length; i++) {
			dst[i] = data;
		}
	}

	static void averageDataNormal(float* dst, const float* src, int length) {
		for (int i = 0; i < length; i++) {
			dst[i] = (dst[i] + src[i]) / 2.f;
		}
	}

	static void resampleNormal(int start, float* dst, const float* src,
		int dstLength, int srcLength, double dstSampleRate, double srcSampleRate) {
		double ratio = srcSampleRate / dstSampleRate;

		dstLength = std::min(dstLength, (int)(srcLength / ratio));

		for (int i = start; i < dstLength; i++) {
			double srcIndex = i * ratio;
			int srcIndexLeft = std::floor(srcIndex);
			int srcIndexRight = std::ceil(srcIndex);
			double srcPer = srcIndex - srcIndexLeft;

			dst[i] = src[srcIndexLeft] * (1 - srcPer) + src[srcIndexRight] * srcPer;
		}
	}

#if __SSE3__ || JUCE_MSVC
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

	static void fillDataSSE3(float* dst, float data, int length) {
		int clipSize = sizeof(__m128) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		__m128 dataV = _mm_set1_ps(data);
		for (int i = 0; i < clipMax; i += clipSize) {
			_mm_storeu_ps(&(dst[i]), dataV);
		}

		fillDataNormal(&(dst[clipMax]), data, length - clipMax);
	}

	static void averageDataSSE3(float* dst, const float* src, int length) {
		int clipSize = sizeof(__m128) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		__m128 averV = _mm_set1_ps(0.5f);
		for (int i = 0; i < clipMax; i += clipSize) {
			__m128 data0 = _mm_loadu_ps(&(dst[i]));
			__m128 data1 = _mm_loadu_ps(&(src[i]));
			__m128 sum = _mm_add_ps(data0, data1);
			__m128 result = _mm_mul_ps(sum, averV);
			_mm_storeu_ps(&(dst[i]), result);
		}

		averageDataNormal(&(dst[clipMax]), &(src[clipMax]), length - clipMax);
	}

	static void resampleSSE3(int start, float* dst, const float* src,
		int dstLength, int srcLength, double dstSampleRate, double srcSampleRate) {
		double ratio = srcSampleRate / dstSampleRate;

		dstLength = std::min(dstLength, (int)(srcLength / ratio));

		int clipSize = sizeof(__m128) / sizeof(float);
		int clipNum = (dstLength - start) / clipSize;
		int clipMax = start + clipNum * clipSize;

		__m128 oneV = _mm_set1_ps(1.0f);
		__m128 zeroV = _mm_set1_ps(0.0f);
#if JUCE_MSVC
		_MM_ALIGN16 float increaseList[4] = { 0, 1, 2, 3 };
		__m128 increaseV = _mm_load_ps(increaseList);
#else
		float increaseList[4] = { 0, 1, 2, 3 };
		__m128 increaseV = _mm_loadu_ps(increaseList);
#endif
		__m128 ratioV = _mm_set1_ps(ratio);
		for (int i = start; i < clipMax; i += clipSize) {
			__m128 baseIndex = _mm_set1_ps((float)i);
			__m128 index = _mm_add_ps(baseIndex, increaseV);
			
			__m128 srcIndex = _mm_mul_ps(index, ratioV);
			__m128 srcIndexLeft = _mm_floor_ps(srcIndex);
			__m128 srcIndexRight = _mm_ceil_ps(srcIndex);
			__m128 srcPer = _mm_sub_ps(srcIndex, srcIndexLeft);

			__m128i srcIndexLeftI = _mm_cvtps_epi32(srcIndexLeft);
			__m128i srcIndexRightI = _mm_cvtps_epi32(srcIndexRight);

			__m128 srcLeftValues = _mm_i32gather_ps(src, srcIndexLeftI, sizeof(float));
			__m128 srcRightValues = _mm_i32gather_ps(src, srcIndexRightI, sizeof(float));
			__m128 rLeft = _mm_sub_ps(oneV, srcPer);
			__m128 rRight = _mm_sub_ps(srcPer, zeroV);
			__m128 dstLeftValues = _mm_mul_ps(srcLeftValues, rLeft);
			__m128 dstRightValues = _mm_mul_ps(srcRightValues, rRight);
			__m128 result = _mm_add_ps(dstLeftValues, dstRightValues);

			_mm_storeu_ps(&(dst[i]), result);
		}

		resampleNormal(clipMax, dst, src, dstLength, srcLength, dstSampleRate, srcSampleRate);
	}

#else //__SSE3__ || JUCE_MSVC
	static void copyDataSSE3(float* dst, const float* src, int length) {
		copyDataNormal(dst, src, length);
	}

	static void addDataSSE3(float* dst, const float* src, int length) {
		addDataNormal(dst, src, length);
	}

	static void fillDataSSE3(float* dst, float data, int length) {
		fillDataNormal(dst, data, length);
	}

	static void averageDataSSE3(float* dst, const float* src, int length) {
		averageDataNormal(dst, src, length);
	}

	static void resampleSSE3(int start, float* dst, const float* src,
		int dstLength, int srcLength, double dstSampleRate, double srcSampleRate) {
		resampleNormal(start, dst, src, dstLength, srcLength, dstSampleRate, srcSampleRate);
	}

#endif //__SSE3__ || JUCE_MSVC

#if __AVX2__ || JUCE_MSVC
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

	static void fillDataAVX2(float* dst, float data, int length) {
		int clipSize = sizeof(__m256) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		__m256 dataV = _mm256_set1_ps(data);
		for (int i = 0; i < clipMax; i += clipSize) {
			_mm256_storeu_ps(&(dst[i]), dataV);
		}

		fillDataNormal(&(dst[clipMax]), data, length - clipMax);
	}

	static void averageDataAVX2(float* dst, const float* src, int length) {
		int clipSize = sizeof(__m256) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		__m256 averV = _mm256_set1_ps(0.5f);
		for (int i = 0; i < clipMax; i += clipSize) {
			__m256 data0 = _mm256_loadu_ps(&(dst[i]));
			__m256 data1 = _mm256_loadu_ps(&(src[i]));
			__m256 sum = _mm256_add_ps(data0, data1);
			__m256 result = _mm256_mul_ps(sum, averV);
			_mm256_storeu_ps(&(dst[i]), result);
		}

		averageDataNormal(&(dst[clipMax]), &(src[clipMax]), length - clipMax);
	}

	static void resampleAVX2(int start, float* dst, const float* src,
		int dstLength, int srcLength, double dstSampleRate, double srcSampleRate) {
		double ratio = srcSampleRate / dstSampleRate;

		dstLength = std::min(dstLength, (int)(srcLength / ratio));

		int clipSize = sizeof(__m256) / sizeof(float);
		int clipNum = (dstLength - start) / clipSize;
		int clipMax = start + clipNum * clipSize;

		__m256 oneV = _mm256_set1_ps(1.0f);
		__m256 zeroV = _mm256_set1_ps(0.0f);
		float increaseList[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
		__m256 increaseV = _mm256_loadu_ps(increaseList);
		__m256 ratioV = _mm256_set1_ps(ratio);
		for (int i = start; i < clipMax; i += clipSize) {
			__m256 baseIndex = _mm256_set1_ps((float)i);
			__m256 index = _mm256_add_ps(baseIndex, increaseV);

			__m256 srcIndex = _mm256_mul_ps(index, ratioV);
			__m256 srcIndexLeft = _mm256_floor_ps(srcIndex);
			__m256 srcIndexRight = _mm256_ceil_ps(srcIndex);
			__m256 srcPer = _mm256_sub_ps(srcIndex, srcIndexLeft);

			__m256i srcIndexLeftI = _mm256_cvtps_epi32(srcIndexLeft);
			__m256i srcIndexRightI = _mm256_cvtps_epi32(srcIndexRight);

			__m256 srcLeftValues = _mm256_i32gather_ps(src, srcIndexLeftI, sizeof(float));
			__m256 srcRightValues = _mm256_i32gather_ps(src, srcIndexRightI, sizeof(float));
			__m256 rLeft = _mm256_sub_ps(oneV, srcPer);
			__m256 rRight = _mm256_sub_ps(srcPer, zeroV);
			__m256 dstLeftValues = _mm256_mul_ps(srcLeftValues, rLeft);
			__m256 dstRightValues = _mm256_mul_ps(srcRightValues, rRight);
			__m256 result = _mm256_add_ps(dstLeftValues, dstRightValues);

			_mm256_storeu_ps(&(dst[i]), result);
		}

		resampleNormal(clipMax, dst, src, dstLength, srcLength, dstSampleRate, srcSampleRate);
	}

#else //__AVX2__ || JUCE_MSVC
	static void copyDataAVX2(float* dst, const float* src, int length) {
		copyDataSSE3(dst, src, length);
	}

	static void addDataAVX2(float* dst, const float* src, int length) {
		addDataSSE3(dst, src, length);
	}

	static void fillDataAVX2(float* dst, float data, int length) {
		fillDataSSE3(dst, data, length);
	}

	static void averageDataAVX2(float* dst, const float* src, int length) {
		averageDataSSE3(dst, src, length);
	}

	static void resampleAVX2(int start, float* dst, const float* src,
		int dstLength, int srcLength, double dstSampleRate, double srcSampleRate) {
		resampleNormal(start, dst, src, dstLength, srcLength, dstSampleRate, srcSampleRate);
	}

#endif //__AVX2__ || JUCE_MSVC

#if __AVX512F__ || JUCE_MSVC
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

	static void fillDataAVX512(float* dst, float data, int length) {
		int clipSize = sizeof(__m512) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		__m512 dataV = _mm512_set1_ps(data);
		for (int i = 0; i < clipMax; i += clipSize) {
			_mm512_storeu_ps(&(dst[i]), dataV);
		}

		fillDataNormal(&(dst[clipMax]), data, length - clipMax);
	}

	static void averageDataAVX512(float* dst, const float* src, int length) {
		int clipSize = sizeof(__m512) / sizeof(float);
		int clipNum = length / clipSize;
		int clipMax = clipNum * clipSize;

		__m512 averV = _mm512_set1_ps(0.5f);
		for (int i = 0; i < clipMax; i += clipSize) {
			__m512 data0 = _mm512_loadu_ps(&(dst[i]));
			__m512 data1 = _mm512_loadu_ps(&(src[i]));
			__m512 sum = _mm512_add_ps(data0, data1);
			__m512 result = _mm512_mul_ps(sum, averV);
			_mm512_storeu_ps(&(dst[i]), result);
		}

		averageDataNormal(&(dst[clipMax]), &(src[clipMax]), length - clipMax);
	}

	static void resampleAVX512(int start, float* dst, const float* src,
		int dstLength, int srcLength, double dstSampleRate, double srcSampleRate) {
		double ratio = srcSampleRate / dstSampleRate;

		dstLength = std::min(dstLength, (int)(srcLength / ratio));

		int clipSize = sizeof(__m512) / sizeof(float);
		int clipNum = (dstLength - start) / clipSize;
		int clipMax = start + clipNum * clipSize;

		__m512 oneV = _mm512_set1_ps(1.0f);
		__m512 zeroV = _mm512_set1_ps(0.0f);
		float increaseList[16] = {
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
		__m512 increaseV = _mm512_loadu_ps(increaseList);
		__m512 ratioV = _mm512_set1_ps(ratio);
		for (int i = start; i < clipMax; i += clipSize) {
			__m512 baseIndex = _mm512_set1_ps((float)i);
			__m512 index = _mm512_add_ps(baseIndex, increaseV);

			__m512 srcIndex = _mm512_mul_ps(index, ratioV);
			__m512 srcIndexLeft = _mm512_floor_ps(srcIndex);
			__m512 srcIndexRight = _mm512_ceil_ps(srcIndex);
			__m512 srcPer = _mm512_sub_ps(srcIndex, srcIndexLeft);

			__m512i srcIndexLeftI = _mm512_cvtps_epi32(srcIndexLeft);
			__m512i srcIndexRightI = _mm512_cvtps_epi32(srcIndexRight);

			__m512 srcLeftValues = _mm512_i32gather_ps(srcIndexLeftI, src, sizeof(float));
			__m512 srcRightValues = _mm512_i32gather_ps(srcIndexRightI, src, sizeof(float));
			__m512 rLeft = _mm512_sub_ps(oneV, srcPer);
			__m512 rRight = _mm512_sub_ps(srcPer, zeroV);
			__m512 dstLeftValues = _mm512_mul_ps(srcLeftValues, rLeft);
			__m512 dstRightValues = _mm512_mul_ps(srcRightValues, rRight);
			__m512 result = _mm512_add_ps(dstLeftValues, dstRightValues);

			_mm512_storeu_ps(&(dst[i]), result);
		}

		resampleNormal(clipMax, dst, src, dstLength, srcLength, dstSampleRate, srcSampleRate);
	}

#else //__AVX512F__ || JUCE_MSVC
	static void copyDataAVX512(float* dst, const float* src, int length) {
		copyDataAVX2(dst, src, length);
	}

	static void addDataAVX512(float* dst, const float* src, int length) {
		addDataAVX2(dst, src, length);
	}

	static void fillDataAVX512(float* dst, float data, int length) {
		fillDataAVX2(dst, data, length);
	}

	static void averageDataAVX512(float* dst, const float* src, int length) {
		averageDataAVX2(dst, src, length);
	}

	static void resampleAVX512(int start, float* dst, const float* src,
		int dstLength, int srcLength, double dstSampleRate, double srcSampleRate) {
		resampleNormal(start, dst, src, dstLength, srcLength, dstSampleRate, srcSampleRate);
	}

#endif //__AVX512F__ || JUCE_MSVC

	static InsType type = InsType::Normal;
	static auto copyData = copyDataNormal;
	static auto addData = addDataNormal;
	static auto fillData = fillDataNormal;
	static auto averageData = averageDataNormal;
	static auto resample = resampleNormal;

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
		constexpr std::array<decltype(vMath::fillData), InsType::MaxNum> fillDataList
			= { vMath::fillDataNormal, vMath::fillDataSSE3, vMath::fillDataAVX2, vMath::fillDataAVX512 };
		constexpr std::array<decltype(vMath::averageData), InsType::MaxNum> averageDataList
			= { vMath::averageDataNormal, vMath::averageDataSSE3, vMath::averageDataAVX2, vMath::averageDataAVX512 };
		constexpr std::array<decltype(vMath::resample), InsType::MaxNum> resampleList
			= { vMath::resampleNormal, vMath::resampleSSE3, vMath::resampleAVX2, vMath::resampleAVX512 };
		
		vMath::copyData = copyDataList[type];
		vMath::addData = addDataList[type];
		vMath::fillData = fillDataList[type];
		vMath::averageData = averageDataList[type];
		vMath::resample = resampleList[type];
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

	void fillAudioData(juce::AudioSampleBuffer& dst, float data,
		int dstStartSample, int dstChannel, int length) {
		auto wPtr = dst.getWritePointer(dstChannel);
		if (!wPtr) { return; }

		fillData(&(wPtr[dstStartSample]), data, length);
	}

	void zeroAudioData(juce::AudioSampleBuffer& dst,
		int dstStartSample, int dstChannel, int length) {
		fillAudioData(dst, 0.f, dstStartSample, dstChannel, length);
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

	void resampleAudioData(juce::AudioSampleBuffer& dst, const juce::AudioSampleBuffer& src,
		int dstStartSample, int srcStartSample, int dstLength,
		double dstSampleRate, double srcSampleRate) {
		int dstChannels = dst.getNumChannels();
		int srcChannels = src.getNumChannels();
		int channels = std::min(dstChannels, srcChannels);
		for (int i = 0; i < channels; i++) {
			resampleAudioData(dst, src, dstStartSample, srcStartSample,
				i, i, dstLength, dstSampleRate, srcSampleRate);
		}
	}
	void resampleAudioData(juce::AudioSampleBuffer& dst, const juce::AudioSampleBuffer& src,
		int dstStartSample, int srcStartSample, int dstChannel, int srcChannel, int dstLength,
		double dstSampleRate, double srcSampleRate) {
		auto wPtr = dst.getWritePointer(dstChannel);
		auto rPtr = src.getReadPointer(srcChannel);
		if (!wPtr || !rPtr) { return; }

		resample(0, &(wPtr[dstStartSample]), &(rPtr[srcStartSample]), dstLength,
			src.getNumSamples() - srcStartSample, dstSampleRate, srcSampleRate);
	}
}

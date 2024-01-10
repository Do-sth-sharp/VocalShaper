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
		/** TODO */
	}

	static void addDataSSE3(float* dst, const float* src, int length) {
		/** TODO */
	}

	static void zeroDataSSE3(float* dst, int length) {
		/** TODO */
	}

	static void averageDataSSE3(float* dst, const float* src, int length) {
		/** TODO */
	}

	static void copyDataAVX2(float* dst, const float* src, int length) {
		/** TODO */
	}

	static void addDataAVX2(float* dst, const float* src, int length) {
		/** TODO */
	}

	static void zeroDataAVX2(float* dst, int length) {
		/** TODO */
	}

	static void averageDataAVX2(float* dst, const float* src, int length) {
		/** TODO */
	}

	static void copyDataAVX512(float* dst, const float* src, int length) {
		/** TODO */
	}

	static void addDataAVX512(float* dst, const float* src, int length) {
		/** TODO */
	}

	static void zeroDataAVX512(float* dst, int length) {
		/** TODO */
	}

	static void averageDataAVX512(float* dst, const float* src, int length) {
		/** TODO */
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
}

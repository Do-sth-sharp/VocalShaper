#pragma once

#include <JuceHeader.h>

namespace vMath {
	enum InsType {
		Normal, SSE3, AVX2, AVX512,
		MaxNum
	};

	void setInsType(InsType type);
	InsType getInsType();
}

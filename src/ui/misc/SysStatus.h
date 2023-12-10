#pragma once

#include <JuceHeader.h>

class SysStatus final {
public:
	static double getCPUUsage();
	static double getMemUsage();
	static uint64_t getProcMemUsage();
};

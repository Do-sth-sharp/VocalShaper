#pragma once

#include <JuceHeader.h>

class SysStatus final: private juce::DeletedAtShutdown {
public:
	SysStatus();
	~SysStatus();

	struct CPUPercTemp final {
		std::array<uint64_t, 3> cpuTemp;
	};

	double getCPUUsage(CPUPercTemp& temp);
	double getMemUsage();
	uint64_t getProcMemUsage();

private:
	void* hProcess = nullptr;/**< For Windows Only */
	uint64_t* ptrCPUIdleTime;
	uint64_t* ptrCPUTotalTime;
	uint64_t* ptrPreviousCPUIdleTime;
	uint64_t* ptrPreviousCPUTotalTime;
	void* ptrProcessorInfo;
	int nProcessors;

public:
	static SysStatus* getInstance();
	static void releaseInstance();

private:
	static SysStatus* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SysStatus)
};

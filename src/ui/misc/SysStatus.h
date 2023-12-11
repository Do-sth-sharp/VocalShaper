#pragma once

#include <JuceHeader.h>
#include <sigar.h>

class SysStatus final: private juce::DeletedAtShutdown {
public:
	SysStatus();
	~SysStatus();

	struct CPUPercTemp final {
		juce::Array<sigar_cpu_t> cpuTemp;
	};

	double getCPUUsage(CPUPercTemp& temp);
	double getMemUsage();
	uint64_t getProcMemUsage();

private:
	sigar_t* pSigar = nullptr;
	void* hProcess = nullptr;/**< For Windows Only */

public:
	static SysStatus* getInstance();
	static void releaseInstance();

private:
	static SysStatus* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SysStatus)
};

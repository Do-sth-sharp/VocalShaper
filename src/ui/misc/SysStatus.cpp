#include "SysStatus.h"
#include <cstdlib>

#if JUCE_WINDOWS
#include <Windows.h>
#include <Winternl.h>
#include <Psapi.h>
#else //JUCE_WINDOWS
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#endif //JUCE_WINDOWS

SysStatus::SysStatus() {
	/** Get Current Process */
#if JUCE_WINDOWS
	this->hProcess = GetCurrentProcess();

	SYSTEM_BASIC_INFORMATION basicInfo{};
	NtQuerySystemInformation(SystemBasicInformation, &basicInfo, sizeof(basicInfo), NULL);
	this->nProcessors = basicInfo.NumberOfProcessors;

	this->ptrProcessorInfo = malloc(sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * this->nProcessors);

#endif //JUCE_WINDOWS
}

SysStatus::~SysStatus() {

#if JUCE_WINDOWS
	if (this->ptrProcessorInfo) {
		free(this->ptrProcessorInfo);
	}

#endif //JUCE_WINDOWS

}

double SysStatus::getCPUUsage(CPUPercTemp& temp) {
#if JUCE_WINDOWS
	uint64_t nSumIdleTime = 0;
	uint64_t nSumTotalTime = 0;

	NtQuerySystemInformation(SystemProcessorPerformanceInformation, this->ptrProcessorInfo, sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * this->nProcessors, NULL);

	SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION* info = (SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION*)this->ptrProcessorInfo;

	for (int i = 0; i < this->nProcessors; i++) {
		nSumIdleTime += info[i].IdleTime.QuadPart;
		nSumTotalTime += info[i].KernelTime.QuadPart + info[i].UserTime.QuadPart;
	}

	uint64_t nDeltaCPUIdleTime = nSumIdleTime - temp.cpuTemp[1];
	uint64_t nDeltaCPUTotalTime = nSumTotalTime - temp.cpuTemp[0];

	temp.cpuTemp[1] = nSumIdleTime;
	temp.cpuTemp[0] = nSumTotalTime;

	if (nDeltaCPUTotalTime) {
		return (100 - ((nDeltaCPUIdleTime * 100.0) / nDeltaCPUTotalTime)) / 100.0;
	}

	return 0;

#else //JUCE_WINDOWS
	long total = 0, idle = 0;

	std::ifstream file("/proc/stat");
	if (file.is_open()) {
		std::string line;
		std::getline(file, line);
		std::istringstream ss(line);
		std::string cpu;
		ss >> cpu;

		long user, nice, system, idle, iowait, irq, softirq, steal;
		ss >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

		total = user + nice + system + idle + iowait + irq + softirq + steal;
		idle = idle + iowait;

		long totalDiff = total - temp.cpuTemp[0];
		long idleDiff = idle - temp.cpuTemp[1];

		float cpuUsage = 100.0 * (totalDiff - idleDiff) / totalDiff;

		temp.cpuTemp[0] = total;
		temp.cpuTemp[1] = idle;

		return cpuUsage;
	}

	return 0.0;

#endif //JUCE_WINDOWS
}

double SysStatus::getMemUsage() {
#if JUCE_WINDOWS
	MEMORYSTATUSEX memInfo{};
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);

	return (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (double)(memInfo.ullTotalPhys);

#else //JUCE_WINDOWS
	std::ifstream file("/proc/meminfo");
	std::string line;
	unsigned long total = 0, free = 0, buffers = 0, cached = 0;

	while (std::getline(file, line)) {
		if (line.find("MemTotal:") == 0) {
			total = std::stoul(line.substr(line.find_first_of("0123456789")));
		}
		else if (line.find("MemFree:") == 0) {
			free = std::stoul(line.substr(line.find_first_of("0123456789")));
		}
		else if (line.find("Buffers:") == 0) {
			buffers = std::stoul(line.substr(line.find_first_of("0123456789")));
		}
		else if (line.find("Cached:") == 0) {
			cached = std::stoul(line.substr(line.find_first_of("0123456789")));
		}
	}

	return (total - free - buffers - cached) / (double)total;

#endif //JUCE_WINDOWS
}

uint64_t SysStatus::getProcMemUsage() {
#if JUCE_WINDOWS
	PROCESS_MEMORY_COUNTERS pmc;
	if (GetProcessMemoryInfo(this->hProcess, &pmc, sizeof(pmc))) {
		return pmc.WorkingSetSize;
	}
	return 0;

#else //JUCE_WINDOWS
	std::ifstream file("/proc/self/statm");
	unsigned long size;
	file >> size;

	return static_cast<float>(size) * sysconf(_SC_PAGESIZE);

#endif //JUCE_WINDOWS
}

SysStatus* SysStatus::getInstance() {
	return SysStatus::instance ? SysStatus::instance
		: (SysStatus::instance = new SysStatus{});
}

void SysStatus::releaseInstance() {
	if (SysStatus::instance) {
		delete SysStatus::instance;
		SysStatus::instance = nullptr;
	}
}

SysStatus* SysStatus::instance = nullptr;

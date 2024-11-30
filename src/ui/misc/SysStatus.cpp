#include "SysStatus.h"
#include <cstdlib>

#if JUCE_WINDOWS
#include <Windows.h>
#include <Winternl.h>
#include <Psapi.h>

#pragma comment(lib,"ntdll.lib")
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

	SYSTEM_BASIC_INFORMATION BasicInfo;
	NtQuerySystemInformation(SystemBasicInformation, &BasicInfo, sizeof(BasicInfo), NULL);
	this->Processors = BasicInfo.NumberOfProcessors;

	this->ProcessorInfo = malloc(sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * this->Processors);

	this->CPUIdleTime = (uint64_t*)malloc(sizeof(uint64_t) * this->Processors);
	this->CPUTotalTime = (uint64_t*)malloc(sizeof(uint64_t) * this->Processors);

	memset(this->CPUIdleTime, 0, sizeof(uint64_t) * this->Processors);
	memset(this->CPUTotalTime, 0, sizeof(uint64_t) * this->Processors);

	this->PreviousCPUIdleTime = (uint64_t*)malloc(sizeof(uint64_t) * this->Processors);
	this->PreviousCPUTotalTime = (uint64_t*)malloc(sizeof(uint64_t) * this->Processors);

	memset(this->PreviousCPUIdleTime, 0, sizeof(uint64_t) * this->Processors);
	memset(this->PreviousCPUTotalTime, 0, sizeof(uint64_t) * this->Processors);

#endif //JUCE_WINDOWS
}

SysStatus::~SysStatus() {

#if JUCE_WINDOWS
	if (this->ProcessorInfo) {
		free(this->ProcessorInfo);
	}

	if (this->CPUIdleTime) {
		free(this->CPUIdleTime);
	}
	if (this->CPUTotalTime) {
		free(this->CPUTotalTime);
	}

	if (this->PreviousCPUIdleTime) {
		free(this->PreviousCPUIdleTime);
	}
	if (this->PreviousCPUTotalTime) {
		free(this->PreviousCPUTotalTime);
	}

#endif //JUCE_WINDOWS

}

double SysStatus::getCPUUsage(CPUPercTemp& temp) {
#if JUCE_WINDOWS
	uint64_t SumIdleTime = 0;
	uint64_t SumTotalTime = 0;

	NtQuerySystemInformation(SystemProcessorPerformanceInformation, this->ProcessorInfo, sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * Processors, NULL);

	SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION* info = (SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION*)this->ProcessorInfo;

	for (int i = 0; i < Processors; i++)
	{
		uint64_t DeltaCPUIdleTime;
		uint64_t DeltaCPUTotalTime;

		this->CPUIdleTime[i] = info[i].IdleTime.QuadPart;
		this->CPUTotalTime[i] = info[i].KernelTime.QuadPart + info[i].UserTime.QuadPart;

		DeltaCPUIdleTime = this->CPUIdleTime[i] - this->PreviousCPUIdleTime[i];
		DeltaCPUTotalTime = this->CPUTotalTime[i] - this->PreviousCPUTotalTime[i];

		SumIdleTime += DeltaCPUIdleTime;
		SumTotalTime += DeltaCPUTotalTime;

		this->PreviousCPUIdleTime[i] = this->CPUIdleTime[i];
		this->PreviousCPUTotalTime[i] = this->CPUTotalTime[i];
	}

	if (SumTotalTime != 0)
    {
        return (100 - ((SumIdleTime * 100) / SumTotalTime)) / 100.0;
    }
    else
    {
        return 0;
    }

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

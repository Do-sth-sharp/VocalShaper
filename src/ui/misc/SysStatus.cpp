#include "SysStatus.h"

#if JUCE_WINDOWS
#include <Windows.h>
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

#endif //JUCE_WINDOWS
}

SysStatus::~SysStatus() {}

double SysStatus::getCPUUsage(CPUPercTemp& temp) {
#if JUCE_WINDOWS
	FILETIME newIdleTime, newKernelTime, newUserTime;
	GetSystemTimes(&newIdleTime, &newKernelTime, &newUserTime);

	uint64_t newIdleTimeTemp = (*(ULARGE_INTEGER*)&newIdleTime).QuadPart;
	uint64_t newKernelTimeTemp = (*(ULARGE_INTEGER*)&newKernelTime).QuadPart;
	uint64_t newUserTimeTemp = (*(ULARGE_INTEGER*)&newUserTime).QuadPart;

	uint64_t idle = newIdleTimeTemp - temp.cpuTemp[0];
	uint64_t kernel = newKernelTimeTemp - temp.cpuTemp[1];
	uint64_t user = newUserTimeTemp - temp.cpuTemp[2];

	temp.cpuTemp[0] = newIdleTimeTemp;
	temp.cpuTemp[1] = newKernelTimeTemp;
	temp.cpuTemp[2] = newUserTimeTemp;

	return (kernel + user) / (double)(idle + kernel + user);

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

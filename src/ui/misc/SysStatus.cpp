#include "SysStatus.h"

extern "C" {
#include <sigar_format.h>
}

SysStatus::SysStatus() {
	/** Open Sigar */
	sigar_open(&this->pSigar);
}

SysStatus::~SysStatus() {
	/** Clear Sigar */
	sigar_close(this->pSigar);
}

double SysStatus::getCPUUsage(CPUPercTemp& temp) {
	/** Check Sigar */
	if (!this->pSigar) { return 0; }

	/** Get CPU List */
	sigar_cpu_list_t cpuList = {};
	sigar_cpu_list_get(this->pSigar, &cpuList);

	/** Get CPU Prec */
	double total = 0;
	int cpuNum = cpuList.number;
	temp.cpuTemp.resize(cpuNum);
	for (int i = 0; i < cpuNum; i++) {
		sigar_cpu_t* cpu = &(cpuList.data[i]);
		sigar_cpu_perc_t perc = {};
		sigar_cpu_perc_calculate(
			&(temp.cpuTemp.getRawDataPointer()[i]), cpu, &perc);

		total += perc.combined;
		temp.cpuTemp.getReference(i) = *cpu;
	}
	total /= cpuNum;

	/** Clear */
	sigar_cpu_list_destroy(this->pSigar, &cpuList);
	
	return total;
}

double SysStatus::getMemUsage() {
	/** Check Sigar */
	if (!this->pSigar) { return 0; }

	/** Get Mem */
	sigar_mem_t mem = {};
	sigar_mem_get(this->pSigar, &mem);

	/** Get Result */
	double result = mem.used_percent / 100;

	return result;
}

uint64_t SysStatus::getProcMemUsage() {
	/** Check Sigar */
	if (!this->pSigar) { return 0; }

	/** Get PID */
	sigar_pid_t pid = sigar_pid_get(this->pSigar);

	/** Get Mem */
	sigar_proc_mem_t mem = {};
	sigar_proc_mem_get(this->pSigar, pid, &mem);

	/** Get Result */
	uint64_t result = mem.size;

	return result;
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

#include "SysStatus.h"

#include <sigar.h>

double SysStatus::getCPUUsage() {
	/** Open Sigar */
	sigar_t* pSigar = nullptr;
	sigar_open(&pSigar);
	if (!pSigar) { return 0; }

	/** Get CPU List */
	sigar_cpu_list_t cpuList = {};
	sigar_cpu_list_get(pSigar, &cpuList);

	/** Get CPU Prec */
	double total = 0;
	int cpuNum = cpuList.number;
	for (int i = 0; i < cpuNum; i++) {
		sigar_cpu_t* cpu = &(cpuList.data[i]);
		total += ((cpu->total - cpu->idle) / (double)cpu->total);
	}
	total /= cpuNum;

	/** Clear */
	sigar_cpu_list_destroy(pSigar, &cpuList);
	sigar_close(pSigar);

	return total;
}

double SysStatus::getMemUsage() {
	/** Open Sigar */
	sigar_t* pSigar = nullptr;
	sigar_open(&pSigar);
	if (!pSigar) { return 0; }

	/** Get Mem */
	sigar_mem_t mem = {};
	sigar_mem_get(pSigar, &mem);

	/** Get Result */
	double result = mem.used_percent;

	/** Clear */
	sigar_close(pSigar);

	return result;
}

uint64_t SysStatus::getProcMemUsage() {
	/** Open Sigar */
	sigar_t* pSigar = nullptr;
	sigar_open(&pSigar);
	if (!pSigar) { return 0; }

	/** Get PID */
	sigar_pid_t pid = sigar_pid_get(pSigar);

	/** Get Mem */
	sigar_proc_mem_t mem = {};
	sigar_proc_mem_get(pSigar, pid, &mem);

	/** Get Result */
	uint64_t result = mem.size;

	/** Clear */
	sigar_close(pSigar);

	return result;
}

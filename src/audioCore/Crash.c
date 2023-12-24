#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

#include "recovery/DataGet.h"

#ifdef WIN32
#include <Windows.h>
#include <DbgHelp.h>
#endif // WIN32

extern char* audioCoreCreateProjectDataBlock();
extern void audioCoreFreeProjectDataBlock(char* mem);
extern size_t audioCoreGetProjectDataSize(const char* mem);
extern int audioCoreCopyProjectData(char* dst, size_t size, const char* mem);

static int saveAsFile(const char* path, const char* data, size_t size) {
	int ret = 1;

	FILE* outFile = fopen(path, "w");
	if (!outFile) { goto error0; }

	if (fwrite(data, 1, size, outFile) != size) {
		goto error1;
	}

done:
	ret = 0;
error1:
	fclose(outFile);
error0:
	return ret;
}

int audioCoreHighLayerDataRecovery(const char* path) {
	int ret = 1;

	char* project = audioCoreCreateProjectDataBlock();
	if (!project) { goto error0; }

	size_t size = audioCoreGetProjectDataSize(project);
	char* data = malloc(size);
	if (!data) { goto error1; }

	if (!audioCoreCopyProjectData(data, size, project)) {
		goto error2;
	}

	if (saveAsFile(path, data, size) != 0) {
		goto error2;
	}

done:
	ret = 0;
error2:
	free(data);
error1:
	audioCoreFreeProjectDataBlock(project);
error0:
	return ret;
}

int audioCoreMidLayerDataRecovery(const char* path) {
	int ret = 1;

	size_t size = getRecoverySize();
	char* data = getRecoveryData();
	if (!data) { goto error0; }

	if (saveAsFile(path, data, size) != 0) {
		goto error0;
	}

done:
	ret = 0;
error0:
	return ret;
}

int audioCoreLowLayerDataRecovery(const char* path, void* info) {
#if WIN32
	int ret = 1;

	HANDLE lhDumpFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (lhDumpFile == NULL) { goto error0; }

	MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
	loExceptionInfo.ExceptionPointers = info;
	loExceptionInfo.ThreadId = GetCurrentThreadId();
	loExceptionInfo.ClientPointers = TRUE;

	if (MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
		lhDumpFile, MiniDumpWithFullMemory, &loExceptionInfo, NULL, NULL) != TRUE) {
		goto error1;
	}

done:
	ret = 0;
error1:
	CloseHandle(lhDumpFile);
error0:
	return ret;

#else
	return 1;

#endif //WIN32
}

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

extern char* audioCoreCreateProjectDataBlock();
extern void audioCoreFreeProjectDataBlock(char* mem);
extern int audioCoreGetProjectDataSize(const char* mem);
extern int audioCoreCopyProjectData(char* dst, int size, const char* mem);

int audioCoreHighLayerDataRecovery(const char* path) {
	int ret = 1;

	char* project = audioCoreCreateProjectDataBlock();
	if (!project) { return 1; }

	int size = audioCoreGetProjectDataSize(project);
	char* data = malloc(size);
	if (!data) { goto error0; }

	if (!audioCoreCopyProjectData(data, size, project)) {
		goto error1;
	}

	FILE* outFile = fopen(path, "w");
	if (!outFile) { goto error1; }

	if (fwrite(data, size, 1, outFile) == 0) {
		goto error2;
	}

done:
	ret = 0;
error2:
	fclose(outFile);
error1:
	free(data);
error0:
	audioCoreFreeProjectDataBlock(project);
	return ret;
}

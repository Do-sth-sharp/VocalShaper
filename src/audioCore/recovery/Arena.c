#include "Arena.h"

#include <malloc.h>

#define ARENA_FIRST_ALLOC_SIZE 64

void arenaCreate(Arena* a) {
	if (!a) { return; }
	a->ptr = malloc(ARENA_FIRST_ALLOC_SIZE);
	a->sizeAllocated = ARENA_FIRST_ALLOC_SIZE;
	a->sizeUsed = 0;
	a->sizeNextAlloc = ARENA_FIRST_ALLOC_SIZE * 2;
}

void arenaDestory(Arena* a) {
	if (!a) { return; }
	free(a->ptr);
	a->ptr = NULL;
	a->sizeAllocated = 0;
	a->sizeUsed = 0;
	a->sizeNextAlloc = 0;
}

void arenaClearQuick(Arena* a) {
	if (!a) { return; }
	a->sizeUsed = 0;
}

char* arenaAlloc(Arena* a, size_t size) {
	if (!a) { return NULL; }

	/** Allocated Enough */
	if (a->sizeAllocated >= (a->sizeUsed + size)) {
		goto getPointer;
	}

	/** Allocate Next Block */
	a->ptr = realloc(a->ptr, (a->sizeAllocated += a->sizeNextAlloc));
	a->sizeNextAlloc *= 2;

getPointer:
	char* ptr = &((a->ptr)[a->sizeUsed]);
	a->sizeUsed += size;
	return ptr;
}

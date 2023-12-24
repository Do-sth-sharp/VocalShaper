#pragma once

#include <stdint.h>

typedef struct {
	char* ptr;
	size_t sizeUsed, sizeAllocated;
	size_t sizeNextAlloc;
} Arena;

extern void arenaCreate(Arena* a);
extern void arenaDestory(Arena* a);
extern void arenaClearQuick(Arena* a);
extern char* arenaAlloc(Arena* a, size_t size);

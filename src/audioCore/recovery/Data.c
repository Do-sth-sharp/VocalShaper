#include "Arena.h"

#include <memory.h>
#include <stdbool.h>

static Arena recoveryMemBlock;

void initRecoveryMemoryBlock() {
	arenaCreate(&recoveryMemBlock);
}

void destoryRecoveryMemoryBlock() {
	arenaDestory(&recoveryMemBlock);
}

void resetRecoveryMemoryBlock() {
	arenaClearQuick(&recoveryMemBlock);
}

#define WRITE_TO_ARENA(a, v) memcpy(arenaAlloc((a), sizeof(v)), &(v), sizeof(v))

void writeRecoveryFloatValue(float value) {
	WRITE_TO_ARENA(&recoveryMemBlock, value);
}

void writeRecoveryDoubleValue(double value) {
	WRITE_TO_ARENA(&recoveryMemBlock, value);
}

void writeRecoveryBoolValue(bool value) {
	WRITE_TO_ARENA(&recoveryMemBlock, value);
}

void writeRecoveryInt8Value(int8_t value) {
	WRITE_TO_ARENA(&recoveryMemBlock, value);
}

void writeRecoveryUInt8Value(uint8_t value) {
	WRITE_TO_ARENA(&recoveryMemBlock, value);
}

void writeRecoveryInt16Value(int16_t value) {
	WRITE_TO_ARENA(&recoveryMemBlock, value);
}

void writeRecoveryUInt16Value(uint16_t value) {
	WRITE_TO_ARENA(&recoveryMemBlock, value);
}

void writeRecoveryInt32Value(int32_t value) {
	WRITE_TO_ARENA(&recoveryMemBlock, value);
}

void writeRecoveryUInt32Value(uint32_t value) {
	WRITE_TO_ARENA(&recoveryMemBlock, value);
}

void writeRecoveryInt64Value(int64_t value) {
	WRITE_TO_ARENA(&recoveryMemBlock, value);
}

void writeRecoveryUInt64Value(uint64_t value) {
	WRITE_TO_ARENA(&recoveryMemBlock, value);
}

void writeRecoverySizeValue(size_t value) {
	WRITE_TO_ARENA(&recoveryMemBlock, value);
}

void writeRecoveryDataBlockValue(const char* data, size_t size) {
	char* ptr = arenaAlloc(&recoveryMemBlock, size);
	memcpy(ptr, data, size);
}

void writeRecoveryStringValue(const char* data, size_t size) {
	writeRecoverySizeValue(size);
	writeRecoveryDataBlockValue(data, size);
}

size_t getRecoverySize() {
	return recoveryMemBlock.sizeUsed;
}

char* getRecoveryData() {
	return recoveryMemBlock.ptr;
}

size_t copyRecoveryData(char* dst, size_t size) {
	return memcpy(dst, recoveryMemBlock.ptr, size)
		? size : 0;
}

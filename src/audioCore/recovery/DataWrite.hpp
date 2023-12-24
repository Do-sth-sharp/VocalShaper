#pragma once

#include <cstdint>
#include <string>

extern "C" void writeRecoveryFloatValue(float value);
extern "C" void writeRecoveryDoubleValue(double value);
extern "C" void writeRecoveryBoolValue(bool value);
extern "C" void writeRecoveryInt8Value(int8_t value);
extern "C" void writeRecoveryUInt8Value(uint8_t value);
extern "C" void writeRecoveryInt16Value(int16_t value);
extern "C" void writeRecoveryUInt16Value(uint16_t value);
extern "C" void writeRecoveryInt32Value(int32_t value);
extern "C" void writeRecoveryUInt32Value(uint32_t value);
extern "C" void writeRecoveryInt64Value(int64_t value);
extern "C" void writeRecoveryUInt64Value(uint64_t value);
extern "C" void writeRecoverySizeValue(size_t value);
extern "C" void writeRecoveryDataBlockValue(const char* data, size_t size);
extern "C" void writeRecoveryStringValue(const char* data, size_t size);

#define writeRecoveryCharValue writeRecoveryInt8Value
#define writeRecoveryUCharValue writeRecoveryUInt8Value
#define writeRecoveryShortValue writeRecoveryInt16Value
#define writeRecoveryUShortValue writeRecoveryUInt16Value
#define writeRecoveryIntValue writeRecoveryInt32Value
#define writeRecoveryUIntValue writeRecoveryUInt32Value
#define writeRecoveryLongValue writeRecoveryInt32Value
#define writeRecoveryULongValue writeRecoveryUInt32Value
#define writeRecoveryLongLongValue writeRecoveryInt64Value
#define writeRecoveryULongLongValue writeRecoveryUInt64Value

#define writeRecoveryActionCode writeRecoveryUIntValue

#define writeRecoveryActionCodeDo(code) writeRecoveryActionCode((unsigned int)(code) | (0U << (sizeof(unsigned int) * 8 - 1)))
#define writeRecoveryActionCodeUndo(code) writeRecoveryActionCode((unsigned int)(code) | (1U << (sizeof(unsigned int) * 8 - 1)))

void writeRecoveryStringValue(const std::string& str);
bool writeRecoveryActionResult(bool result);

template<typename T>
void writeRecoveryBlockValue(const T& block) {
	writeRecoverySizeValue(sizeof(T));
	writeRecoveryDataBlockValue((const char*)(&block), sizeof(T));
}
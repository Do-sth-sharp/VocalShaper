#pragma once

#include <cstdint>

/** Save Project Data With Protobuf. This Function Maybe Cause Another Crash. */
extern "C" int audioCoreHighLayerDataRecovery(const char* path);

/** Save Project History. */
extern "C" int audioCoreMidLayerDataRecovery(const char* path);

/** Save Memory Dump. */
extern "C" int audioCoreLowLayerDataRecovery(const char* path, void* info);
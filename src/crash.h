#pragma once

#include <JuceHeader.h>

void applicationCrashHandler(void*);

const juce::Array<juce::File> getAllDumpFiles();

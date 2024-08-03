#pragma once

#include <JuceHeader.h>

void applicationCrashHandler(void*);

const juce::Array<juce::File> getAllDumpFiles();

void initCrashHandler(const juce::String& path);

using UICrashCallback = std::function<void(void)>;
void setUICrashCallback(const UICrashCallback& callback);
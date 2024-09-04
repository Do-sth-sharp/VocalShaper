#pragma once

#include <JuceHeader.h>

bool checkAdmin();
bool regProjectFileInSystem(const juce::String& appPath);
bool unregProjectFileFromSystem();
void refreshSystemIconCache();

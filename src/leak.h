#pragma once

#include <JuceHeader.h>

#ifdef _DEBUG && JUCE_MSVC && !JUCE_CHECK_MEMORY_LEAKS
#define newd new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define newd new
#endif

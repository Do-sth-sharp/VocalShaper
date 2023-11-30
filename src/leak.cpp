#include <JuceHeader.h>

#if JUCE_MSVC && !JUCE_CHECK_MEMORY_LEAKS
struct DebugFlagsInitialiser
{
    DebugFlagsInitialiser()
    {
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    }
};

static DebugFlagsInitialiser debugFlagsInitialiser;
#endif

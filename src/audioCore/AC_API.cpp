#include "AC_API.h"

#include "AudioCore.h"

void shutdownAudioCore() {
	AudioCore::releaseInstance();
}

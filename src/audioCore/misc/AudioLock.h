#pragma once

#include <JuceHeader.h>

namespace audioLock {
	juce::ReadWriteLock& getLock();
}

#pragma once

#include <JuceHeader.h>

namespace audioLock {
	juce::ReadWriteLock& getAudioLock();
	juce::ReadWriteLock& getSourceLock();
	juce::ReadWriteLock& getPluginLock();
	juce::ReadWriteLock& getPositionLock();
	juce::ReadWriteLock& getMackieLock();
	juce::ReadWriteLock& getLevelMeterLock();
}

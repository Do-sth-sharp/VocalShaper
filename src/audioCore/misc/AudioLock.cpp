#include "AudioLock.h"

namespace audioLock {
	class LockHelper final : private juce::DeletedAtShutdown {
	public:
		juce::ReadWriteLock lock;
	};

	static LockHelper* lock = new LockHelper;

	juce::ReadWriteLock& getLock() {
		return lock->lock;
	}
}

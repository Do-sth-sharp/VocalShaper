#include "PlayPosition.h"

juce::Optional<juce::AudioPlayHead::PositionInfo> PlayPosition::getPosition() const {
	/** TODO Get Play Position */
	return juce::Optional<juce::AudioPlayHead::PositionInfo>{};
}

PlayPosition* PlayPosition::getInstance() {
	juce::GenericScopedLock locker{ PlayPosition::instanceGuard };
	return PlayPosition::instance ? PlayPosition::instance : (PlayPosition::instance = new PlayPosition());
}

void PlayPosition::releaseInstance() {
	juce::GenericScopedLock locker{ PlayPosition::instanceGuard };
	if (PlayPosition::instance) {
		delete PlayPosition::instance;
		PlayPosition::instance = nullptr;
	}
}

PlayPosition* PlayPosition::instance = nullptr;
juce::CriticalSection PlayPosition::instanceGuard;

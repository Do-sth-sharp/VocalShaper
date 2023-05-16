#pragma once

#include <JuceHeader.h>

class PlayPosition final : public juce::AudioPlayHead,
	private juce::DeletedAtShutdown {
public:
	PlayPosition() = default;
	~PlayPosition()override = default;

private:
	juce::Optional<juce::AudioPlayHead::PositionInfo> getPosition() const override;

public:
	static PlayPosition* getInstance();
	static void releaseInstance();

private:
	static PlayPosition* instance;
	static juce::CriticalSection instanceGuard;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayPosition)
};

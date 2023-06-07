#pragma once

#include <JuceHeader.h>

class PlayPosition final : public juce::AudioPlayHead,
	private juce::DeletedAtShutdown {
public:
	PlayPosition() = default;
	~PlayPosition()override = default;

	juce::Optional<juce::AudioPlayHead::PositionInfo> getPosition() const override;

	bool canControlTransport() override;
	void transportPlay(bool shouldStartPlaying) override;
	void transportRecord(bool shouldStartRecording) override;
	void transportRewind() override;

private:
	mutable juce::AudioPlayHead::PositionInfo position;
	juce::ReadWriteLock lock;

public:
	static PlayPosition* getInstance();
	static void releaseInstance();

private:
	static PlayPosition* instance;
	static juce::CriticalSection instanceGuard;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayPosition)
};

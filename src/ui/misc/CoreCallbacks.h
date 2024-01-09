#pragma once

#include <JuceHeader.h>

class CoreCallbacks final : private juce::DeletedAtShutdown {
public:
	CoreCallbacks();

	using ErrorCallback = std::function<void(const juce::String&, const juce::String&)>;
	void addError(const ErrorCallback& callback);
	using PlayingStatusCallback = std::function<void(bool)>;
	void addPlayingStatus(const PlayingStatusCallback& callback);
	using RecordingStatusCallback = std::function<void(bool)>;
	void addRecordingStatus(const RecordingStatusCallback& callback);

	void invokeError(const juce::String& title, const juce::String& mes) const;
	void invokePlayingStatus(bool status) const;
	void invokeRecordingStatus(bool status) const;

private:
	juce::Array<ErrorCallback> error;
	juce::Array<PlayingStatusCallback> playingStatus;
	juce::Array<RecordingStatusCallback> recordingingStatus;

public:
	static CoreCallbacks* getInstance();
	static void releaseInstance();

private:
	static CoreCallbacks* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreCallbacks)
};

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
	using ErrorMesCallback = std::function<void(const juce::String&)>;
	void addErrorMes(const ErrorMesCallback& callback);
	using SearchPluginCallback = std::function<void(bool)>;
	void addSearchPlugin(const SearchPluginCallback& callback);
	using SourceChangedCallback = std::function<void(int)>;
	void addSourceChanged(const SourceChangedCallback& callback);
	using InstrChangedCallback = std::function<void(int)>;
	void addInstrChanged(const InstrChangedCallback& callback);
	using TrackChangedCallback = std::function<void(int)>;
	void addTrackChanged(const TrackChangedCallback& callback);

	void invokeError(const juce::String& title, const juce::String& mes) const;
	void invokePlayingStatus(bool status) const;
	void invokeRecordingStatus(bool status) const;
	void invokeErrorMes(const juce::String& mes) const;
	void invokeSearchPlugin(bool status) const;
	void invokeSourceChanged(int index) const;
	void invokeInstrChanged(int index) const;
	void invokeTrackChanged(int index) const;

private:
	juce::Array<ErrorCallback> error;
	juce::Array<PlayingStatusCallback> playingStatus;
	juce::Array<RecordingStatusCallback> recordingingStatus;
	juce::Array<ErrorMesCallback> errorMes;
	juce::Array<SearchPluginCallback> searchPlugin;
	juce::Array<SourceChangedCallback> sourceChanged;
	juce::Array<InstrChangedCallback> instrChanged;
	juce::Array<TrackChangedCallback> trackChanged;

public:
	static CoreCallbacks* getInstance();
	static void releaseInstance();

private:
	static CoreCallbacks* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreCallbacks)
};

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
	void addTrackGainChanged(const TrackChangedCallback& callback);
	void addTrackPanChanged(const TrackChangedCallback& callback);
	void addTrackFaderChanged(const TrackChangedCallback& callback);
	void addTrackMuteChanged(const TrackChangedCallback& callback);
	using EffectChangedCallback = std::function<void(int, int)>;
	void addEffectChanged(const EffectChangedCallback& callback);
	using SeqChangedCallback = std::function<void(int)>;
	void addSeqChanged(const SeqChangedCallback& callback);
	using SeqBlockChangedCallback = std::function<void(int, int)>;
	void addSeqBlockChanged(const SeqBlockChangedCallback& callback);
	using TempoChangedCallback = std::function<void(void)>;
	void addTempoChanged(const TempoChangedCallback& callback);

	void invokeError(const juce::String& title, const juce::String& mes) const;
	void invokePlayingStatus(bool status) const;
	void invokeRecordingStatus(bool status) const;
	void invokeErrorMes(const juce::String& mes) const;
	void invokeSearchPlugin(bool status) const;
	void invokeSourceChanged(int index) const;
	void invokeInstrChanged(int index) const;
	void invokeTrackChanged(int index) const;
	void invokeTrackGainChanged(int index) const;
	void invokeTrackPanChanged(int index) const;
	void invokeTrackFaderChanged(int index) const;
	void invokeTrackMuteChanged(int index) const;
	void invokeEffectChanged(int track, int index) const;
	void invokeSeqChanged(int index) const;
	void invokeSeqBlockChanged(int track, int index) const;
	void invokeTempoChanged() const;

private:
	juce::Array<ErrorCallback> error;
	juce::Array<PlayingStatusCallback> playingStatus;
	juce::Array<RecordingStatusCallback> recordingingStatus;
	juce::Array<ErrorMesCallback> errorMes;
	juce::Array<SearchPluginCallback> searchPlugin;
	juce::Array<SourceChangedCallback> sourceChanged;
	juce::Array<InstrChangedCallback> instrChanged;
	juce::Array<TrackChangedCallback> trackChanged;
	juce::Array<TrackChangedCallback> trackGainChanged;
	juce::Array<TrackChangedCallback> trackPanChanged;
	juce::Array<TrackChangedCallback> trackFaderChanged;
	juce::Array<TrackChangedCallback> trackMuteChanged;
	juce::Array<EffectChangedCallback> effectChanged;
	juce::Array<SeqChangedCallback> seqChanged;
	juce::Array<SeqBlockChangedCallback> seqBlockChanged;
	juce::Array<TempoChangedCallback> tempoChanged;

public:
	static CoreCallbacks* getInstance();
	static void releaseInstance();

private:
	static CoreCallbacks* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreCallbacks)
};

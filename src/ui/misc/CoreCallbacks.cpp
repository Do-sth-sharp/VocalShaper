#include "CoreCallbacks.h"
#include "../../audioCore/AC_API.h"

CoreCallbacks::CoreCallbacks() {
	UICallbackAPI<const juce::String&, const juce::String&>::set(UICallbackType::ErrorAlert,
		[](const juce::String& title, const juce::String& mes) {
			CoreCallbackAPI<const juce::String&, const juce::String&>::invoke(UICallbackType::ErrorAlert, title, mes);
		});
	UICallbackAPI<bool>::set(UICallbackType::PlayStateChanged,
		[](bool status) {
			CoreCallbackAPI<bool>::invoke(UICallbackType::PlayStateChanged, status);
		});
	UICallbackAPI<bool>::set(UICallbackType::RecordStateChanged,
		[](bool status) {
			CoreCallbackAPI<bool>::invoke(UICallbackType::RecordStateChanged, status);
		});
	UICallbackAPI<const juce::String&>::set(UICallbackType::ErrorMessage,
		[](const juce::String& mes) {
			CoreCallbackAPI<const juce::String&>::invoke(UICallbackType::ErrorMessage, mes);
		});
	UICallbackAPI<bool>::set(UICallbackType::PluginSearchStateChanged,
		[](bool state) {
			CoreCallbackAPI<bool>::invoke(UICallbackType::PluginSearchStateChanged, state);
		});
	UICallbackAPI<void>::set(UICallbackType::TempoChanged,
		[] {
			CoreCallbackAPI<void>::invoke(UICallbackType::TempoChanged);
		});
	UICallbackAPI<const juce::String&>::set(UICallbackType::PluginSearchMessage,
		[](const juce::String& mes) {
			CoreCallbackAPI<const juce::String&>::invoke(UICallbackType::PluginSearchMessage, mes);
		});

	UICallbackAPI<void>::set(UICallbackType::GraphUpdated,
		[] {
			CoreCallbackAPI<void>::invoke(UICallbackType::GraphUpdated);
		});
	UICallbackAPI<int, int>::set(UICallbackType::TrackAdded,
		[](int type, int index) {
			CoreCallbackAPI<int, int>::invoke(UICallbackType::TrackAdded, type, index);
		});
	UICallbackAPI<int, int>::set(UICallbackType::TrackRemoved,
		[](int type, int index) {
			CoreCallbackAPI<int, int>::invoke(UICallbackType::TrackRemoved, type, index);
		});
	UICallbackAPI<int, int>::set(UICallbackType::TrackInfoChanged,
		[](int type, int index) {
			CoreCallbackAPI<int, int>::invoke(UICallbackType::TrackInfoChanged, type, index);
		});
	UICallbackAPI<int, int>::set(UICallbackType::TrackSideChainChanged,
		[](int type, int index) {
			CoreCallbackAPI<int, int>::invoke(UICallbackType::TrackSideChainChanged, type, index);
		});
	UICallbackAPI<int, int>::set(UICallbackType::TrackInputConnectionChanged,
		[](int type, int index) {
			CoreCallbackAPI<int, int>::invoke(UICallbackType::TrackInputConnectionChanged, type, index);
		});
	UICallbackAPI<int, int>::set(UICallbackType::TrackSendConnectionChanged,
		[](int type, int index) {
			CoreCallbackAPI<int, int>::invoke(UICallbackType::TrackSendConnectionChanged, type, index);
		});
	UICallbackAPI<int, int>::set(UICallbackType::TrackMuteSoloChanged,
		[](int type, int index) {
			CoreCallbackAPI<int, int>::invoke(UICallbackType::TrackMuteSoloChanged, type, index);
		});

	UICallbackAPI<int, int>::set(UICallbackType::TrackBlockChanged,
		[](int track, int index) {
			CoreCallbackAPI<int, int>::invoke(UICallbackType::TrackBlockChanged, track, index);
		});
	UICallbackAPI<int>::set(UICallbackType::TrackInstrChanged,
		[](int track) {
			CoreCallbackAPI<int>::invoke(UICallbackType::TrackInstrChanged, track);
		});
	UICallbackAPI<int>::set(UICallbackType::TrackDataRefChanged,
		[](int track) {
			CoreCallbackAPI<int>::invoke(UICallbackType::TrackDataRefChanged, track);
		});
	UICallbackAPI<int>::set(UICallbackType::TrackSourceChanged,
		[](int track) {
			CoreCallbackAPI<int>::invoke(UICallbackType::TrackSourceChanged, track);
		});
	UICallbackAPI<int>::set(UICallbackType::TrackRecordingChanged,
		[](int track) {
			CoreCallbackAPI<int>::invoke(UICallbackType::TrackRecordingChanged, track);
		});
	UICallbackAPI<int>::set(UICallbackType::TrackInputMonitoringChanged,
		[](int track) {
			CoreCallbackAPI<int>::invoke(UICallbackType::TrackInputMonitoringChanged, track);
		});

	UICallbackAPI<int, int>::set(UICallbackType::TrackGainChanged,
		[](int type, int index) {
			CoreCallbackAPI<int, int>::invoke(UICallbackType::TrackGainChanged, type, index);
		});
	UICallbackAPI<int, int>::set(UICallbackType::TrackPanChanged,
		[](int type, int index) {
			CoreCallbackAPI<int, int>::invoke(UICallbackType::TrackPanChanged, type, index);
		});
	UICallbackAPI<int, int>::set(UICallbackType::TrackFaderChanged,
		[](int type, int index) {
			CoreCallbackAPI<int, int>::invoke(UICallbackType::TrackFaderChanged, type, index);
		});
	UICallbackAPI<int, int, int>::set(UICallbackType::TrackEffectChanged,
		[](int trackType, int trackIndex, int index) {
			CoreCallbackAPI<int, int, int>::invoke(
				UICallbackType::TrackEffectChanged, trackType, trackIndex, index);
		});
	UICallbackAPI<int, int, int, int>::set(UICallbackType::TrackEffectIndexChanged,
		[](int trackType, int trackIndex, int oldIndex, int newIndex) {
			CoreCallbackAPI<int, int, int, int>::invoke(
				UICallbackType::TrackEffectIndexChanged, trackType, trackIndex, oldIndex, newIndex);
		});
}

void CoreCallbacks::addCallback(CallbackType type,
	std::unique_ptr<CoreCallbackWrapperBase> wrapper) {
	this->list[(int)type].add(std::move(wrapper));
}

const juce::OwnedArray<CoreCallbackWrapperBase>& CoreCallbacks::getCallbackList(CallbackType type) const {
	return this->list[(int)type];
}

CoreCallbacks* CoreCallbacks::getInstance() {
	return CoreCallbacks::instance ? CoreCallbacks::instance
		: (CoreCallbacks::instance = new CoreCallbacks{});
}

void CoreCallbacks::releaseInstance() {
	if (CoreCallbacks::instance) {
		delete CoreCallbacks::instance;
		CoreCallbacks::instance = nullptr;
	}
}

CoreCallbacks* CoreCallbacks::instance = nullptr;

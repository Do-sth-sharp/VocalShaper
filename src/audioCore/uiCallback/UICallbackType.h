#pragma once

enum class UICallbackType : int {
	ErrorAlert,
	PlayStateChanged,
	RecordStateChanged,
	ErrorMessage,
	PluginSearchStateChanged,
	TempoChanged,
	PluginSearchMessage,

	GraphUpdated,
	TrackAdded,
	TrackRemoved,
	TrackInfoChanged,
	TrackSideChainChanged,
	TrackInputConnectionChanged,
	TrackSendConnectionChanged,
	TrackMuteSoloChanged,

	TrackBlockChanged,
	TrackInstrChanged,
	TrackDataRefChanged,
	TrackSourceChanged,
	TrackRecordingChanged,
	TrackInputMonitoringChanged,

	TrackGainChanged,
	TrackPanChanged,
	TrackFaderChanged,
	TrackEffectChanged,
	TrackEffectIndexChanged,

	TypeMaxNum
};

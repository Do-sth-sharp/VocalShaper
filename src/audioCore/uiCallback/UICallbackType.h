#pragma once

enum class UICallbackType : int {
	ErrorAlert,
	PlayStateChanged,
	RecordStateChanged,
	ErrorMessage,
	PluginSearchStateChanged,
	SourceChanged,
	InstrChanged,
	TrackChanged,

	TypeMaxNum
};

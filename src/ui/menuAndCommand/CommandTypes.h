#pragma once

enum class CoreCommandType : int {
	NewProject = 0x2000,
	OpenProject,
	SaveProject,

	LoadSource,
	LoadSynthSource,
	SaveSource,
	ExportSource,

	Render,

	Undo,
	Redo
};

enum class GUICommandType : int {
	CloseEditor = 0x3000,

	Copy,
	Cut,
	Paste,
	Clipboard,
	SelectAll,
	Delete,

	LoadLayout,
	SaveLayout,
	PluginView,
	SourceView,
	TrackView,
	InstrView,
	MixerView,
	SourceEditView,
	AudioDebugger,
	MidiDebugger
};

#pragma once

enum class CoreCommandType : int {
	NewProject = 0x2000,
	OpenProject,
	SaveProject,

	LoadSource = 0x2100,
	LoadSynthSource,
	SaveSource,
	ExportSource,

	Render = 0x2200,

	Undo = 0x2300,
	Redo
};

enum class GUICommandType : int {
	CloseEditor = 0x3000,

	Copy = 0x3100,
	Cut,
	Paste,
	Clipboard,
	SelectAll,
	Delete,

	LoadLayout = 0x3200,
	SaveLayout,
	PluginView,
	SourceView,
	TrackView,
	InstrView,
	MixerView,
	SourceEditView,
	SourceRecordView,
	AudioDebugger,
	MidiDebugger,

	StartupConfig = 0x3500,
	FunctionConfig,
	AudioConfig,
	OutputConfig,
	PluginConfig,
	KeyMappingConfig,

	Help = 0x3600,
	Update,
	Bilibili,
	Github,
	Website,
	RegProj,
	UnregProj,
	License,
	About
};

#pragma once

enum class CoreCommandType : int {
	NewProject = 0x2000,
	OpenProject,
	SaveProject,

	Render = 0x2100,

	Undo = 0x2200,
	Redo,

	Play = 0x2300,
	Stop,
	Record,
	Rewind
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
	TrackView,
	MixerView,
	ResourceEditView,
	AudioDebugger,
	MidiDebugger,

	Follow = 0x3400,

	Arrow = 0x3410,
	Pencil,

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
	About,
	MakeCrash
};

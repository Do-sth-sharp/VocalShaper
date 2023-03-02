require "luce.Debug"
require "luce.Colours"
require "luce.FlowComponent"
require "luce.FlowWindow"
require "luce.ResizableWindow"

luce.initialise = function(commandLineParameters)
	-- Load Config And Theme
	conf = require("ui.Config")
	ui = require("ui.themes." .. conf.theme)

	-- Set UI Style
	require("ui.FlowStyle")
	updateFlowColor()
	require("ui.MenuStyle")
	updateMenuColor()
	require("ui.AlertStyle")
	updateAlertColor()

	-- Set Window Background Color
	luce.Desktop.getInstance():getDefaultLookAndFeel():setColour(
			luce.ResizableWindow.ColourIds.backgroundColourId, ui.color.themeColorB2)

	-- Set FlowUI Button ICON
	luce.FlowStyle.setButtonLeftIcon("./RemixIcon/Design/layout-left-2-line.svg")
	luce.FlowStyle.setButtonRightIcon("./RemixIcon/Design/layout-right-2-line.svg")
	luce.FlowStyle.setButtonTopIcon("./RemixIcon/Design/layout-top-2-line.svg")
	luce.FlowStyle.setButtonBottomIcon("./RemixIcon/Design/layout-bottom-2-line.svg")
	luce.FlowStyle.setButtonAdsorbCenterIcon("./RemixIcon/Editor/link.svg")
	luce.FlowStyle.setButtonAdsorbLeftIcon("./RemixIcon/Design/layout-left-2-line.svg")
	luce.FlowStyle.setButtonAdsorbRightIcon("./RemixIcon/Design/layout-right-2-line.svg")
	luce.FlowStyle.setButtonAdsorbTopIcon("./RemixIcon/Design/layout-top-2-line.svg")
	luce.FlowStyle.setButtonAdsorbBottomIcon("./RemixIcon/Design/layout-bottom-2-line.svg")

	-- Set Flow Window ICON
	luce.FlowWindow.setIcon("./rc/logo.png")
	-- Flow Window Use OpenGL Renderer
	luce.FlowWindow.setOpenGL(true)

	-- Load UI Translate
	luce.LocalisedStrings.loadData("./translates/" .. conf.language .. "/")
	-- Set UI Font
	luce.Desktop.getInstance():getDefaultLookAndFeel():setDefaultSansSerifTypeface(
		"./fonts/" .. conf.font .. ".ttf")

	-- Create Flow Components
	startMenu = luce.FlowComponent.new("StartMenu")
	toolBar = luce.FlowComponent.new("ToolBar")
	resourceView = luce.FlowComponent.new("Resource")
	patternView = luce.FlowComponent.new("Pattern")
	trackView = luce.FlowComponent.new("Track")
	audioDebugger = require("ui.AudioDebugger")
	midiDebugger = require("ui.MIDIDebugger")

	-- Auto Layout Flow Components
	luce.FlowWindow.layout("./layouts/" .. conf.layout .. ".json",
		toolBar, resourceView, patternView, trackView, audioDebugger, midiDebugger)

	-- Set Main Window Full Screen
	local windowNum = luce.FlowWindow.getWindowNum()
	if windowNum > 0 then
		local mainWindow = luce.FlowWindow.getWindow(0)
		mainWindow:setFullScreen(true)
	end
end

luce.shutdown = function()
	-- Shutdown FlowUI At Application Shutdown To Avoid Memory Leak
	luce.FlowWindow.shutdown()
end

luce.anotherInstanceStarted = function(commandLine)
	-- TODO Another App Instance Started
end
require "luce.Debug"
require "luce.Colours"
require "luce.FlowComponent"
require "luce.FlowWindow"
require "luce.ResizableWindow"

luce.initialise = function(commandLineParameters)
	conf = require("ui.Config")
	ui = require("ui.themes." .. conf.theme)

	require("ui.FlowStyle")
	updateFlowColor()
	require("ui.MenuStyle")
	updateMenuColor()
	require("ui.AlertStyle")
	updateAlertColor()

	luce.Desktop.getInstance():getDefaultLookAndFeel():setColour(
			luce.ResizableWindow.ColourIds.backgroundColourId, ui.color.themeColorB2)

	luce.FlowStyle.setButtonLeftIcon("./RemixIcon/Design/layout-left-2-line.svg")
	luce.FlowStyle.setButtonRightIcon("./RemixIcon/Design/layout-right-2-line.svg")
	luce.FlowStyle.setButtonTopIcon("./RemixIcon/Design/layout-top-2-line.svg")
	luce.FlowStyle.setButtonBottomIcon("./RemixIcon/Design/layout-bottom-2-line.svg")
	luce.FlowStyle.setButtonAdsorbCenterIcon("./RemixIcon/Editor/link.svg")
	luce.FlowStyle.setButtonAdsorbLeftIcon("./RemixIcon/Design/layout-left-2-line.svg")
	luce.FlowStyle.setButtonAdsorbRightIcon("./RemixIcon/Design/layout-right-2-line.svg")
	luce.FlowStyle.setButtonAdsorbTopIcon("./RemixIcon/Design/layout-top-2-line.svg")
	luce.FlowStyle.setButtonAdsorbBottomIcon("./RemixIcon/Design/layout-bottom-2-line.svg")

	luce.FlowWindow.setIcon("./rc/logo.png")
	luce.FlowWindow.setOpenGL(true)

	luce.LocalisedStrings.loadData("./translates/" .. conf.language .. "/")
	luce.Desktop.getInstance():getDefaultLookAndFeel():setDefaultSansSerifTypeface(
		"./fonts/" .. conf.font .. ".ttf")

	startMenu = luce.FlowComponent.new("StartMenu")
	toolBar = luce.FlowComponent.new("ToolBar")
	resourceView = luce.FlowComponent.new("Resource")
	patternView = luce.FlowComponent.new("Pattern")
	trackView = luce.FlowComponent.new("Track")

	luce.FlowWindow.layout("./layouts/" .. conf.layout .. ".json",
		toolBar, resourceView, patternView, trackView)

	local windowNum = luce.FlowWindow.getWindowNum()
	if windowNum > 0 then
		local mainWindow = luce.FlowWindow.getWindow(0)
		mainWindow:setFullScreen(true)
	end

	audioDeviceList = luce.AudioCore.getAllAudioDeviceList()
	for k, v in ipairs(audioDeviceList) do
		print(v)
	end
end

luce.shutdown = function()
	luce.FlowWindow.shutdown()
end

luce.anotherInstanceStarted = function(commandLine)
end
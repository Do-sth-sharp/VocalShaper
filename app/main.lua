require "luce.Debug"
require "luce.Colours"
require "luce.FlowComponent"
require "luce.ResizableWindow"

luce.initialise = function(commandLineParameters)
	conf = require("ui.Config")
	ui = require("ui.themes." .. conf.theme)

	require("ui.FlowStyle")

	updateFlowColor()

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

	local mainWindow = luce.FlowWindow.new()
	mainWindow:setFullScreen(true)

	comp1 = luce.FlowComponent.new("comp1")
	mainWindow:openComponent(comp1)
	comp2 = luce.FlowComponent.new("comp2")
	mainWindow:openComponent(comp2)
	comp3 = luce.FlowComponent.new("comp3")
	mainWindow:openComponent(comp3)
end

luce.shutdown = function()
	luce.FlowWindow.shutdown()
end

luce.anotherInstanceStarted = function(commandLine)
end
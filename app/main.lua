require "luce.Debug"
require "luce.Colours"
require "luce.FlowComponent"

luce.initialise = function(commandLineParameters)
	luce.FlowStyle.setButtonLeftIcon("./RemixIcon/Design/layout-left-2-line.svg")
	luce.FlowStyle.setButtonRightIcon("./RemixIcon/Design/layout-right-2-line.svg")
	luce.FlowStyle.setButtonTopIcon("./RemixIcon/Design/layout-top-2-line.svg")
	luce.FlowStyle.setButtonBottomIcon("./RemixIcon/Design/layout-bottom-2-line.svg")
	luce.FlowStyle.setButtonAdsorbCenterIcon("./RemixIcon/Editor/link.svg")
	luce.FlowStyle.setButtonAdsorbLeftIcon("./RemixIcon/Design/layout-left-2-line.svg")
	luce.FlowStyle.setButtonAdsorbRightIcon("./RemixIcon/Design/layout-right-2-line.svg")
	luce.FlowStyle.setButtonAdsorbTopIcon("./RemixIcon/Design/layout-top-2-line.svg")
	luce.FlowStyle.setButtonAdsorbBottomIcon("./RemixIcon/Design/layout-bottom-2-line.svg")

	local mainWindow = luce.FlowWindow.new()
end

luce.shutdown = function()
	luce.FlowWindow.shutdown()
end

luce.anotherInstanceStarted = function(commandLine)
end
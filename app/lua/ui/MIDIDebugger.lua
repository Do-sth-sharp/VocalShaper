require "luce.FlowComponent"

-- Create FlowComponent To Contain The Debugger
local debugger = luce.FlowComponent.new("MIDIDebugger")

-- Get The Debugger Pointer
debugger:data().ptrComp = luce.AudioCore.getMIDIDebugger()

-- Add The Debugger To FlowComponent As Child
debugger:addAndMakeVisible(debugger:data().ptrComp)

-- Set The Resized Method Of The FlowComponent
debugger:data().resized = function(self)
	debugger:data().ptrComp:setBounds(0, 0, self:getWidth(), self:getHeight())
end

-- Return The FlowComponent
return debugger
require "luce.AlertWindow"
require "luce.TextButton"
require "luce.ComboBox"

function updateAlertColor()
	local laf = luce.Desktop.getInstance():getDefaultLookAndFeel()

	laf:setColour(
			luce.AlertWindow.ColourIds.backgroundColourId, ui.color.themeColorB3)
	laf:setColour(
			luce.AlertWindow.ColourIds.textColourId, ui.color.themeColorD2)
	laf:setColour(
			luce.AlertWindow.ColourIds.outlineColourId, ui.color.themeColorB3)
	laf:setColour(
			luce.TextButton.ColourIds.buttonColourId, ui.color.themeColorA2)
	laf:setColour(
			luce.TextButton.ColourIds.buttonOnColourId, ui.color.themeColorA2)
	laf:setColour(
			luce.TextButton.ColourIds.textColourOffId, ui.color.themeColorD1)
	laf:setColour(
			luce.TextButton.ColourIds.textColourOnId, ui.color.themeColorD1)
	laf:setColour(
			luce.ComboBox.ColourIds.outlineColourId, luce.Colour.new(0, 0, 0, 0))
end
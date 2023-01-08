require "luce.PopupMenu"

function updateMenuColor()
	local laf = luce.Desktop.getInstance():getDefaultLookAndFeel()

	laf:setColour(
			luce.PopupMenu.ColourIds.backgroundColourId, ui.color.themeColorB4)
	laf:setColour(
			luce.PopupMenu.ColourIds.textColourId, ui.color.themeColorD2)
	laf:setColour(
			luce.PopupMenu.ColourIds.headerTextColourId, ui.color.themeColorD2)
	laf:setColour(
			luce.PopupMenu.ColourIds.highlightedBackgroundColourId, ui.color.themeColorB3)
	laf:setColour(
			luce.PopupMenu.ColourIds.highlightedTextColourId, ui.color.themeColorD2)
end
#pragma once

#include <JuceHeader.h>

class MenuBarModel {
public:
	MenuBarModel() = default;
	virtual ~MenuBarModel() = default;

	virtual const juce::StringArray getMenuBarNames() = 0;
	virtual juce::PopupMenu getMenuForIndex(
		int topLevelMenuIndex, const juce::String& menuName) = 0;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuBarModel)
};

#pragma once

#include <JuceHeader.h>

class MainMenuModel final : public juce::MenuBarModel {
public:
	MainMenuModel();

	juce::StringArray getMenuBarNames() override;
	juce::PopupMenu getMenuForIndex(
		int topLevelMenuIndex, const juce::String& menuName) override;
	void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
	void menuBarActivated(bool isActive) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainMenuModel)
};

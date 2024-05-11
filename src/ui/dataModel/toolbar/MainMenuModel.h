#pragma once

#include <JuceHeader.h>
#include "MenuBarModel.h"

class MainMenuModel final : public MenuBarModel {
public:
	MainMenuModel();

	const juce::StringArray getMenuBarNames() override;
	juce::PopupMenu getMenuForIndex(
		int topLevelMenuIndex, const juce::String& menuName) override;

private:
	juce::PopupMenu createFile() const;
	juce::PopupMenu createEdit() const;
	juce::PopupMenu createView() const;
	juce::PopupMenu createProject() const;
	juce::PopupMenu createControl() const;
	juce::PopupMenu createConfig() const;
	juce::PopupMenu createMisc() const;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainMenuModel)
};

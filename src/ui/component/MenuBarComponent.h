#pragma once

#include <JuceHeader.h>
#include "../dataModel/MenuBarModel.h"

class MenuBarComponent final : public juce::Component {
public:
	MenuBarComponent() = delete;
	MenuBarComponent(MenuBarModel* model);

	void setModel(MenuBarModel* model);

	void resized() override;

private:
	MenuBarModel* model = nullptr;
	juce::OwnedArray<juce::TextButton> buttons;
	std::unique_ptr<juce::TextButton> moreButton = nullptr;
	int buttonShownNum = 0;

	void updateSize();
	void updateButtons();
	void clearButtons();

	void showMoreMenu();
	juce::PopupMenu getMoreMenu();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuBarComponent)
};

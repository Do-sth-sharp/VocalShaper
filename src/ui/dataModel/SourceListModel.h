#pragma once

#include <JuceHeader.h>

class SourceListModel final : public juce::ListBoxModel {
public:
	SourceListModel(const std::function<void(int)>& selectCallback);

	int getNumRows() override;
	void paintListBoxItem(int rowNumber, juce::Graphics& g,
		int width, int height, bool rowIsSelected) override;
	juce::Component* refreshComponentForRow(
		int rowNumber, bool isRowSelected,
		juce::Component* existingComponentToUpdate) override;
	juce::String getNameForRow(int rowNumber) override;
	void backgroundClicked(const juce::MouseEvent&) override;
	juce::String getTooltipForRow(int row) override;
	juce::MouseCursor getMouseCursorForRow(int row) override;

private:
	const std::function<void(int)> selectCallback;

	void showBackgroundMenu();

	juce::PopupMenu createBackgroundMenu() const;
	juce::PopupMenu createNewMenu() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceListModel)
};

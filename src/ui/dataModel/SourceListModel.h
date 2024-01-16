#pragma once

#include <JuceHeader.h>

class SourceListModel final : public juce::ListBoxModel {
public:
	SourceListModel(const juce::Component* parent);

	int getNumRows() override;
	void paintListBoxItem(int rowNumber, juce::Graphics& g,
		int width, int height, bool rowIsSelected) override;
	juce::Component* refreshComponentForRow(
		int rowNumber, bool isRowSelected,
		juce::Component* existingComponentToUpdate) override;
	juce::String getNameForRow(int rowNumber) override;
	void listBoxItemClicked(int row, const juce::MouseEvent&) override;
	void backgroundClicked(const juce::MouseEvent&) override;
	void deleteKeyPressed(int lastRowSelected) override;
	juce::var getDragSourceDescription(
		const juce::SparseSet<int>& rowsToDescribe) override;
	bool mayDragToExternalWindows() const override;
	juce::String getTooltipForRow(int row) override;
	juce::MouseCursor getMouseCursorForRow(int row) override;

private:
	const juce::Component* const parent;

	void showItemMenu(int index);
	void showBackgroundMenu();
	void deleteItem(int index);

	juce::PopupMenu createItemMenu(int index) const;
	juce::PopupMenu createBackgroundMenu() const;
	juce::PopupMenu createNewMenu() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceListModel)
};

#pragma once

#include <JuceHeader.h>

class InstrListModel final : public juce::ListBoxModel {
public:
	InstrListModel() = default;

	int getNumRows() override;
	void paintListBoxItem(int rowNumber, juce::Graphics& g,
		int width, int height, bool rowIsSelected) override;
	juce::Component* refreshComponentForRow(int rowNumber, bool isRowSelected,
		juce::Component* existingComponentToUpdate) override;
	juce::String getNameForRow(int rowNumber) override;
	void backgroundClicked(const juce::MouseEvent&) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrListModel)
};

#pragma once

#include <JuceHeader.h>

class EffectListModel final : public juce::ListBoxModel {
public:
	EffectListModel() = default;

	int getNumRows() override;
	void paintListBoxItem(int rowNumber, juce::Graphics& g,
		int width, int height, bool rowIsSelected) override;
	juce::Component* refreshComponentForRow(int rowNumber, bool isRowSelected,
		juce::Component* existingComponentToUpdate) override;
	juce::String getNameForRow(int rowNumber) override;
	void backgroundClicked(const juce::MouseEvent&) override;

	void update(int index);

private:
	int index = -1;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectListModel)
};

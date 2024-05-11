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
	void backgroundClicked(const juce::MouseEvent& event) override;

	void update(int index);

private:
	int index = -1;

	juce::PopupMenu createBackgroundMenu(
		const std::function<void(const juce::PluginDescription&)>& callback);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectListModel)
};

#pragma once

#include <JuceHeader.h>

class ConfigListModel final : public juce::ListBoxModel {
public:
	ConfigListModel();

	int getNumRows() override;
	void paintListBoxItem(
		int rowNumber, juce::Graphics& g,
		int width, int height, bool rowIsSelected) override;
	juce::String getNameForRow(int rowNumber) override;
	void selectedRowsChanged(int lastRowSelected) override;

	using RowCallback = std::function<void(int)>;
	void setCallback(const RowCallback& callback);

private:
	const juce::StringArray list;
	juce::LookAndFeel& lookAndFeel;
	RowCallback callback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigListModel)
};

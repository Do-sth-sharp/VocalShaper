#pragma once

#include <JuceHeader.h>

class LicenseListModel final : public juce::ListBoxModel {
public:
	LicenseListModel();

	int getNumRows() override;
	void paintListBoxItem(
		int rowNumber, juce::Graphics& g,
		int width, int height, bool rowIsSelected) override;
	juce::String getNameForRow(int rowNumber) override;
	void selectedRowsChanged(int lastRowSelected) override;
	juce::String getTooltipForRow(int row) override;

	void setList(const juce::Array<juce::File>& list);
	using RowCallback = std::function<void(const juce::File&)>;
	void setCallback(const RowCallback& callback);

private:
	juce::LookAndFeel& lookAndFeel;
	juce::Array<juce::File> fileList;
	RowCallback callback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LicenseListModel)
};

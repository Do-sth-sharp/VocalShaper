#pragma once

#include <JuceHeader.h>

class MetaDataModel final : public juce::TableListBoxModel {
public:
	MetaDataModel(const juce::StringPairArray& data);

	int getNumRows() override;
	void paintRowBackground(juce::Graphics& g, int rowNumber,
		int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics&, int rowNumber, int columnId,
		int width, int height, bool rowIsSelected) override;
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event) override;
	void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& event) override;
	void backgroundClicked(const juce::MouseEvent& event) override;
	int getColumnAutoSizeWidth(int columnId) override;
	void deleteKeyPressed(int lastRowSelected) override;

private:
	const juce::StringPairArray data;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetaDataModel)
};

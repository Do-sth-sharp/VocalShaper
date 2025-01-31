#pragma once

#include <JuceHeader.h>

class TrackListBoxModel final : public juce::ListBoxModel {
public:
	TrackListBoxModel();

	int getNumRows() override;
	void paintListBoxItem(int rowNumber, juce::Graphics& g,
		int width, int height, bool rowIsSelected) override;

	/** Type, Index, Name, Bus */
	using TrackInfo = std::tuple<int, int, juce::String, juce::String>;
	void setItems(const juce::Array<TrackInfo>& items);

private:
	juce::Array<TrackInfo> trackItemList;
	juce::LookAndFeel& lookAndFeel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListBoxModel)
};

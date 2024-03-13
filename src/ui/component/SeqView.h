#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "Scroller.h"

class SeqView final
	: public flowUI::FlowComponent,
	public juce::DragAndDropContainer {
public:
	SeqView();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update(int index);
	void updateBlock(int track, int index);

private:
	std::unique_ptr<Scroller> hScroller = nullptr;
	std::unique_ptr<Scroller> vScroller = nullptr;

	juce::OwnedArray<juce::Component> trackList;
	juce::Array<juce::Colour> colorTemp;

	/** Track Index, Start, End */
	using BlockItem = std::tuple<int, double, double>;
	juce::Array<BlockItem> blockTemp;
	double totalLength = 0;

	int getViewWidth() const;
	double getTimeLength() const;
	std::tuple<double, double> getTimeWidthLimit() const;

	void updateHPos(double pos, double itemSize);
	void paintBlockPreview(juce::Graphics& g,
		int width, int height, bool vertical);

	int getViewHeight() const;
	int getTrackNum() const;
	std::tuple<double, double> getTrackHeightLimit() const;

	void updateVPos(double pos, double itemSize);
	void paintTrackPreview(juce::Graphics& g, int itemIndex,
		int width, int height, bool vertical);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqView)
};

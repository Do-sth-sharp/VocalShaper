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

private:
	std::unique_ptr<Scroller> hScroller = nullptr;
	std::unique_ptr<Scroller> vScroller = nullptr;

	int getViewWidth() const;
	double getTimeLength() const;
	std::tuple<double, double> getTimeWidthLimit() const;

	void updateHPos(double pos, double itemSize);

	int getViewHeight() const;
	int getTrackNum() const;
	std::tuple<double, double> getTrackHeightLimit() const;

	void updateVPos(double pos, double itemSize);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqView)
};

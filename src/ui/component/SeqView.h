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

private:
	std::unique_ptr<Scroller> hScroller = nullptr;
	std::unique_ptr<Scroller> vScroller = nullptr;

	int getViewWidth() const;
	double getTimeLength() const;
	std::tuple<int, int> getTimeWidthLimit() const;

	void updateHPos(int pos, int itemSize);

	int getViewHeight() const;
	int getTrackNum() const;
	std::tuple<int, int> getTrackHeightLimit() const;

	void updateVPos(int pos, int itemSize);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqView)
};

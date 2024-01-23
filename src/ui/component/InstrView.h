#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "../dataModel/InstrListModel.h"

class InstrView final 
	: public flowUI::FlowComponent,
	public juce::DragAndDropContainer,
	public juce::DragAndDropTarget {
public:
	InstrView();

	void resized() override;
	void paint(juce::Graphics& g) override;
	void paintOverChildren(juce::Graphics& g) override;

	void update();

	bool isInterestedInDragSource(
		const SourceDetails& dragSourceDetails) override;
	void itemDragEnter(const SourceDetails& dragSourceDetails) override;
	void itemDragMove(const SourceDetails& dragSourceDetails) override;
	void itemDragExit(const SourceDetails& dragSourceDetails) override;
	void itemDropped(const SourceDetails& dragSourceDetails) override;

private:
	std::unique_ptr<juce::ListBox> instrList = nullptr;
	std::unique_ptr<InstrListModel> instrListModel = nullptr;

	/** Index, YPos */
	std::tuple<int, int> dropItemState = { -1, 0 };

	void preDrop(const juce::Point<int>& pos);
	void endDrop();

	int getInsertIndex(const juce::Point<int>& pos);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrView)
};

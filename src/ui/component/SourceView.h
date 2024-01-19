#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>

#include "../dataModel/SourceListModel.h"

class SourceView final
	: public flowUI::FlowComponent,
	public juce::DragAndDropContainer {
public:
	SourceView();
	~SourceView();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update();
	void select(int index);

private:
	std::unique_ptr<juce::ListBox> list = nullptr;
	std::unique_ptr<SourceListModel> sources = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceView)
};

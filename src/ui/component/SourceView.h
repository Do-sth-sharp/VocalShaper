#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>

class SourceView final
	: public flowUI::FlowComponent,
	public juce::DragAndDropContainer {
public:
	SourceView();

	void resized() override;
	void paint(juce::Graphics& g) override;

private:
	std::unique_ptr<juce::ListBox> list = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceView)
};

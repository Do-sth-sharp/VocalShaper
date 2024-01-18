#pragma once

#include <JuceHeader.h>

class SourceComponent final
	: public juce::Component,
	public juce::DragAndDropTarget {
public:
	SourceComponent();

	void update(int index, bool selected);

	bool isInterestedInDragSource(
		const SourceDetails& dragSourceDetails) override;
	void itemDropped(const SourceDetails& dragSourceDetails) override;

private:
	int index = -1, id = -1;
	int type = 0;
	juce::String name, typeName;
	double length = 0;
	int channels = 0, tracks = 0;
	int events = 0;
	juce::String synthesizer;
	double sampleRate = 0;
	bool selected = false;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceComponent)
};

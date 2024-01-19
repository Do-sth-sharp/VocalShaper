#pragma once

#include <JuceHeader.h>

class SourceComponent final
	: public juce::Component,
	public juce::DragAndDropTarget {
public:
	SourceComponent(const std::function<void(int)>& selectCallback);

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update(int index, bool selected);

	bool isInterestedInDragSource(
		const SourceDetails& dragSourceDetails) override;
	void itemDragEnter(const SourceDetails& dragSourceDetails) override;
	void itemDragExit(const SourceDetails& dragSourceDetails) override;
	void itemDropped(const SourceDetails& dragSourceDetails) override;

	void mouseDown(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;

private:
	const std::function<void(int)> selectCallback;

	std::unique_ptr<juce::TextEditor> nameEditor = nullptr;
	int index = -1, id = -1;
	int type = 0;
	juce::String typeName;
	double length = 0;
	int channels = 0, tracks = 0;
	int events = 0;
	juce::String synthesizer;
	double sampleRate = 0;
	bool selected = false, itemDragAccepted = false;
	bool isIOTask = false, isSynthing = false;

	void updateName(const juce::String& name);

	void preDrop();
	void endDrop();

	void selectThis();
	void showSourceMenu();
	void startDrag();

	juce::PopupMenu createSourceMenu() const;
	juce::PopupMenu createNewMenu() const;

	juce::var getDragSourceDescription() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceComponent)
};

#pragma once

#include <JuceHeader.h>
#include "InstrIOComponent.h"

class InstrComponent final
	: public juce::Component,
	public juce::SettableTooltipClient,
	public juce::DragAndDropTarget {
public:
	InstrComponent();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update(int index);

	void mouseUp(const juce::MouseEvent& event) override;

	bool isInterestedInDragSource(
		const SourceDetails& dragSourceDetails) override;
	void itemDragEnter(const SourceDetails& dragSourceDetails) override;
	void itemDragExit(const SourceDetails& dragSourceDetails) override;
	void itemDropped(const SourceDetails& dragSourceDetails) override;

private:
	int index = -1;
	juce::String name;
	bool editorOpened = false;
	bool dragHovered = false;

	std::unique_ptr<juce::Drawable> bypassIcon = nullptr;
	std::unique_ptr<juce::Drawable> bypassIconOn = nullptr;
	std::unique_ptr<juce::DrawableButton> bypassButton = nullptr;

	std::unique_ptr<InstrIOComponent> input = nullptr;
	std::unique_ptr<InstrIOComponent> output = nullptr;

	void preDrop();
	void endDrop();

	void bypass();
	void editorShow();
	void showMenu();

	juce::String createToolTip() const;
	juce::PopupMenu createMenu() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrComponent)
};

#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class PluginViewLookAndFeel : public MainLookAndFeel {
public:
	PluginViewLookAndFeel();

	void drawTreeviewPlusMinusBox(juce::Graphics& g, const juce::Rectangle<float>& area,
		juce::Colour backgroundColour, bool isOpen, bool isMouseOver) override;
	bool areLinesDrawnForTreeView(juce::TreeView&) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginViewLookAndFeel)
};

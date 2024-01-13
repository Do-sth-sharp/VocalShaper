#pragma once

#include <JuceHeader.h>

class MainLookAndFeel : public juce::LookAndFeel_V4 {
public:
	MainLookAndFeel();

	void drawTableHeaderColumn(juce::Graphics& g, juce::TableHeaderComponent& header,
		const juce::String& columnName, int columnId,
		int width, int height, bool isMouseOver, bool isMouseDown,
		int columnFlags) override;

	void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
	void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
		const bool isSeparator, const bool isActive,
		const bool isHighlighted, const bool isTicked,
		const bool hasSubMenu, const juce::String& text,
		const juce::String& shortcutKeyText,
		const juce::Drawable* icon, const juce::Colour* const textColourToUse) override;

	juce::Font getTextButtonFont(juce::TextButton& b, int) override;

	void drawCallOutBoxBackground(
		juce::CallOutBox&, juce::Graphics&, const juce::Path&, juce::Image&) override;
	int getCallOutBoxBorderSize(const juce::CallOutBox&) override;
	float getCallOutBoxCornerSize(const juce::CallOutBox&) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLookAndFeel)
};

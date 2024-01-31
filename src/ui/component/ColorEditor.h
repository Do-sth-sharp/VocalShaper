#pragma once

#include <JuceHeader.h>

class ColorEditorContent final : public juce::Component {
public:
	using Callback = std::function<void(const juce::Colour&)>;

public:
	ColorEditorContent() = delete;
	ColorEditorContent(const Callback& callback,
		const juce::Colour& defaultColor);

private:
	const Callback callback;
	const juce::Array<juce::Colour> themeColors;
	const juce::Array<juce::Colour> themeAlertColors;
	const juce::Array<juce::Colour> historyList;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorEditorContent)
};

class ColorEditor final : public juce::DocumentWindow {
	using Callback = ColorEditorContent::Callback;

public:
	ColorEditor() = delete;
	ColorEditor(const Callback& callback,
		const juce::Colour& defaultColor);

	void closeButtonPressed() override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorEditor)
};

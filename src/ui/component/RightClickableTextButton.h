#pragma once

#include <JuceHeader.h>

class RightClickableTextButton : public juce::TextButton {
public:
	using ClickCallback = std::function<void(void)>;
	RightClickableTextButton() = delete;
	RightClickableTextButton(
		const juce::String& text,
		const ClickCallback& leftButtonClickedCallback,
		const ClickCallback& rightButtonClickedCallback);

protected:
	void clicked(const juce::ModifierKeys& modifiers) override;

private:
	const ClickCallback leftButtonClickedCallback, rightButtonClickedCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RightClickableTextButton)
};

#include "RightClickableTextButton.h"

RightClickableTextButton::RightClickableTextButton(
	const juce::String& text,
	const ClickCallback& leftButtonClickedCallback,
	const ClickCallback& rightButtonClickedCallback)
	: TextButton(text),
	leftButtonClickedCallback(leftButtonClickedCallback),
	rightButtonClickedCallback(rightButtonClickedCallback) {}

void RightClickableTextButton::clicked(
	const juce::ModifierKeys& modifiers) {
	if (modifiers.isLeftButtonDown() && this->leftButtonClickedCallback) {
		this->leftButtonClickedCallback();
	}
	else if (modifiers.isRightButtonDown() && this->rightButtonClickedCallback) {
		this->rightButtonClickedCallback();
	}
}

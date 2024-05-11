#pragma once

#include <JuceHeader.h>

class ControllerComponent final : public juce::Component {
public:
	ControllerComponent();

	void resized() override;
	void paint(juce::Graphics& g) override;

private:
	std::unique_ptr<juce::DrawableButton> playButton = nullptr;
	std::unique_ptr<juce::DrawableButton> stopButton = nullptr;
	std::unique_ptr<juce::DrawableButton> recordButton = nullptr;

	std::unique_ptr<juce::DrawableButton> rewindButton = nullptr;
	std::unique_ptr<juce::DrawableButton> followButton = nullptr;

	std::unique_ptr<juce::Drawable> playIcon = nullptr;
	std::unique_ptr<juce::Drawable> pauseIcon = nullptr;
	std::unique_ptr<juce::Drawable> stopIcon = nullptr;
	std::unique_ptr<juce::Drawable> recordIcon = nullptr;
	std::unique_ptr<juce::Drawable> recordOnIcon = nullptr;

	std::unique_ptr<juce::Drawable> rewindIcon = nullptr;
	std::unique_ptr<juce::Drawable> followIcon = nullptr;
	std::unique_ptr<juce::Drawable> followOnIcon = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerComponent)
};

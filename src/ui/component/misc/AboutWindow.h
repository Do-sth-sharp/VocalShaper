#pragma once

#include <JuceHeader.h>
#include "AboutComponent.h"

class AboutWindow final : public juce::DocumentWindow {
public:
	AboutWindow();
	~AboutWindow();

	void resized() override;
	void closeButtonPressed() override;

private:
	juce::Image iconTemp;
	std::unique_ptr<juce::OpenGLContext> renderer = nullptr;
	std::unique_ptr<juce::TooltipWindow> toolTip = nullptr;
	std::unique_ptr<juce::Viewport> viewport = nullptr;
	std::unique_ptr<AboutComponent> content = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AboutWindow)
};

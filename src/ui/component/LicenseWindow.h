#pragma once

#include <JuceHeader.h>

class LicenseWindow final : public juce::DocumentWindow {
public:
	LicenseWindow();
	~LicenseWindow();

	void resized() override;
	void closeButtonPressed() override;

private:
	juce::Image iconTemp;
	juce::OpenGLContext renderer;
	std::unique_ptr<juce::TooltipWindow> toolTip = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LicenseWindow)
};

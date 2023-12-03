#pragma once

#include <JuceHeader.h>

class LicenseComponent final : public juce::Component {
public:
	LicenseComponent();

	void resized() override;

private:
	std::unique_ptr<juce::ListBox> licenseList = nullptr;
	std::unique_ptr<juce::TextEditor> licenseContent = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LicenseComponent)
};

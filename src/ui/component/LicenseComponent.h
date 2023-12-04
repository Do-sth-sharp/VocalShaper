#pragma once

#include <JuceHeader.h>
#include "../dataModel/LicenseListModel.h"

class LicenseComponent final : public juce::Component {
public:
	LicenseComponent();

	void resized() override;

private:
	std::unique_ptr<juce::ListBox> licenseList = nullptr;
	std::unique_ptr<juce::TextEditor> licenseContent = nullptr;
	std::unique_ptr<LicenseListModel> licenseListModel = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LicenseComponent)
};

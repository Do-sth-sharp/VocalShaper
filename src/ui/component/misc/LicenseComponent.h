#pragma once

#include <JuceHeader.h>
#include "../../dataModel/misc/LicenseListModel.h"

class LicenseComponent final : public juce::Component {
public:
	LicenseComponent();

	void resized() override;

	void scan();
	void load(const juce::File& file);

private:
	std::unique_ptr<juce::ListBox> licenseList = nullptr;
	std::unique_ptr<juce::TextEditor> licenseContent = nullptr;
	std::unique_ptr<LicenseListModel> licenseListModel = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LicenseComponent)
};

#pragma once

#include <JuceHeader.h>

class OutputConfigComponent final : public juce::Component {
public:
	OutputConfigComponent();

	int getHeightPrefered() const;

	void resized() override;

	void currentFormatChanged(const juce::String& format);

private:
	std::unique_ptr<juce::Label> formatSelectorLabel = nullptr;
	std::unique_ptr<juce::ComboBox> formatSelector = nullptr;
	std::unique_ptr<juce::PropertyPanel> properties = nullptr;
	std::unique_ptr<juce::Label> metaLabel = nullptr;
	std::unique_ptr<juce::TableListBox> metaList = nullptr;
 
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutputConfigComponent)
};

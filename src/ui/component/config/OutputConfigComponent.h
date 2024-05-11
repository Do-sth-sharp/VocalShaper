#pragma once

#include <JuceHeader.h>
#include "../../dataModel/config/MetaDataModel.h"

class OutputConfigComponent final : public juce::Component {
public:
	OutputConfigComponent();

	int getHeightPrefered() const;

	void resized() override;

	void currentFormatChanged(const juce::String& format);
	void addMeta();
	void editMeta(const juce::String& key, const juce::String& value);
	void removeMeta(const juce::String& key);

private:
	std::unique_ptr<juce::Label> formatSelectorLabel = nullptr;
	std::unique_ptr<juce::ComboBox> formatSelector = nullptr;
	std::unique_ptr<juce::PropertyPanel> properties = nullptr;
	std::unique_ptr<juce::Label> metaLabel = nullptr;
	std::unique_ptr<juce::TableListBox> metaList = nullptr;
	std::unique_ptr<MetaDataModel> metaModel = nullptr;

	bool writeMetaConfig(const juce::String& format, const juce::StringPairArray& data);
	using MetaEditorCallback = std::function<void(const juce::String&, const juce::String&)>;
	void showMetaEditor(const juce::String& title, const juce::StringArray& possibleKeys,
		const MetaEditorCallback& callback, bool keyLocked = false,
		const juce::String& defaultKey = "", const juce::String& defaultValue = "");
 
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutputConfigComponent)
};

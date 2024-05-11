#pragma once

#include <JuceHeader.h>
#include "../../dataModel/config/ConfigListModel.h"

class ConfigComponent final : public juce::Component {
public:
	ConfigComponent();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void setPage(int index);
	int getPage() const;

	void scan();
	void showPage(int index);

private:
	std::unique_ptr<ConfigListModel> listModel = nullptr;
	std::unique_ptr<juce::ListBox> list = nullptr;
	juce::OwnedArray<juce::Component> pageList;

	bool scannedFlag = false;
	juce::String scanningMes;

	struct StartupOptions {
		juce::StringArray
			theme, language,
			layout, font;
	};
	void createStartupPage(const StartupOptions& option);
	void createFunctionPage();
	void createAudioPage();
	void createOutputPage();
	void createPluginPage();
	void createKeyMappingPage();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigComponent)
};

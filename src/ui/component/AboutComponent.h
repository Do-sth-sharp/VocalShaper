#pragma once

#include <JuceHeader.h>

class AboutComponent final : public juce::Component {
public:
	AboutComponent();

	void paint(juce::Graphics& g) override;

	int getHeightPrefered() const;

	void scanForSplash();
	using SplashConfig = std::tuple<juce::String, juce::var>;
	using SplashConfigs = juce::Array<SplashConfig>;
	void updateSplashList(const SplashConfigs& list);

private:
	juce::Image logo;
	juce::Typeface::Ptr nameType;
	juce::String productName;
	juce::String verMes, copyMes;

	SplashConfigs splashList;

	juce::Colour picBackGroundColor;
	std::unique_ptr<juce::Image> relImg;
	double imgScale = 1.0;

	juce::String staffTitle;
	juce::String staffList;
	juce::String illustTitle;
	juce::String illustList;
	juce::String thanksTitle;
	juce::String thanksList;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AboutComponent)
};

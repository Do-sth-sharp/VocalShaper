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

	void scanForTrans();
	using TransConfig = std::tuple<juce::String, juce::var>;
	using TransConfigs = juce::Array<TransConfig>;
	void updateTransList(const TransConfigs& list);

	void scanForTheme();
	using ThemeConfig = std::tuple<juce::String, juce::var>;
	using ThemeConfigs = juce::Array<ThemeConfig>;
	void updateThemeList(const ThemeConfigs& list);

private:
	juce::Image logo;
	juce::Typeface::Ptr nameType;
	juce::String productName;
	juce::String verMes, copyMes;

	juce::Colour picBackGroundColor;
	std::unique_ptr<juce::Image> relImg;
	double imgScale = 1.0;

	juce::String staffTitle;
	juce::String staffList;
	juce::String illustTitle;
	juce::String illustList;
	juce::String transTitle;
	juce::String transList;
	juce::String themeTitle;
	juce::String themeList;
	juce::String thanksTitle;
	juce::String thanksList;
	juce::String testerTitle;
	juce::String testerList;

	int staffLines = 0;
	int illustLines = 0;
	int transLines = 0;
	int themeLines = 0;
	int thanksLines = 0;
	int testerLines = 0;

	std::array<juce::Colour, 2> colorA;
	std::array<juce::Colour, 11> colorB;
	std::array<juce::Colour, 6> colorC;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AboutComponent)
};

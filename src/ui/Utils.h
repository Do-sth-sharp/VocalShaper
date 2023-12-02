#pragma once

#include <JuceHeader.h>

namespace utils {
	const juce::File getAppRootDir();

	const juce::File getLayoutDir();
	const juce::File getResourceDir();
	const juce::File getFontDir();
	const juce::File getSplashConfigDir();
	const juce::File getThemeDir();
	const juce::File getTransDir();
	const juce::File getDataDir();

	const juce::File getConfigDir();
	const juce::File getThemeRootDir(const juce::String& name);
	const juce::File getTransRootDir(const juce::String& name);

	const juce::File getConfigFile(const juce::String& name,
		const juce::String& type = ".json");
	const juce::File getThemeColorFile(const juce::String& name,
		const juce::String& file = "colors.json");
	const juce::File getFontFile(const juce::String& name,
		const juce::String& type = ".ttf");
	const juce::File getResourceFile(const juce::String& name);
	const juce::File getSplashConfigFile(const juce::String& name,
		const juce::String& type = ".json");

	const juce::URL getHelpPage(const juce::String& version, const juce::String& branch);
	const juce::URL getUpdatePage(const juce::String& version, const juce::String& branch);
	const juce::URL getBilibiliPage();
	const juce::URL getGithubPage();
	const juce::URL getWebsitePage();
}

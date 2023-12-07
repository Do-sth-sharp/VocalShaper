#pragma once

#include <JuceHeader.h>

namespace utils {
	const juce::File getAppExecFile();
	const juce::File getAppRootDir();
	const juce::File getRegExecFile();

	const juce::File getLayoutDir();
	const juce::File getResourceDir();
	const juce::File getFontDir();
	const juce::File getSplashConfigDir();
	const juce::File getThemeDir();
	const juce::File getTransDir();
	const juce::File getDataDir();
	const juce::File getLicenseDir();

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
	const juce::File getTransConfigFile(const juce::String& name,
		const juce::String& file = "config.json");
	const juce::File getThemeConfigFile(const juce::String& name,
		const juce::String& file = "config.json");

	const juce::URL getHelpPage(const juce::String& version, const juce::String& branch);
	const juce::URL getUpdatePage(const juce::String& version, const juce::String& branch);
	const juce::URL getBilibiliPage();
	const juce::URL getGithubPage();
	const juce::URL getWebsitePage();

	const juce::Rectangle<int> getScreenSize(const juce::Component* comp);

	const juce::StringArray parseCommand(const juce::String& command);

	bool regProjectFileInSystem();
	bool unregProjectFileFromSystem();
}

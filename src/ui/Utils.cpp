#include "Utils.h"

namespace utils {
	const juce::File getAppRootDir() {
		return juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentApplicationFile)
			.getParentDirectory();
	}

	const juce::File getLayoutDir() {
		return getAppRootDir().getChildFile("./layouts/");
	}

	const juce::File getResourceDir() {
		return getAppRootDir().getChildFile("./rc/");
	}

	const juce::File getFontDir() {
		return getAppRootDir().getChildFile("./fonts/");
	}

	const juce::File getSplashConfigDir() {
		return getAppRootDir().getChildFile("./splash/");
	}

	const juce::File getThemeDir() {
		return getAppRootDir().getChildFile("./themes/");
	}

	const juce::File getTransDir() {
		return getAppRootDir().getChildFile("./translates/");
	}

	const juce::File getDataDir() {
		return getAppRootDir().getChildFile("./data/");
	}

	const juce::File getConfigDir() {
		return getDataDir().getChildFile("./config/");
	}

	const juce::File getThemeRootDir(const juce::String& name) {
		return getThemeDir().getChildFile("./" + name + "/");
	}

	const juce::File getTransRootDir(const juce::String& name) {
		return getTransDir().getChildFile("./" + name + "/");
	}

	const juce::File getConfigFile(const juce::String& name,
		const juce::String& type) {
		return getConfigDir().getChildFile(name + type);
	}

	const juce::File getThemeColorFile(const juce::String& name,
		const juce::String& file) {
		return getThemeRootDir(name).getChildFile(file);
	}

	const juce::File getFontFile(const juce::String& name,
		const juce::String& type) {
		return getFontDir().getChildFile(name + type);
	}

	const juce::File getResourceFile(const juce::String& name) {
		return getResourceDir().getChildFile(name);
	}

	const juce::File getSplashConfigFile(const juce::String& name,
		const juce::String& type) {
		return getSplashConfigDir().getChildFile(name + type);
	}

	const juce::URL getHelpPage(const juce::String& version,
		const juce::String& branch) {
		return juce::URL{ "https://help.daw.org.cn" }
			.withParameter("version", version)
			.withParameter("branch", branch);
	}

	const juce::URL getUpdatePage(const juce::String& version,
		const juce::String& branch) {
		return juce::URL{ "https://update.daw.org.cn" }
			.withParameter("version", version)
			.withParameter("branch", branch);
	}

	const juce::URL getBilibiliPage() {
		return juce::URL{ "https://space.bilibili.com/2060981097" };
	}

	const juce::URL getGithubPage() {
		return juce::URL{ "https://github.com/Do-sth-sharp/VocalShaper" };
	}

	const juce::URL getWebsitePage() {
		return juce::URL{ "https://daw.org.cn" };
	}
}

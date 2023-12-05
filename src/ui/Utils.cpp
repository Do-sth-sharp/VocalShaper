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

	const juce::File getLicenseDir() {
		return getAppRootDir().getChildFile("./licenses/");
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

	const juce::Rectangle<int> getScreenSize(const juce::Component* comp) {
		/** Get current screen */
		if (!comp) { return { 0, 0 }; }
		auto ptrScreen = juce::Desktop::getInstance().getDisplays()
			.getDisplayForRect(comp->getScreenBounds());
		if (!ptrScreen) { return { 0, 0 }; }

		/** Base size */
		juce::Rectangle<int> rect = ptrScreen->userArea;
		juce::Rectangle<int> rectTotal = ptrScreen->totalArea;
		int width = rect.getWidth();
		int height = rect.getHeight();
		int widthT = rectTotal.getWidth();
		int heightT = rectTotal.getHeight();

		/** Width is the longer side */
		if (width < height) {
			int temp = width;
			width = height;
			height = temp;
		}
		if (widthT < heightT) {
			int temp = widthT;
			widthT = heightT;
			heightT = temp;
		}

		/** Calculate aspect ratio */
		double pNow = 9.0 / 16.0, pTotal = 9.0 / 16.0;
		pNow = (double)height / (double)width;
		pTotal = (double)heightT / (double)widthT;

		/** Scale to the standard */
		constexpr double proportion = 816.0 / 1536.0;
		constexpr double prop4Scale = 1080.0 / 1920.0;
		const double scaleM = 1.25 / 1920;
		const double scaleN = 1.0 / ((pTotal >= prop4Scale) ? widthT : (heightT / prop4Scale));
		const double scaleMN = scaleN / scaleM;

		/** Limit aspect ratio */
		if (pNow > proportion) {
			height = width * proportion;
		}
		else if (pNow < proportion) {
			width = height / proportion;
		}

		/** Result */
		return juce::Rectangle<int>{
			(int)(width * scaleMN), (int)(height * scaleMN) };
	}
}

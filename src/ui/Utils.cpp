#include "Utils.h"

#if JUCE_WINDOWS
#include <Windows.h>
#endif //JUCE_WINDOWS

/** To Fix Symbol Export Error Of juce::ScopedMessageBox */
#include <juce_gui_basics/detail/juce_ScopedMessageBoxInterface.h>
#include <juce_gui_basics/detail/juce_ScopedMessageBoxImpl.h>
#include <juce_gui_basics/windows/juce_ScopedMessageBox.cpp>

/** To Fix Symbol Export Error Of XmlElement::TextFormat::TextFormat() */
namespace juce {
	XmlElement::TextFormat::TextFormat() {}
}

namespace utils {
	const juce::File getAppExecFile() {
		return juce::File::getSpecialLocation(juce::File::SpecialLocationType::hostApplicationPath);
	}
	const juce::File getAppRootDir() {
		return getAppExecFile().getParentDirectory();
	}
	const juce::File getRegExecFile() {
#if JUCE_WINDOWS
		return getAppRootDir().getChildFile("./FileRegistrar.exe");

#elif JUCE_LINUX
		return getAppRootDir().getChildFile("./FileRegistrar");

#elif JUCE_MAC
		return getAppRootDir().getChildFile("./FileRegistrar");

#else
		return getAppRootDir().getChildFile("./FileRegistrar");

#endif
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

	const juce::File getAudioDir() {
		return getDataDir().getChildFile("./audio/");
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

	const juce::File getKeyMappingFile(
		const juce::String& file) {
		return getConfigDir().getChildFile(file);
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

	const juce::File getTransConfigFile(const juce::String& name,
		const juce::String& file) {
		return getTransRootDir(name).getChildFile(file);
	}

	const juce::File getThemeConfigFile(const juce::String& name,
		const juce::String& file) {
		return getThemeRootDir(name).getChildFile(file);
	}

	const juce::File getLayoutFile(const juce::String& name,
		const juce::String& type) {
		return getLayoutDir().getChildFile(name + type);
	}

	const juce::File getPluginBlackListFile(
		const juce::String& file) {
		return getAudioDir().getChildFile(file);
	}

	const juce::File getPluginListFile(
		const juce::String& file) {
		return getAudioDir().getChildFile(file);
	}

	const juce::File getPluginSearchPathFile(
		const juce::String& file) {
		return getAudioDir().getChildFile(file);
	}

	const juce::File getPluginDeadTempDir(
		const juce::String& path) {
		return getAudioDir().getChildFile(path);
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

	const juce::StringArray parseCommand(const juce::String& command) {
		return juce::StringArray::fromTokens(command, true);
	}

	bool regProjectFileInSystem() {
#if JUCE_WINDOWS
		juce::String regExecFilePath = getRegExecFile().getFullPathName();
		juce::String appExecFilePath = getAppExecFile().getFullPathName();

		std::string fileStr = regExecFilePath.toStdString();
		std::string paramStr = juce::String{ "\"" + appExecFilePath + "\"" + " true" }.toStdString();

		SHELLEXECUTEINFO sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.lpVerb = "runas";
		sei.lpFile = fileStr.c_str();
		sei.lpParameters = paramStr.c_str();
		sei.nShow = SW_HIDE;

		DWORD exitCode = -1;
		if (ShellExecuteEx(&sei)) {
			if (sei.hProcess != NULL) {
				WaitForSingleObject(sei.hProcess, INFINITE);
				GetExitCodeProcess(sei.hProcess, &exitCode);
				CloseHandle(sei.hProcess);
			}
		}

		return exitCode == 0;

#else //JUCE_WINDOWS
		return false;

#endif //JUCE_WINDOWS
	}

	bool unregProjectFileFromSystem() {
#if JUCE_WINDOWS
		juce::String regExecFilePath = getRegExecFile().getFullPathName();
		juce::String appExecFilePath = getAppExecFile().getFullPathName();

		std::string fileStr = regExecFilePath.toStdString();
		std::string paramStr = juce::String{ "\"" + appExecFilePath + "\"" + " false" }.toStdString();

		SHELLEXECUTEINFO sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.lpVerb = "runas";
		sei.lpFile = fileStr.c_str();
		sei.lpParameters = paramStr.c_str();
		sei.nShow = SW_HIDE;

		DWORD exitCode = -1;
		if (ShellExecuteEx(&sei)) {
			if (sei.hProcess != NULL) {
				WaitForSingleObject(sei.hProcess, INFINITE);
				GetExitCodeProcess(sei.hProcess, &exitCode);
				CloseHandle(sei.hProcess);
			}
		}

		return exitCode == 0;

#else //JUCE_WINDOWS
		return false;

#endif //JUCE_WINDOWS
	}

	static const std::vector<int> preKMP(const juce::String& strLong) {
		std::vector<int> next;
		next.resize(strLong.length());

		next[0] = -1;/**< While the first char not match, i++, j++ */
		int j = 0;
		int k = -1;
		while (j < strLong.length() - 1) {
			if (k == -1 || strLong[j] == strLong[k]) {
				j++;
				k++;
				next[j] = k;
			}
			else {
				k = next[k];
			}
		}

		return next;
	}

	static int KMP(const juce::String& strShort, const juce::String& strLong, const std::vector<int>& next) {
		int i = 0;
		int j = 0;
		while (i < strLong.length() && j < strShort.length()) {
			if (j == -1 || strLong[i] == strShort[j]) {
				i++;
				j++;
			}
			else {
				j = next[j];
			}
		}
		if (j == strShort.length()) {
			return i - j;
		}
		return -1;
	}

	const juce::StringArray searchKMP(const juce::StringArray& list, const juce::String& word) {
		juce::StringArray result;
		for (auto& s : list) {
			if (KMP(word, s, preKMP(s)) >= 0) {
				result.add(s);
			}
		}
		return result;
	}

	bool saveXml(const juce::File& file, juce::XmlElement* xml) {
		if (!xml) { return false; }

		juce::FileOutputStream ostream(file);
		if (!ostream.openedOk()) { return false; }
		ostream.setPosition(0);
		ostream.truncate();

		xml->writeTo(ostream, juce::XmlElement::TextFormat{});
		return true;
	}

	std::unique_ptr<juce::XmlElement> readXml(const juce::File& file) {
		juce::XmlDocument doc(file);
		return doc.getDocumentElement(false);
	}

	void panic() {
		/** Force SIGSEGV */
		*((char*)-1) = 'x';
	}
}

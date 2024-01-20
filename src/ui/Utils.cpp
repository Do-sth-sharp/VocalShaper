#include "Utils.h"

#if JUCE_WINDOWS
#include <Windows.h>
#endif //JUCE_WINDOWS

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

	const juce::File getIconDir() {
		return getAppRootDir().getChildFile("./RemixIcon/");
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

	const juce::File getIconClassDir(const juce::String& name) {
		return getIconDir().getChildFile("./" + name + "/");
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

	const juce::File getIconFile(
		const juce::String& className, const juce::String& name,
		const juce::String& type) {
		return getIconClassDir(className).getChildFile(name + type);
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

	int matchKMP(const juce::String& line, const juce::String& word) {
		return KMP(word, line, preKMP(line));
	}

	const juce::StringArray searchKMP(const juce::StringArray& list, const juce::String& word) {
		juce::StringArray result;
		for (auto& s : list) {
			if (matchKMP(s, word) >= 0) {
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

	float logRMS(float rms) {
#if JUCE_MSVC
		return 20.f * std::log10f(rms);
#else //JUCE_MSVC
		return 20.f * std::log10(rms);
#endif //JUCE_MSVC
	}

	float getLogLevelPercent(float logLevel, float total) {
		return (logLevel - (-total)) / total;
	}

	float getLogFromPercent(float percent, float total) {
		return (-total) + percent * total;
	}

	TextIntegerFilter::TextIntegerFilter(int minLimit, int maxLimit)
		: minLimit(minLimit), maxLimit(maxLimit) {}

	juce::String TextIntegerFilter::filterNewText(
		juce::TextEditor& editor, const juce::String& newInput) {
		if (!newInput.containsOnly("-0123456789")) { return ""; }

		int num = (editor.getText() + newInput).getIntValue();
		if (num < this->minLimit) { return ""; }
		if (num > this->maxLimit) { return ""; }
		return newInput;
	}

	TextDoubleFilter::TextDoubleFilter(
		double minLimit, double maxLimit, int numberOfDecimalPlaces)
		: minLimit(minLimit), maxLimit(maxLimit),
		numberOfDecimalPlaces(numberOfDecimalPlaces) {}

	juce::String TextDoubleFilter::filterNewText(
		juce::TextEditor& editor, const juce::String& newInput) {
		if (!newInput.containsOnly("-0123456789.")) { return ""; }

		auto currentText = editor.getText();
		int dotPlace = currentText.indexOf(0, ".");
		if ((dotPlace > -1) && newInput.containsChar('.')) { return ""; }
		if ((dotPlace > -1) && (this->numberOfDecimalPlaces > -1)
			&& ((currentText.length() - dotPlace - 1 + newInput.length()) >
			this->numberOfDecimalPlaces)) { return ""; }

		double num = (currentText + newInput).getDoubleValue();
		if (num < this->minLimit) { return ""; }
		if (num > this->maxLimit) { return ""; }
		return newInput;
	}

	const juce::Array<PluginGroup> groupPlugin(const juce::Array<juce::PluginDescription>& list,
		PluginGroupType groupType, bool search, const juce::String& searchText) {
		/** Plugin Groups */
		std::map<juce::String, juce::Array<juce::PluginDescription>> groupMap;

		/** Group Plugin */
		for (auto& i : list) {
			/** Group Index */
			juce::StringArray indexs;
			switch (groupType) {
			case PluginGroupType::Format:
				indexs.add(i.pluginFormatName);
				break;
			case PluginGroupType::Manufacturer:
				indexs.add(i.manufacturerName);
				break;
			case PluginGroupType::Category: {
				indexs = juce::StringArray::fromTokens(i.category, "|", "\"\'");
				break;
			}
			}

			/** Searching By Name */
			if (search) {
				if (utils::matchKMP(i.name, searchText) < 0
					&& utils::searchKMP(indexs, searchText).isEmpty()) {
					continue;
				}
			}

			/** Insert To Group List */
			for (auto& index : indexs) {
				auto& groupList = groupMap[index];
				groupList.add(i);
			}
		}

		/** Plugin Group List */
		juce::Array<PluginGroup> groupList;
		for (auto& i : groupMap) {
			groupList.add({ i.first, i.second });
		}

		return groupList;
	}

	juce::PopupMenu createPluginMenu(const juce::Array<PluginGroup>& list,
		const std::function<void(const juce::PluginDescription&)>& callback) {
		juce::PopupMenu menu;

		for (auto& [name, plugins] : list) {
			menu.addSectionHeader(name);
			for (auto& i : plugins) {
				menu.addItem(i.name, std::bind(callback, i));
			}
		}

		return menu;
	}

	const TimeInSeconds splitTime(double seconds) {
		int msec = (uint64_t)(seconds * (uint64_t)1000) % 1000;
		int sec = (uint64_t)std::floor(seconds) % 60;
		int minute = ((uint64_t)std::floor(seconds) / 60) % 60;
		int hour = seconds / 3600;

		return { hour, minute, sec, msec };
	}

	const TimeInBeats splitBeat(uint64_t measures, double beats) {
		int mbeat = (uint64_t)(beats * (uint64_t)100) % 100;
		int beat = (uint64_t)std::floor(beats) % 100;
		int measure = (uint64_t)std::floor(measures) % 10000;

		return { measure, beat, mbeat };
	}

	const std::array<uint8_t, 8> createTimeStringBase(const TimeInSeconds& time) {
		std::array<uint8_t, 8> num = { 0 };
		auto& [hour, minute, sec, msec] = time;

		num[0] = (hour / 1) % 10;
		num[1] = (minute / 10) % 10;
		num[2] = (minute / 1) % 10;
		num[3] = (sec / 10) % 10;
		num[4] = (sec / 1) % 10;
		num[5] = (msec / 100) % 10;
		num[6] = (msec / 10) % 10;
		num[7] = (msec / 1) % 10;

		return num;
	}

	const std::array<uint8_t, 8> createBeatStringBase(const TimeInBeats& time) {
		std::array<uint8_t, 8> num = { 0 };
		auto& [measure, beat, mbeat] = time;

		num[0] = (measure / 1000) % 10;
		num[1] = (measure / 100) % 10;
		num[2] = (measure / 10) % 10;
		num[3] = (measure / 1) % 10;
		num[4] = (beat / 10) % 10;
		num[5] = (beat / 1) % 10;
		num[6] = (mbeat / 10) % 10;
		num[7] = (mbeat / 1) % 10;

		return num;
	}

	const juce::String createTimeString(const TimeInSeconds& time) {
		/** Get Num */
		auto num = utils::createTimeStringBase(time);

		/** Result */
		return juce::String{ num[0] } + ":"
			+ juce::String{ num[1] } + juce::String{ num[2] } + ":"
			+ juce::String{ num[3] } + juce::String{ num[4] } + "."
			+ juce::String{ num[5] } + juce::String{ num[6] } + juce::String{ num[7] };
	}

	const juce::String createBeatString(const TimeInBeats& time) {
		/** Get Num */
		auto num = utils::createBeatStringBase(time);

		/** Result */
		return juce::String{ num[0] } + juce::String{ num[1] } + juce::String{ num[2] } + juce::String{ num[3] } + ":"
			+ juce::String{num[4]} + juce::String{ num[5] } + ":"
			+ juce::String{num[6]} + juce::String{num[7]};
	}
}

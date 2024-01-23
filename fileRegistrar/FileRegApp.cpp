#include <JuceHeader.h>
#include "SystemUtils.h"

#define OUT(x) \
	DBG(x); \
	std::cout << (x) << std::endl

class FileRegApp final : public juce::JUCEApplication {
public:
	const juce::String getApplicationName() override { return "VocalShaper.FileRegistrar"; };
	const juce::String getApplicationVersion() override {
		return juce::String{ PROJECT_VERSION_MAJOR } + "." + juce::String{ PROJECT_VERSION_MINOR } + "." + juce::String{ PROJECT_VERSION_PATCH };
	};
	bool moreThanOneInstanceAllowed() override { return false; };

	void initialise(const juce::String& commandLine) override {
		OUT("VocalShaper File Registrar v" + this->getApplicationVersion());
		OUT("Copyright 2023-2024 VocalSharp Org. All rights reserved.");
		OUT("");

		/** Check Admin */
		if (!checkAdmin()) {
			OUT("\033[31m[ERROR]\033[0m Bad Privilege!");
			this->setApplicationReturnValue(1);
			juce::JUCEApplication::quit();
			return;
		}

		/** Parse Command */
		juce::StringArray commandArray = juce::StringArray::fromTokens(commandLine, " ", "\"");
		for (auto& s : commandArray) {
			/** Remove Quote */
			s = s.removeCharacters("\"");
		}
		/** Check First Arg And Remove Execute Path */
		{
			juce::File firstArgFile(commandArray[0]);
			juce::File execFile = juce::File::getSpecialLocation(juce::File::hostApplicationPath);
			if (firstArgFile == execFile) {
				commandArray.remove(0);
			}
		}

		/** Get Command */
		juce::String appPath;
		bool isReg = true;
		if (commandArray.size() < 1 || commandArray.size() > 2) {
			OUT("\033[31m[ERROR]\033[0m Bad Command!");
			this->setApplicationReturnValue(2);
			juce::JUCEApplication::quit();
			return;
		}
		else {
			appPath = commandArray[0];
			if (commandArray.size() == 2) {
				isReg = !(commandArray[1] == "0" || commandArray[1] == "false" || commandArray[1] == "FALSE");
			}

			OUT("\033[36mApplication Path:\033[0m " + appPath.toStdString());
			OUT("\033[36mRegister:\033[0m " + std::string{ isReg ? "Yes" : "No" });
		}
		OUT("");

		/** Reg */
		if (isReg) {
			if (!regProjectFileInSystem(appPath)) {
				OUT("\033[33m[ERROR]\033[0m Can't Register!");
				this->setApplicationReturnValue(3);
				juce::JUCEApplication::quit();
				return;
			}
			OUT("\033[32m[OK]\033[0m Registered!");
			OUT("");
		}
		else {
			if (!unregProjectFileFromSystem()) {
				OUT("\033[33m[ERROR]\033[0m Can't Unregister!");
				this->setApplicationReturnValue(3);
				juce::JUCEApplication::quit();
				return;
			}
			OUT("\033[32m[OK]\033[0m Unregistered!");
			OUT("");
		}

		/** Return */
		juce::JUCEApplication::quit();
	};

	void shutdown() override {};
};

START_JUCE_APPLICATION(FileRegApp)

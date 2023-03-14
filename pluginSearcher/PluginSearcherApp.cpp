#include <JuceHeader.h>

#include "PluginSearcher.h"

#define OUT(x) \
    DBG(x); \
    std::cout << (x) << std::endl

class PluginSearchApp final : public juce::JUCEApplication {
    std::unique_ptr<PluginSearcher> pluginSearcher = nullptr;

public:
	const juce::String getApplicationName() override { return "VocalShaper.PluginSearcher"; };
	const juce::String getApplicationVersion() override { return "0.0.1"; };
	bool moreThanOneInstanceAllowed() override { return false; };

    void initialise(const juce::String& commandLine) override {
        OUT("VocalShaper Plugin Searcher v0.0.1");
        OUT("Copyright 2023 VocalSharp Org. All rights reserved.");
        OUT("");

        juce::StringArray commandArray = juce::StringArray::fromTokens(commandLine, " ", "\"");
        for (auto& s : commandArray) {
            /** Remove Quote */
            s = s.removeCharacters("\"");
        }
        if (commandArray.size() == 5) {
            /** Check First Arg And Remove Execute Path */
            juce::File firstArgFile(commandArray[0]);
            juce::File execFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
            if (firstArgFile == execFile) {
                commandArray.remove(0);
            }
        }
        if (commandArray.size() != 4) {
            OUT("\033[31m[ERROR]\033[0m Bad Command!");
            juce::JUCEApplication::quit();
        }
        else {
            OUT("\033[36mPlugin Path List:\033[0m " + commandArray[0].toStdString());
            OUT("\033[36mPlugin List Temporary:\033[0m " + commandArray[1].toStdString());
            OUT("\033[36mPlugin Black List:\033[0m " + commandArray[2].toStdString());
            OUT("\033[36mDead Plugin Temporary:\033[0m " + commandArray[3].toStdString());
        }
        OUT("");

        this->pluginSearcher = std::make_unique<PluginSearcher>(
            commandArray[0], commandArray[1], commandArray[2], commandArray[3]);
        juce::MessageManager::callAsync(
            [pluginSearcher = this->pluginSearcher.get()] {pluginSearcher->start(); });
    };

    void shutdown() override {
        this->pluginSearcher = nullptr;
    };
};

START_JUCE_APPLICATION(PluginSearchApp)


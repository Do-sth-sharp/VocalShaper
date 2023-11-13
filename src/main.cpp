#include <JuceHeader.h>
#include "audioCore/AC_API.h"

class MainApplication : public juce::JUCEApplication {
public:
	const juce::String getApplicationName() override {
		return utils::getAudioPlatformName(); };
	const juce::String getApplicationVersion() override {
		return utils::getAudioPlatformVersionString(); };
	bool moreThanOneInstanceAllowed() override { return false; };

	void initialise(const juce::String& commandLineParameters) override {
		//TODO
	};
	void shutdown() override {
		//TODO
	};
};

START_JUCE_APPLICATION(MainApplication)

#pragma once

#include <JuceHeader.h>

class LookAndFeelFactory final : private juce::DeletedAtShutdown {
public:
	LookAndFeelFactory() = default;

	void initialise();
	void setDefaultSansSerifTypeface(juce::Typeface::Ptr typeface);

	juce::LookAndFeel_V4* forToolBar() const;
	juce::LookAndFeel_V4* forMainMenu() const;
	juce::LookAndFeel_V4* forSysStatus() const;
	juce::LookAndFeel_V4* forTime() const;

private:
	std::unique_ptr<juce::LookAndFeel> mainLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> toolBarLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> mainMenuLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> sysStatusLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> timeLAF = nullptr;

public:
	static LookAndFeelFactory* getInstance();
	static void releaseInstance();

private:
	static LookAndFeelFactory* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeelFactory)
};

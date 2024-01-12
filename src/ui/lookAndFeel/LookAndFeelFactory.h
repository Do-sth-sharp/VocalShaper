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
	juce::LookAndFeel_V4* forController() const;
	juce::LookAndFeel_V4* forTools() const;
	juce::LookAndFeel_V4* forMessage() const;
	juce::LookAndFeel_V4* forMessageView() const;

private:
	std::unique_ptr<juce::LookAndFeel> mainLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> toolBarLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> mainMenuLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> sysStatusLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> timeLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> controllerLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> toolsLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> messageLAF = nullptr;
	std::unique_ptr<juce::LookAndFeel_V4> messageViewLAF = nullptr;

public:
	static LookAndFeelFactory* getInstance();
	static void releaseInstance();

private:
	static LookAndFeelFactory* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeelFactory)
};

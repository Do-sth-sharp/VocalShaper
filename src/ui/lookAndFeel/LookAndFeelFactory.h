﻿#pragma once

#include <JuceHeader.h>

class LookAndFeelFactory final : private juce::DeletedAtShutdown {
public:
	LookAndFeelFactory() = default;

	void initialise();
	void setDefaultSansSerifTypeface(juce::Typeface::Ptr typeface);

	juce::LookAndFeel_V4* forToolBar() const;

private:
	std::unique_ptr<juce::LookAndFeel_V4> toolBarLAF = nullptr;

public:
	static LookAndFeelFactory* getInstance();
	static void releaseInstance();

private:
	static LookAndFeelFactory* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeelFactory)
};
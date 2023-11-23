#pragma once

#include <JuceHeader.h>

class MainMenu final : private juce::DeletedAtShutdown {
public:
	MainMenu() = default;

	juce::PopupMenu create() const;

private:
	juce::PopupMenu createFile() const;
	juce::PopupMenu createEdit() const;
	juce::PopupMenu createView() const;
	juce::PopupMenu createProject() const;
	juce::PopupMenu createControl() const;
	juce::PopupMenu createConfig() const;
	juce::PopupMenu createMisc() const;

public:
	static MainMenu* getInstance();
	static void releaseInstance();

private:
	static MainMenu* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainMenu)
};

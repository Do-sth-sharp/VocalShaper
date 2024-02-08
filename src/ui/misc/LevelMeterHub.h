#pragma once

#include <JuceHeader.h>

class LevelMeterHub final
	: public juce::Timer,
	private juce::DeletedAtShutdown {
public:
	LevelMeterHub();

	void timerCallback() override;

	class Target {
	public:
		Target() {
			LevelMeterHub::getInstance()->add(this);
		};
		virtual ~Target() {
			LevelMeterHub::getInstance()->remove(this);
		};

	public:
		virtual void updateLevelMeter() = 0;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Target)
	};

	void add(Target* target);
	void remove(Target* target);

private:
	juce::Array<Target*> list;

public:
	static LevelMeterHub* getInstance();
	static void releaseInstance();

private:
	static LevelMeterHub* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterHub)
};

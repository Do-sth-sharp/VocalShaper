#pragma once

#include <JuceHeader.h>

class ActionBase {
public:
	ActionBase() = default;
	virtual ~ActionBase() = default;

	virtual bool doAction() = 0;

protected:
	void output(const juce::String& mes);

private:
	JUCE_LEAK_DETECTOR(ActionBase)
};

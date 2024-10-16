#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class EditorLookAndFeel : public MainLookAndFeel {
public:
	EditorLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditorLookAndFeel)
};
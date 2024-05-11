#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class ColorEditorLookAndFeel : public MainLookAndFeel {
public:
	ColorEditorLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorEditorLookAndFeel)
};
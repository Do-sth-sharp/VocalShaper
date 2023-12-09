#pragma once

#include <JuceHeader.h>

class SysStatusComponent final : public juce::AnimatedAppComponent {
public:
	SysStatusComponent();

	void update() override;
	void paint(juce::Graphics& g) override;
	void resized() override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SysStatusComponent)
};

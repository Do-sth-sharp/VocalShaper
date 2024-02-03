#pragma once

#include <JuceHeader.h>

class SideChainComponent final : public juce::Component {
public:
	SideChainComponent();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update(int index);

private:
	std::unique_ptr<juce::Drawable> addIcon = nullptr;
	std::unique_ptr<juce::Drawable> subIcon = nullptr;
	std::unique_ptr<juce::DrawableButton> addButton = nullptr;
	std::unique_ptr<juce::DrawableButton> subButton = nullptr;

	int index = -1;
	int sideChainNum = 0;

	void add();
	void sub();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SideChainComponent)
};

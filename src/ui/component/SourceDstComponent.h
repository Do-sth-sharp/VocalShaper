#pragma once

#include <JuceHeader.h>

class SourceDstComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	SourceDstComponent();

	void update(int index, int dst);

	void paint(juce::Graphics& g) override;
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;

private:
	int index = -1, dst = -1;

	juce::var getDragSourceDescription() const;
	juce::PopupMenu createDstSourceMenu(const std::function<void(int)>& callback) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceDstComponent)
};

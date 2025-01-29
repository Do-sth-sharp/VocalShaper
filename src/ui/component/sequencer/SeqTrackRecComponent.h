#pragma once

#include <JuceHeader.h>

class SeqTrackRecComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	SeqTrackRecComponent();

	void paint(juce::Graphics& g) override;

	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

	void updateIndex(int index);
	void update();

private:
	int index = -1;
	int rec = 0;

	void changeRecQuick();
	void changeRec();

	juce::PopupMenu createMenu() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackRecComponent)
};

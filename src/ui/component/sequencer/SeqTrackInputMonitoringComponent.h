#pragma once

#include <JuceHeader.h>

class SeqTrackInputMonitoringComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	SeqTrackInputMonitoringComponent();

	void paint(juce::Graphics& g) override;

	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

	void updateIndex(int index);
	void update();

private:
	int index = -1;
	bool inputMonitoring = false;

	using AudioLink = std::pair<int, int>;

	bool midiInput = false;
	std::set<AudioLink> audioInput;

	void changeInputMonitoring();
	void showMenu();

	void changeMIDIInput();
	void changeAudioInput();

	juce::PopupMenu createMenu();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackInputMonitoringComponent)
};

#pragma once

#include <JuceHeader.h>

class InstrIOComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	InstrIOComponent() = delete;
	InstrIOComponent(bool isInput);

	void paint(juce::Graphics& g) override;

	void update(int index);

	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

	void outputTo(int trackIndex, bool link);

private:
	const bool isInput;
	int index = -1;
	bool linked = false;
	juce::String name;

	using MIDILink = std::tuple<int, int>;
	using AudioLink = std::tuple<int, int, int, int>;
	bool midiInputFromDevice = false;
	juce::Array<MIDILink> midiInputFromSource;
	juce::Array<AudioLink> audioOutputToMixer;
	std::set<int> inputSourceTemp;
	std::set<int> outputMixerTemp;

	void showLinkMenu(bool link);

	juce::var getDragSourceDescription() const;
	juce::PopupMenu createLinkMenu();
	juce::PopupMenu createUnlinkMenu();
	const juce::Array<std::tuple<int, int>> getOutputChannelLinks(int track) const;
	juce::String createToolTipString() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrIOComponent)
};

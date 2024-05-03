#pragma once

#include <JuceHeader.h>

class SeqTrackIOComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	SeqTrackIOComponent() = delete;
	SeqTrackIOComponent(bool isMidi);

	void paint(juce::Graphics& g) override;

	void update(int index);

	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

private:
	const bool isMidi;
	int index = -1;
	bool linked = false;
	juce::String name;

	using MIDILink = std::tuple<int, int>;
	using AudioLink = std::tuple<int, int, int, int>;

	juce::Array<MIDILink> midiOutputToMixer;
	juce::Array<AudioLink> audioOutputToMixer;
	std::set<int> midiOutputMixerTemp;
	std::set<int> audioOutputMixerTemp;

	void showLinkMenu(bool link);

	const juce::Array<std::tuple<int, int>> getOutputToMixerChannelLinks(int trackIndex) const;

	juce::var getDragSourceDescription() const;
	juce::String createToolTipString() const;
	juce::PopupMenu createLinkMenu() const;
	juce::PopupMenu createUnlinkMenu() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackIOComponent)
};

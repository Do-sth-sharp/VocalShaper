#pragma once

#include <JuceHeader.h>

class MixerTrackIOComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	MixerTrackIOComponent() = delete;
	MixerTrackIOComponent(bool isInput, bool isMidi);

	void paint(juce::Graphics& g) override;

	void update(int index);

	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

private:
	const bool isInput, isMidi;
	int index = -1;
	bool linked = false;
	juce::String name;

	using MIDILink = std::tuple<int, int>;
	using AudioLink = std::tuple<int, int, int, int>;

	bool midiInputFromDevice = false;
	juce::Array<MIDILink> midiInputFromSource;
	juce::Array<AudioLink> audioInputFromDevice;
	juce::Array<AudioLink> audioInputFromSource;
	juce::Array<AudioLink> audioInputFromInstr;
	juce::Array<AudioLink> audioInputFromSend;
	std::set<int> midiInputSourceTemp;
	std::set<int> audioInputSourceTemp;
	std::set<int> audioInputInstrTemp;
	std::set<int> audioInputSendTemp;

	bool midiOutputToDevice = false;
	juce::Array<AudioLink> audioOutputToDevice;
	juce::Array<AudioLink> audioOutputToSend;
	std::set<int> audioOutputSendTemp;

	void showLinkMenu(bool link);

	const juce::Array<std::tuple<int, int>> getInputFromDeviceChannelLinks() const;
	const juce::Array<std::tuple<int, int>> getInputFromSourceChannelLinks(int seqIndex) const;
	const juce::Array<std::tuple<int, int>> getInputFromInstrChannelLinks(int instrIndex) const;
	const juce::Array<std::tuple<int, int>> getInputFromSendChannelLinks(int trackIndex) const;
	const juce::Array<std::tuple<int, int>> getOutputToDeviceChannelLinks() const;
	const juce::Array<std::tuple<int, int>> getOutputToSendChannelLinks(int trackIndex) const;

	juce::var getDragSourceDescription() const;
	juce::String createToolTipString() const;
	juce::PopupMenu createLinkMenu() const;
	juce::PopupMenu createUnlinkMenu() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerTrackIOComponent)
};

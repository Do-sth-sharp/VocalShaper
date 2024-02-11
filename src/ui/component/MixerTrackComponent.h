#pragma once

#include <JuceHeader.h>
#include "SideChainComponent.h"
#include "MixerTrackIOComponent.h"
#include "KnobBase.h"
#include "FaderBase.h"
#include "MixerTrackLevelMeter.h"
#include "MixerTrackMuteComponent.h"

class MixerTrackComponent final
	: public juce::Component,
	public juce::DragAndDropTarget {
public:
	MixerTrackComponent();

	void resized() override;
	void paint(juce::Graphics& g) override;
	void paintOverChildren(juce::Graphics& g) override;

	void update(int index);
	void updateGain();
	void updatePan();
	void updateFader();
	void updateMute();

	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

	bool isInterestedInDragSource(
		const SourceDetails& dragSourceDetails) override;
	void itemDragEnter(const SourceDetails& dragSourceDetails) override;
	void itemDragExit(const SourceDetails& dragSourceDetails) override;
	void itemDropped(const SourceDetails& dragSourceDetails) override;

private:
	int index = -1;
	juce::Colour trackColor, nameColor;
	juce::String name;
	bool dragHovered = false;
	bool panValid = true;

	std::unique_ptr<SideChainComponent> sideChain = nullptr;
	std::unique_ptr<MixerTrackIOComponent> midiInput = nullptr;
	std::unique_ptr<MixerTrackIOComponent> audioInput = nullptr;
	std::unique_ptr<MixerTrackIOComponent> midiOutput = nullptr;
	std::unique_ptr<MixerTrackIOComponent> audioOutput = nullptr;
	std::unique_ptr<KnobBase> gainKnob = nullptr;
	std::unique_ptr<KnobBase> panKnob = nullptr;
	std::unique_ptr<FaderBase> fader = nullptr;
	std::unique_ptr<MixerTrackLevelMeter> levelMeter = nullptr;
	std::unique_ptr<MixerTrackMuteComponent> muteButton = nullptr;
	std::unique_ptr<juce::ListBox> effectList = nullptr;

	void preDrop();
	void endDrop();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerTrackComponent)
};

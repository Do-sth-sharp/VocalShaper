#pragma once

#include <JuceHeader.h>
#include "SeqTrackMuteComponent.h"
#include "SeqTrackRecComponent.h"
#include "SeqTrackIOComponent.h"
#include "SeqTrackLevelMeter.h"
#include "RightClickableTextButton.h"

class SeqTrackComponent final
	: public juce::Component,
	public juce::DragAndDropTarget {
public:
	SeqTrackComponent();

	void update(int index);
	void updateBlock(int blockIndex);
	void updateMute();
	void updateRec();
	void updateInstr();
	void updateHPos(double pos, double itemSize);

	void resized() override;
	void paint(juce::Graphics& g) override;
	void paintOverChildren(juce::Graphics& g) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

	bool isInterestedInDragSource(
		const SourceDetails& dragSourceDetails) override;
	void itemDragEnter(const SourceDetails& dragSourceDetails) override;
	void itemDragExit(const SourceDetails& dragSourceDetails) override;
	void itemDropped(const SourceDetails& dragSourceDetails) override;

private:
	int index = -1;
	juce::Colour trackColor, idColor;
	bool dragHovered = false;

	std::unique_ptr<juce::TextButton> trackName = nullptr;
	std::unique_ptr<SeqTrackMuteComponent> muteButton = nullptr;
	std::unique_ptr<SeqTrackRecComponent> recButton = nullptr;

	std::unique_ptr<RightClickableTextButton> instrButton = nullptr;
	std::unique_ptr<juce::DrawableButton> instrBypassButton = nullptr;
	std::unique_ptr<juce::DrawableButton> instrOfflineButton = nullptr;
	std::unique_ptr<juce::Drawable> instrBypassIcon = nullptr, instrBypassIconOn = nullptr;
	std::unique_ptr<juce::Drawable> instrOfflineIcon = nullptr, instrOfflineIconOn = nullptr;

	std::unique_ptr<SeqTrackIOComponent> midiOutput = nullptr;
	std::unique_ptr<SeqTrackIOComponent> audioOutput = nullptr;
	std::unique_ptr<SeqTrackLevelMeter> levelMeter = nullptr;

	void editTrackName();
	void instrEditorShow();
	void instrBypass();
	void instrOffline();
	void instrMenuShow();

	void setInstr(const juce::String& pid);

	void preDrop();
	void endDrop();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackComponent)
};

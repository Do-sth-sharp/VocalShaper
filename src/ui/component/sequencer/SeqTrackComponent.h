#pragma once

#include <JuceHeader.h>
#include "SeqTrackMuteComponent.h"
#include "SeqTrackRecComponent.h"
#include "SeqTrackIOComponent.h"
#include "SeqTrackLevelMeter.h"
#include "../base/RightClickableTextButton.h"
#include "SeqTrackContentViewer.h"

class SeqTrackComponent final
	: public juce::Component,
	public juce::DragAndDropTarget,
	public juce::FileDragAndDropTarget,
	public juce::SettableTooltipClient {
public:
	using ScrollFunc = std::function<void(double)>;
	using WheelFunc = std::function<void(float, bool)>;
	using WheelAltFunc = std::function<void(double, double, float, bool)>;
	using DragStartFunc = std::function<void(void)>;
	using DragProcessFunc = std::function<void(int, int, bool, bool)>;
	using DragEndFunc = std::function<void(void)>;
	using SeqTrackSelectFunc = std::function<void(int, bool)>;
	SeqTrackComponent(const ScrollFunc& scrollFunc,
		const WheelFunc& wheelHFunc,
		const WheelAltFunc& wheelAltHFunc,
		const WheelFunc& wheelVFunc,
		const WheelAltFunc& wheelAltVFunc,
		const DragStartFunc& dragStartFunc,
		const DragProcessFunc& dragProcessFunc,
		const DragEndFunc& dragEndFunc,
		const SeqTrackSelectFunc& trackSelectFunc);

	void update(int index);
	void updateBlock(int blockIndex);
	void updateMute();
	void updateRec();
	void updateInstr();
	void updateHPos(double pos, double itemSize);
	void updateMixerTrack();
	void updateDataRef();
	void updateData();
	void updateSynthState(bool state);

	void resized() override;
	void paint(juce::Graphics& g) override;
	void paintOverChildren(juce::Graphics& g) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseWheelMove(const juce::MouseEvent& event,
		const juce::MouseWheelDetails& wheel) override;

	bool isInterestedInDragSource(
		const SourceDetails& dragSourceDetails) override;
	void itemDragEnter(const SourceDetails& dragSourceDetails) override;
	void itemDragExit(const SourceDetails& dragSourceDetails) override;
	void itemDropped(const SourceDetails& dragSourceDetails) override;

	bool isInterestedInFileDrag(const juce::StringArray& files) override;
	void fileDragEnter(const juce::StringArray& files, int x, int y) override;
	void fileDragExit(const juce::StringArray& files) override;
	void filesDropped(const juce::StringArray& files, int x, int y) override;

	void focusGained(FocusChangeType cause) override;
	void focusLost(FocusChangeType cause) override;

private:
	const WheelFunc wheelVFunc;
	const WheelAltFunc wheelAltVFunc;
	const SeqTrackSelectFunc trackSelectFunc;

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

	std::unique_ptr<SeqTrackContentViewer> content = nullptr;

	void editTrackName();
	void instrEditorShow();
	void instrBypass();
	void instrOffline();
	void instrMenuShow();
	void menuShow();

	void setInstr(const juce::String& pid, bool addARA);
	void add();
	void remove();

	void setContentAudioRef(const juce::String& path);
	void setContentMIDIRef(const juce::String& path);

	void preDrop();
	void endDrop();

	juce::PopupMenu createInstrMenu(
		const std::function<void(const juce::PluginDescription&)>& addCallback) const;
	juce::PopupMenu createInstrAddMenu(
		const std::function<void(const juce::PluginDescription&)>& callback) const;
	juce::PopupMenu createMenu() const;
	juce::String createToolTipString() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackComponent)
};

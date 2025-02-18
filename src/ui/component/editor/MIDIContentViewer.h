#pragma once

#include <JuceHeader.h>
#include "../../misc/LevelMeterHub.h"

class MIDIContentViewer final
	: public juce::Component,
	public LevelMeterHub::Target {
public:
	using ScrollFunc = std::function<void(double)>;
	using WheelFunc = std::function<void(float, bool)>;
	using WheelAltFunc = std::function<void(double, double, float, bool)>;
	using MouseYPosFunc = std::function<void(float)>;
	using MouseLeaveFunc = std::function<void()>;
	using DragStartFunc = std::function<void(void)>;
	using DragProcessFunc = std::function<void(int, int, bool, bool)>;
	using DragEndFunc = std::function<void(void)>;
	MIDIContentViewer(
		const ScrollFunc& scrollFunc,
		const WheelFunc& wheelFunc,
		const WheelAltFunc& wheelAltFunc,
		const MouseYPosFunc& mouseYPosFunc,
		const MouseLeaveFunc& mouseLeaveFunc,
		const DragStartFunc& dragStartFunc,
		const DragProcessFunc& dragProcessFunc,
		const DragEndFunc& dragEndFunc);

	void update(int index, uint64_t ref);
	void updateTempoLabel();
	void updateBlocks();
	void updateData();
	void updateLevelMeter() override;

	void updateHPos(double pos, double itemSize);
	void updateVPos(double pos, double itemSize);

	void resized() override;
	void paint(juce::Graphics& g) override;
	void paintOverChildren(juce::Graphics& g) override;

	void mouseDown(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;
	void mouseWheelMove(const juce::MouseEvent& event,
		const juce::MouseWheelDetails& wheel) override;

	void midiChannelChanged();

private:
	const ScrollFunc scrollFunc;
	const WheelFunc wheelFunc;
	const WheelAltFunc wheelAltFunc;
	const MouseYPosFunc mouseYPosFunc;
	const MouseLeaveFunc mouseLeaveFunc;
	const DragStartFunc dragStartFunc;
	const DragProcessFunc dragProcessFunc;
	const DragEndFunc dragEndFunc;

	/** 0 for white key and 1 for black key */
	const std::array<int, 12> keyMasks{ 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 };
	const int totalKeys = 128;

	const juce::StringArray keyNames{ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

	int index = -1;
	uint64_t ref = 0;
	int currentMIDITrack = -1;

	double hPos = 0, hItemSize = 0;
	double secStart = 0, secEnd = 0;

	double vPos = 0, vItemSize = 0;
	double keyTop = 0, keyBottom = 0;

	double playPosSec = 0;
	double loopStartSec = 0, loopEndSec = 0;

	enum class LineItemType {
		Bar, Beat, Dashed
	};
	/** Place, Type, barId */
	using LineItem = std::tuple<double, LineItemType, int>;
	using LineItemList = juce::Array<LineItem>;
	LineItemList lineTemp;

	/** Start, End, SourceStart */
	using BlockItem = std::tuple<double, double, double>;
	juce::Array<BlockItem> blockItemTemp;

	bool viewMoving = false;

	/** Start, End, Num */
	struct Note final {
		double startSec, endSec;
		uint8_t num;
		uint8_t vel;
		uint8_t channel;
		juce::String lyrics;
	};
	juce::Array<Note> midiDataTemp;
	uint8_t midiMinNote = 0, midiMaxNote = 0;

	double noteInsertTime = -1, noteInsertLength = -1;
	uint8_t noteInsertPitch = 0, noteInsertChannel = 0;

	std::unique_ptr<juce::Image> rulerTemp = nullptr;
	std::unique_ptr<juce::Image> keyTemp = nullptr;
	std::unique_ptr<juce::Image> blockTemp = nullptr;
	std::unique_ptr<juce::Image> noteTemp = nullptr;

	juce::Colour trackColor;
	//bool trackColorIsLight = false;
	juce::Array<juce::Colour> noteColorGradient;
	juce::Array<juce::Colour> noteLabelColorGradient;

	/** Index, Rect, Channel */
	using NoteRectTemp = std::tuple<int, juce::Rectangle<float>, uint8_t>;
	juce::Array<NoteRectTemp> noteRectTempList;

	std::unique_ptr<juce::ChangeListener> midiChannelListener = nullptr;

	void updateKeyImageTemp();
	void updateRulerImageTemp();
	void updateBlockImageTemp();
	void updateNoteImageTemp();

	void updateMouseCursor(const juce::Point<float>& pos);

	enum class NoteControllerType {
		None, Left, Right, Inside
	};
	std::tuple<NoteControllerType, int> getNoteController(const juce::Point<float>& pos) const;
	std::tuple<NoteControllerType, int> getNoteControllerWithoutEdge(const juce::Point<float>& pos) const;

	std::tuple<double, double> getHViewArea(double pos, double itemSize) const;
	std::tuple<double, double> getVViewArea(double pos, double itemSize) const;

	const LineItemList createRulerLine(double pos, double itemSize) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIContentViewer)
};

#pragma once

#include <JuceHeader.h>

class SeqTrackContentViewer final : public juce::Component {
public:
	using ScrollFunc = std::function<void(double)>;
	using DragStartFunc = std::function<void(void)>;
	using DragProcessFunc = std::function<void(int, int, bool, bool)>;
	using DragEndFunc = std::function<void(void)>;
	SeqTrackContentViewer(const ScrollFunc& scrollFunc,
		const DragStartFunc& dragStartFunc,
		const DragProcessFunc& dragProcessFunc,
		const DragEndFunc& dragEndFunc);

	void setCompressed(bool isCompressed);

	void update(int index);
	void updateBlock(int blockIndex);
	void updateHPos(double pos, double itemSize);
	void updateDataRef();
	void updateData();
	void updateDataImage();

	void paint(juce::Graphics& g) override;

	void mouseMove(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseDown(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;

private:
	const ScrollFunc scrollFunc;
	const DragStartFunc dragStartFunc;
	const DragProcessFunc dragProcessFunc;
	const DragEndFunc dragEndFunc;

	bool compressed = false;
	int index = -1;
	double pos = 0, itemSize = 0;
	double secStart = 0, secEnd = 0;
	juce::Colour trackColor, nameColor;
	bool audioValid = false, midiValid = false;
	juce::String audioName, midiName;
	juce::String blockNameCombined;

	struct BlockItem final {
		double startTime = 0, endTime = 0, offset = 0;
	};
	juce::OwnedArray<BlockItem> blockTemp;

	std::tuple<double, juce::AudioSampleBuffer> audioDataTemp;
	juce::MidiMessageSequence midiDataTemp;

	juce::Array<juce::MemoryBlock> audioPointTemp;
	int midiMinNote = 0, midiMaxNote = 0;

	std::unique_ptr<juce::Timer> blockImageUpdateTimer = nullptr;

	enum class DragType {
		None, Add, Move, Left, Right
	};
	bool viewMoving = false;
	DragType dragType = DragType::None;
	int pressedBlockIndex = -1;
	double mousePressedSecond = 0;
	double mouseCurrentSecond = 0;
	float scissorsPosX = -1;
	bool blockValid = true;

	void updateBlockInternal(int blockIndex);
	void setAudioPointTempInternal(const juce::Array<juce::MemoryBlock>& temp);
	void updateMIDINoteTempInternal();

	enum class BlockControllerType {
		None, Left, Right, Inside
	};
	std::tuple<BlockControllerType, int> getBlockController(float posX) const;
	std::tuple<BlockControllerType, int> getBlockControllerWithoutEdge(float posX) const;

	double limitTimeSec(double timeSec) const;
	bool checkBlockValid(double startSec, double endSec, int excludeIndex = -1) const;

	void addBlock(double startSec, double endSec);
	void splitBlock(int blockIndex, double timeSec);
	void removeBlock(int blockIndex);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackContentViewer)
};

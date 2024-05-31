#include "SeqTrackContentViewer.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/AudioExtractor.h"
#include "../../misc/MainThreadPool.h"
#include "../../misc/Tools.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

class DataImageUpdateTimer final : public juce::Timer {
public:
	DataImageUpdateTimer() = delete;
	DataImageUpdateTimer(
		juce::Component::SafePointer<SeqTrackContentViewer> parent);

	void timerCallback() override;

private:
	const juce::Component::SafePointer<SeqTrackContentViewer> parent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataImageUpdateTimer)
};

DataImageUpdateTimer::DataImageUpdateTimer(
	juce::Component::SafePointer<SeqTrackContentViewer> parent)
	: parent(parent) {}

void DataImageUpdateTimer::timerCallback() {
	if (this->parent) {
		this->parent->updateDataImage();
	}
}

SeqTrackContentViewer::SeqTrackContentViewer(
	const ScrollFunc& scrollFunc,
	const DragStartFunc& dragStartFunc,
	const DragProcessFunc& dragProcessFunc,
	const DragEndFunc& dragEndFunc)
	: scrollFunc(scrollFunc),
	dragStartFunc(dragStartFunc), dragProcessFunc(dragProcessFunc),
	dragEndFunc(dragEndFunc) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSeqBlock());

	/** Data Update Timer */
	this->blockImageUpdateTimer = std::make_unique<DataImageUpdateTimer>(this);
	/**
	 * To fix the bug in AudioExtractor.
	 * This will invoke AudioExtractor::extractAsync(...) every period of time.
	 * I currently do not have a better solution.
	 */
	this->blockImageUpdateTimer->startTimer(5000);
}

void SeqTrackContentViewer::setCompressed(bool isCompressed) {
	this->compressed = isCompressed;
	this->repaint();
}

void SeqTrackContentViewer::update(int index) {
	bool changeData = true;//this->index != index;

	this->index = index;
	if (index > -1) {
		this->updateBlock(-1);
		
		this->trackColor = quickAPI::getSeqTrackColor(index);

		auto& laf = this->getLookAndFeel();
		if (utils::isLightColor(this->trackColor)) {
			this->nameColor = laf.findColour(
				juce::Label::ColourIds::textWhenEditingColourId);
		}
		else {
			this->nameColor = laf.findColour(
				juce::Label::ColourIds::textColourId);
		}

		if (changeData) {
			this->updateDataRef();
			this->updateData();
		}

		this->repaint();
	}
}

void SeqTrackContentViewer::updateBlock(int blockIndex) {
	/** Create Or Remove Block */
	int currentSize = this->blockTemp.size();
	int newSize = quickAPI::getBlockNum(this->index);
	if (currentSize > newSize) {
		for (int i = currentSize - 1; i >= newSize; i--) {
			this->blockTemp.remove(i);
		}
	}
	else {
		for (int i = currentSize; i < newSize; i++) {
			auto block = std::make_unique<BlockItem>();
			this->blockTemp.add(std::move(block));
		}
	}

	/** Update Blocks */
	if (blockIndex >= 0 && blockIndex < this->blockTemp.size()) {
		this->updateBlockInternal(blockIndex);
	}
	else {
		for (int i = 0; i < this->blockTemp.size(); i++) {
			this->updateBlockInternal(i);
		}
	}

	/** Repaint */
	this->repaint();
}

void SeqTrackContentViewer::updateHPos(double pos, double itemSize) {
	bool shouldRepaintDataImage = !juce::approximatelyEqual(this->itemSize, itemSize);

	this->pos = pos;
	this->itemSize = itemSize;

	this->secStart = pos / itemSize;
	this->secEnd = this->secStart + (this->getWidth() / itemSize);

	if (shouldRepaintDataImage) {
		this->updateDataImage();
	}

	this->repaint();
}

void SeqTrackContentViewer::updateDataRef() {
	this->audioValid = quickAPI::isSeqTrackHasAudioData(this->index);
	this->midiValid = quickAPI::isSeqTrackHasMIDIData(this->index);

	this->audioName = this->audioValid ? quickAPI::getSeqTrackDataRefAudio(this->index) : juce::String{};
	this->midiName = this->midiValid ? quickAPI::getSeqTrackDataRefMIDI(this->index) : juce::String{};

	this->blockNameCombined = this->audioName
		+ ((this->audioValid&& this->midiValid) ? " + " : "")
		+ this->midiName;

	this->updateData();

	this->repaint();
}

void SeqTrackContentViewer::updateData() {
	/** Clear Temp */
	this->audioDataTemp = {};
	this->midiDataTemp = juce::MidiMessageSequence{};
	//this->audioPointTemp.clear();
	//this->midiMinNote = this->midiMaxNote = 0;

	/** Get Audio Data */
	if (this->audioValid) {
		this->audioDataTemp = quickAPI::getSeqTrackAudioData(this->index);
	}

	/** Get MIDI Data */
	if (this->midiValid) {
		this->midiDataTemp = quickAPI::getSeqTrackMIDIData(this->index);
	}

	/** Update Image Temp */
	this->updateDataImage();

	/** Repaint */
	this->repaint();
}

void SeqTrackContentViewer::updateDataImage() {
	/** Audio Data */
	if (this->audioValid) {
		/** Get Data */
		auto& [sampleRate, data] = this->audioDataTemp;

		/** Prepare Callback */
		auto callback = [comp = SafePointer{ this }](const AudioExtractor::Result& result) {
			if (comp) {
				/** Set Temp */
				comp->setAudioPointTempInternal(result);

				/** Repaint */
				comp->repaint();
			}
			};

		/** Get Point Num */
		double lengthSec = data.getNumSamples() / sampleRate;
		int pointNum = std::floor(lengthSec * this->itemSize);

		/** Extract */
		AudioExtractor::getInstance() ->extractAsync(
			this, data, pointNum, callback);
	}
	
	/** MIDI Data */
	if (this->midiValid) {
		/** Update MIDI */
		this->updateMIDINoteTempInternal();

		/** Repaint */
		this->repaint();
	}
}

void SeqTrackContentViewer::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float paddingHeight = screenSize.getHeight() * 0.0025;
	float blockRadius = screenSize.getHeight() * 0.005;
	float outlineThickness = screenSize.getHeight() * 0.0015;

	float blockPaddingHeight = screenSize.getHeight() * 0.01;
	float blockPaddingWidth = screenSize.getWidth() * 0.01;

	float blockNameFontHeight = screenSize.getHeight() * 0.015;

	float noteMaxHeight = screenSize.getHeight() * 0.015;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour outlineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);

	/** Font */
	juce::Font blockNameFont(blockNameFontHeight);

	/** Data Scale Ratio */
	auto& [sampleRate, data] = this->audioDataTemp;
	double dstLengthSec = data.getNumSamples() / sampleRate;
	int dstPointNum = std::floor(dstLengthSec * this->itemSize);
	double dstPointPerSec = this->itemSize;

	double imgScaleRatio = 1;
	if (this->audioPointTemp.size() > 0) {
		auto& data = this->audioPointTemp.getReference(0);
		imgScaleRatio = dstPointNum / (double)(data.getSize() / 2 / sizeof(float));
	}

	/** Blocks */
	for (auto block : this->blockTemp) {
		if ((block->startTime <= this->secEnd)
			&& (block->endTime >= this->secStart)) {
			float startPos = (block->startTime - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
			float endPos = (block->endTime - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();

			/** Rect */
			juce::Rectangle<float> blockRect(
				startPos, paddingHeight,
				endPos - startPos, this->getHeight() - paddingHeight * 2);
			g.setColour(this->trackColor.withAlpha(0.5f));
			g.fillRoundedRectangle(blockRect, blockRadius);
			g.setColour(outlineColor);
			g.drawRoundedRectangle(blockRect, blockRadius, outlineThickness);

			/** Name */
			float nameStartPos = std::max(startPos, 0.f) + blockPaddingWidth;
			float nameEndPos = std::min(endPos, (float)this->getWidth()) - blockPaddingWidth;
			juce::Rectangle<float> nameRect(
				nameStartPos, this->compressed ? 0 : blockPaddingHeight,
				nameEndPos - nameStartPos, this->compressed ? this->getHeight() : blockNameFontHeight);
			g.setColour(this->nameColor);
			g.setFont(blockNameFont);
			g.drawFittedText(this->blockNameCombined, nameRect.toNearestInt(),
				juce::Justification::centredLeft, 1, 1.f);

			/** Wave */
			if (!this->compressed) {
				/** Select Time */
				double startSec = std::max(block->startTime + block->offset, this->secStart);
				double endSec = std::min(block->endTime + block->offset, this->secEnd);
				int startPixel = startSec * dstPointPerSec / imgScaleRatio;
				int endPixel = endSec * dstPointPerSec / imgScaleRatio;

				/** Paint Each Channel */
				g.setColour(this->nameColor);
				float wavePosY = blockPaddingHeight + blockNameFontHeight + blockPaddingHeight;
				float channelHeight = (this->getHeight() - blockPaddingHeight - wavePosY) / (float)this->audioPointTemp.size();
				for (int i = 0; i < this->audioPointTemp.size(); i++) {
					float channelPosY = wavePosY + channelHeight * i;
					auto& data = this->audioPointTemp.getReference(i);
					int dataSize = data.getSize() / 2 / sizeof(float);

					/** Paint Each Point */
					for (int j = std::max(startPixel, 0); j <= endPixel && j < dataSize; j++) {
						/** Get Value */
						float minVal = 0, maxVal = 0;
						data.copyTo(&minVal, (j * 2 + 0) * (int)sizeof(float), sizeof(float));
						data.copyTo(&maxVal, (j * 2 + 1) * (int)sizeof(float), sizeof(float));

						/** Paint Point */
						double pixelSec = j * imgScaleRatio / dstPointPerSec;
						float pixelPosX = (pixelSec - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
						juce::Rectangle<float> pointRect(
							pixelPosX,
							channelPosY + channelHeight / 2.f - (maxVal / 1.f) * channelHeight / 2.f,
							imgScaleRatio,
							channelHeight * ((maxVal - minVal) / 2.f));
						g.fillRect(pointRect);
					}
				}
			}

			/** Note */
			if (!this->compressed) {
				/** Select Time */
				double startSec = std::max(block->startTime + block->offset, this->secStart);
				double endSec = std::min(block->endTime + block->offset, this->secEnd);

				/** Content Area */
				float notePosY = blockPaddingHeight + blockNameFontHeight + blockPaddingHeight;
				float noteAreaHeight = this->getHeight() - blockPaddingHeight - notePosY;
				juce::Rectangle<float> noteContentRect(0, notePosY, this->getWidth(), noteAreaHeight);

				/** Limit Note Height */
				double minNoteID = this->midiMinNote, maxNoteID = this->midiMaxNote;
				float noteHeight = noteAreaHeight / (maxNoteID - minNoteID + 1);
				if (noteHeight > noteMaxHeight) {
					noteHeight = noteMaxHeight;

					double centerNoteID = minNoteID + (maxNoteID - minNoteID) / 2;
					minNoteID = centerNoteID - ((noteAreaHeight / noteHeight + 1) / 2 - 1);
					maxNoteID = centerNoteID + ((noteAreaHeight / noteHeight + 1) / 2 - 1);
				}

				/** Paint Each Note */
				g.setColour(this->nameColor);
				
				std::array<double, 128> noteStartTime{};
				for (int i = 0; i < 128; i++) {
					noteStartTime[i] = -1;
				}

				for (auto event : this->midiDataTemp) {
					if (event->message.isNoteOn(true)) {
						int noteNumber = event->message.getNoteNumber();
						if (noteNumber >= 0 && noteNumber < 128) {
							noteStartTime[noteNumber] = event->message.getTimeStamp();
						}
					}
					else if (event->message.isNoteOff(false)) {
						int noteNumber = event->message.getNoteNumber();
						if (noteNumber >= 0 && noteNumber < 128) {
							double noteStart = noteStartTime[noteNumber];
							noteStartTime[noteNumber] = -1;
							if (noteStart >= 0) {
								double noteEnd = event->message.getTimeStamp();
								if (noteStart <= endSec && noteEnd >= startSec) {
									juce::Rectangle<float> noteRect(
										(noteStart - this->secStart) / (this->secEnd - this->secStart) * this->getWidth(),
										notePosY + (maxNoteID - event->message.getNoteNumber()) * noteHeight,
										(noteEnd - noteStart) / (this->secEnd - this->secStart) * this->getWidth(),
										noteHeight);
									g.fillRect(noteRect);
								}
							}
						}
					}
				}
			}
		}
	}
}

void SeqTrackContentViewer::mouseMove(const juce::MouseEvent& event) {
	/** Move */
	if (Tools::getInstance()->getType() == Tools::Type::Hand) {
		this->setMouseCursor(juce::MouseCursor::DraggingHandCursor);
		return;
	}

	/** Block */
	float posX = event.position.getX();
	std::tuple<SeqTrackContentViewer::BlockControllerType, int> blockController;
	switch (Tools::getInstance()->getType()) {
	case Tools::Type::Arrow:
	case Tools::Type::Eraser:
	case Tools::Type::Scissors:
		blockController = this->getBlockControllerWithoutEdge(posX);
		break;
	case Tools::Type::Magic:
	case Tools::Type::Pencil:
		blockController = this->getBlockController(posX);
		break;
	}

	if (std::get<0>(blockController) == BlockControllerType::Left) {
		this->setMouseCursor(juce::MouseCursor::LeftEdgeResizeCursor);
		return;
	}
	else if (std::get<0>(blockController) == BlockControllerType::Right) {
		this->setMouseCursor(juce::MouseCursor::RightEdgeResizeCursor);
		return;
	}
	else if (std::get<0>(blockController) == BlockControllerType::Inside) {
		this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
		return;
	}

	this->setMouseCursor(juce::MouseCursor::NormalCursor);
}

void SeqTrackContentViewer::mouseDrag(const juce::MouseEvent& event) {
	/** Auto Scroll */
	float xPos = event.position.getX();
	if (!this->viewMoving) {
		double delta = 0;
		if (xPos > this->getWidth()) {
			delta = xPos - this->getWidth();
		}
		else if (xPos < 0) {
			delta = xPos;
		}

		if (delta != 0) {
			this->scrollFunc(delta / 4);
		}
	}

	if (event.mods.isLeftButtonDown()) {
		/** Move View */
		if (this->viewMoving) {
			int distanceX = event.getDistanceFromDragStartX();
			int distanceY = event.getDistanceFromDragStartY();
			this->dragProcessFunc(distanceX, distanceY, true, true);
		}

		/** Edit Block */
		else if (this->dragType != DragType::None) {
			this->mouseCurrentSecond = this->secStart + (event.position.x / (double)this->getWidth()) * (this->secEnd - this->secStart);
			this->repaint();
		}
	}
}

void SeqTrackContentViewer::mouseDown(const juce::MouseEvent& event) {
	/** Get Tool Type */
	auto tool = Tools::getInstance()->getType();

	if (event.mods.isLeftButtonDown()) {
		/** Check Tool Type */
		switch (tool) {
		case Tools::Type::Hand:
			/** Move View Area */
			this->viewMoving = true;
			this->dragStartFunc();
			break;
		case Tools::Type::Magic:
		case Tools::Type::Arrow: {
			/** Edit */
			auto [controller, index] = this->getBlockController(event.position.x);

			if (index > -1) {
				this->pressedBlockIndex = index;
				if (controller == BlockControllerType::Inside) {
					this->dragType = DragType::Move;
				}
				else if (controller == BlockControllerType::Left) {
					this->dragType = DragType::Left;
				}
				else if (controller == BlockControllerType::Right) {
					this->dragType = DragType::Right;
				}
				this->mousePressedSecond = this->secStart + (event.position.x / (double)this->getWidth()) * (this->secEnd - this->secStart);
			}
			break;
		}
		case Tools::Type::Pencil: {
			this->mousePressedSecond = this->secStart + (event.position.x / (double)this->getWidth()) * (this->secEnd - this->secStart);
			this->dragType = DragType::Add;
			break;
		}
		case Tools::Type::Eraser: {
			/** TODO */
			break;
		}
		case Tools::Type::Scissors: {
			/** TODO */
			break;
		}
		}
	}
	else if (event.mods.isRightButtonDown()) {
		/** TODO */
	}
}

void SeqTrackContentViewer::mouseUp(const juce::MouseEvent& event) {
	/** Move View */
	if (this->viewMoving) {
		this->dragEndFunc();
	}

	/** Other */
	this->dragType = DragType::None;
	this->pressedBlockIndex = -1;
	this->mousePressedSecond = 0;
	this->mouseCurrentSecond = 0;
}

void SeqTrackContentViewer::updateBlockInternal(int blockIndex) {
	if (auto temp = this->blockTemp[blockIndex]) {
		std::tie(temp->startTime, temp->endTime, temp->offset)
			= quickAPI::getBlock(this->index, blockIndex);
	}
}

void SeqTrackContentViewer::setAudioPointTempInternal(
	const juce::Array<juce::MemoryBlock>& temp) {
	this->audioPointTemp = temp;
}

void SeqTrackContentViewer::updateMIDINoteTempInternal() {
	/** Get Note Zone */
	int minNote = 127, maxNote = 0;
	for (auto e : this->midiDataTemp) {
		if (e->message.isNoteOn(true)) {
			minNote = std::min(minNote, e->message.getNoteNumber());
			maxNote = std::max(maxNote, e->message.getNoteNumber());
		}
	}
	if (maxNote < minNote) { maxNote = minNote = 0; }
	this->midiMinNote = minNote;
	this->midiMaxNote = maxNote;
}

std::tuple<SeqTrackContentViewer::BlockControllerType, int> 
SeqTrackContentViewer::getBlockController(float posX) const {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int blockJudgeWidth = screenSize.getWidth() * 0.005;

	/** Get Each Block */
	for (int i = 0; i < this->blockTemp.size(); i++) {
		/** Get Block Time */
		auto block = this->blockTemp.getUnchecked(i);
		float startX = (block->startTime - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
		float endX = (block->endTime - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();

		/** Judge Area */
		float judgeSSX = startX - blockJudgeWidth, judgeSEX = startX + blockJudgeWidth;
		float judgeESX = endX - blockJudgeWidth, judgeEEX = endX + blockJudgeWidth;
		if (endX - startX < blockJudgeWidth * 2) {
			judgeSEX = startX + (endX - startX) / 2;
			judgeESX = endX - (endX - startX) / 2;
		}
		if (i > 0) {
			auto lastBlock = this->blockTemp.getUnchecked(i - 1);
			float lastEndX = (lastBlock->endTime - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
			if (startX - lastEndX < blockJudgeWidth * 2) {
				judgeSSX = startX - (startX - lastEndX) / 2;
			}
		}
		if (i < this->blockTemp.size() - 1) {
			auto nextBlock = this->blockTemp.getUnchecked(i + 1);
			float nextStartX = (nextBlock->startTime - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
			if (nextStartX - endX < blockJudgeWidth * 2) {
				judgeEEX = endX + (nextStartX - endX) / 2;
			}
		}

		/** Get Controller */
		if (posX >= judgeSSX && posX < judgeSEX) {
			return { BlockControllerType::Left, i };
		}
		else if (posX >= judgeESX && posX < judgeEEX) {
			return { BlockControllerType::Right, i };
		}
		else if (posX >= judgeSEX && posX < judgeESX) {
			return { BlockControllerType::Inside, i };
		}
	}

	/** None */
	return { BlockControllerType::None, -1 };
}

std::tuple<SeqTrackContentViewer::BlockControllerType, int>
SeqTrackContentViewer::getBlockControllerWithoutEdge(float posX) const {
	/** Get Each Block */
	for (int i = 0; i < this->blockTemp.size(); i++) {
		/** Get Block Time */
		auto block = this->blockTemp.getUnchecked(i);
		float startX = (block->startTime - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
		float endX = (block->endTime - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();

		/** Set Cursor */
		if (posX >= startX && posX < endX) {
			return { BlockControllerType::Inside, i };
		}
	}

	/** None */
	return { BlockControllerType::None, -1 };
}

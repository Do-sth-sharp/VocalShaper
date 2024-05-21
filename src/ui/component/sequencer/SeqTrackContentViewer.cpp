﻿#include "SeqTrackContentViewer.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/AudioExtractor.h"
#include "../../misc/MainThreadPool.h"
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

SeqTrackContentViewer::SeqTrackContentViewer() {
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
	this->midiDataTemp = juce::MidiFile{};
	this->audioPointTemp.clear();

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
		/** TODO Update MIDI */
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
		}
	}
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

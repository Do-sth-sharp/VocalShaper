#include "SeqTrackContentViewer.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

SeqTrackContentViewer::SeqTrackContentViewer() {
	/** TODO */
}

void SeqTrackContentViewer::setCompressed(bool isCompressed) {
	this->compressed = isCompressed;
	this->repaint();
}

void SeqTrackContentViewer::update(int index) {
	this->index = index;
	if (index > -1) {
		this->updateBlock(-1);
		
		this->trackColor = quickAPI::getSeqTrackColor(index);

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
	this->pos = pos;
	this->itemSize = itemSize;

	this->secStart = pos / itemSize;
	this->secEnd = this->secStart + (this->getWidth() / itemSize);

	this->repaint();
}

void SeqTrackContentViewer::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float paddingHeight = screenSize.getHeight() * 0.0025;
	float blockRadius = screenSize.getHeight() * 0.005;

	/** Blocks */
	for (auto block : this->blockTemp) {
		if ((block->startTime <= this->secEnd)
			&& (block->endTime >= this->secStart)) {
			float startPos = (block->startTime - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
			float endPos = (block->endTime - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();

			juce::Rectangle<float> blockRect(
				startPos, paddingHeight,
				endPos - startPos, this->getHeight() - paddingHeight * 2);
			g.setColour(this->trackColor.withAlpha(0.5f));
			g.fillRoundedRectangle(blockRect, blockRadius);
		}
	}
}

void SeqTrackContentViewer::updateBlockInternal(int blockIndex) {
	if (auto temp = this->blockTemp[blockIndex]) {
		std::tie(temp->startTime, temp->endTime, temp->offset)
			= quickAPI::getBlock(this->index, blockIndex);
	}
}

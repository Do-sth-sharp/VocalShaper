#include "SeqTrackContentViewer.h"
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
	bool shouldRepaintAudioImageTemp = !juce::approximatelyEqual(this->pos, pos);

	this->pos = pos;
	this->itemSize = itemSize;

	this->secStart = pos / itemSize;
	this->secEnd = this->secStart + (this->getWidth() / itemSize);

	if (shouldRepaintDataImage) {
		this->updateDataImage();
	}
	else if (shouldRepaintAudioImageTemp) {
		this->updateAudioImage();
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
	this->audioImageTemp = nullptr;
	this->audioImageIndex.clear();
	this->midiImageTemp = nullptr;
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

				/** Update Image */
				comp->updateAudioImage();
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
		/** Update Image */
		this->updateMIDIImage();
	}
}

void SeqTrackContentViewer::updateAudioImage() {
	/** Compressed */
	if (this->compressed) {
		return;
	}

	/** Get Clips */
	double hotSecStart = this->secStart - (this->secEnd - this->secStart);
	double hotSecEnd = this->secEnd + (this->secEnd - this->secStart);
	juce::Array<std::pair<int, int>> clips;
	for (auto block : this->blockTemp) {
		if ((block->startTime <= hotSecEnd)
			&& (block->endTime >= hotSecStart)) {
			double hotBlockSecStart = block->startTime - (block->endTime - block->startTime);
			double hotBlockSecEnd = block->endTime + (block->endTime - block->startTime);

			double hotClipSecStart = std::max(hotSecStart, hotBlockSecStart);
			double hotClipSecEnd = std::min(hotSecEnd, hotBlockSecEnd);

			clips.add({ (int)std::floor(hotClipSecStart * this->itemSize),
				(int)std::floor(hotClipSecEnd * this->itemSize) });
		}
	}

	/** Join Into Union */
	auto clipUnion = utils::getUnionSections(clips);
	int totalPoints = 0;
	for (auto& i : clipUnion) {
		totalPoints += (i.second - i.first + 1);
	}

	/** Get Size */
	auto screenSize = utils::getScreenSize(this);
	float blockPaddingHeight = screenSize.getHeight() * 0.01;
	float blockNameFontHeight = screenSize.getHeight() * 0.015;

	int height = this->getHeight();

	/** Get Color */
	juce::Colour waveColor = this->nameColor;

	/** Content Rect */
	int posY = blockPaddingHeight + blockNameFontHeight + blockPaddingHeight;
	juce::Rectangle<int> contentRect(
		0, posY, totalPoints, height - blockPaddingHeight - posY);

	/** Callback */
	auto callback = [comp = SafePointer{ this }](
		const juce::Image& image, const ImageIndexList& index) {
		if (comp) {
			comp->setAudioImageTemp(image, index);
		}
		};

	/** Paint Job */
	auto paintJob = [temp = this->audioPointTemp,
		clipUnion, contentRect, waveColor, callback] {
		/** Create Image With Graph */
		juce::Image image(
			juce::Image::PixelFormat::ARGB,
			std::max(contentRect.getWidth(), 1),
			std::max(contentRect.getHeight(), 1), true);
		juce::Graphics g(image);
		g.setColour(waveColor);

		/** Paint Each Clip */
		ImageIndexList imageIndex;
		int imageIndexTemp = 0;

		float channelHeight = contentRect.getHeight() / (float)temp.size();
		for (auto& i : clipUnion) {
			int clipSize = i.second - i.first + 1;
			imageIndex.add({ i, imageIndexTemp });

			/** Paint Each Channel */
			for (int j = 0; j < temp.size(); j++) {
				auto& data = temp.getReference(j);
				juce::Rectangle<float> channelRect(
					imageIndexTemp, channelHeight * j, clipSize, channelHeight);
				int pointNum = data.getSize() / 2 / sizeof(float);

				/** Paint Each Point */
				for (int k = std::max(i.first, 0); k <= i.second && k < pointNum; k++) {
					/** Get Value */
					float minVal = 0, maxVal = 0;
					data.copyTo(&minVal, (k * 2 + 0) * (int)sizeof(float), sizeof(float));
					data.copyTo(&maxVal, (k * 2 + 1) * (int)sizeof(float), sizeof(float));

					/** Paint */
					juce::Rectangle<float> lineRect(
						imageIndexTemp + (k - i.first),
						channelRect.getCentreY() - (maxVal / 1.f) * channelRect.getHeight() / 2.f,
						1, channelRect.getHeight() * ((maxVal - minVal) / 2.f));
					g.fillRect(lineRect);
				}
			}

			imageIndexTemp += clipSize;
		}

		/** Invoke Callback */
		auto cb = [callback, image, imageIndex] {
			callback(image, imageIndex);
			};
		juce::MessageManager::callAsync(cb);
		};

	/** Run Job */
	MainThreadPool::getInstance()->runJob(paintJob);
}

void SeqTrackContentViewer::updateMIDIImage() {
	/** TODO */
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
				if (this->audioImageTemp) {
					/** Select Time */
					double startSec = std::max(block->startTime + block->offset, this->secStart);
					double endSec = std::min(block->endTime + block->offset, this->secEnd);
					int startPixel = startSec * dstPointPerSec / imgScaleRatio;
					int endPixel = endSec * dstPointPerSec / imgScaleRatio;

					/** Find Index Temp */
					for (auto& i : this->audioImageIndex) {
						if (startPixel <= i.first.second &&
							endPixel >= i.first.first) {
							int realStartPixel = std::max(startPixel, i.first.first);
							int realEndPixel = std::min(endPixel, i.first.second);

							double startLimitPercent = (realStartPixel - startPixel) / (double)(endPixel - startPixel);
							double endLimitPercent = (realEndPixel - startPixel) / (double)(endPixel - startPixel);
							float blockStartPosX = blockRect.getX() + blockRect.getWidth() * startLimitPercent;
							float blockEndPosX = blockRect.getX() + blockRect.getWidth() * endLimitPercent;

							/** Draw Content */
							float nameAreaHeight = blockPaddingHeight + blockNameFontHeight + blockPaddingHeight;
							juce::Rectangle<float> contentRect(
								blockStartPosX, nameAreaHeight,
								blockEndPosX - blockStartPosX,
								this->getHeight() - blockPaddingHeight - nameAreaHeight);
							juce::Rectangle<int> imageRect(
								i.second + (realStartPixel - i.first.first), 0,
								realEndPixel - realStartPixel, this->audioImageTemp->getHeight());
							g.drawImage(this->audioImageTemp->getClippedImage(imageRect), contentRect,
								juce::RectanglePlacement::stretchToFit, false);
						}
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

void SeqTrackContentViewer::setAudioImageTemp(
	const juce::Image& image, const ImageIndexList& index) {
	this->audioImageTemp = std::make_unique<juce::Image>(image);
	this->audioImageIndex = index;
	this->repaint();
}

void SeqTrackContentViewer::setMIDIImageTemp(const juce::Image& image) {
	this->midiImageTemp = std::make_unique<juce::Image>(image);
	this->repaint();
}

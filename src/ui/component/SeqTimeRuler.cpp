#include "SeqTimeRuler.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/Tools.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

SeqTimeRuler::SeqTimeRuler(
	const ScrollFunc& scrollFunc,
	const ScaleFunc& scaleFunc)
	: scrollFunc(scrollFunc), scaleFunc(scaleFunc) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forTimeRuler());

	/** Init Temp */
	this->rulerTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, 1, 1, false);
}

void SeqTimeRuler::updateBlock(int /*track*/, int /*index*/) {
	/** Get Total Length */
	this->totalLengthSec = quickAPI::getTotalLength();
}

void SeqTimeRuler::updateHPos(double pos, double itemSize) {
	/** Set Pos */
	this->pos = pos;
	this->itemSize = itemSize;
	std::tie(this->secStart, this->secEnd) = this->getViewArea(pos, itemSize);

	/** Update Line Temp */
	std::tie(this->lineTemp, this->minInterval) = this->createRulerLine(pos, itemSize);
	this->updateRulerTemp();
}

void SeqTimeRuler::updateRulerTemp() {
	/** Temp Size */
	juce::Graphics g(*(this->rulerTemp.get()));

	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float outlineThickness = screenSize.getHeight() * 0.0015;
	float lineThickness = screenSize.getWidth() * 0.00075;
	float longLineHeight = screenSize.getHeight() * 0.015;
	float shortLineHeight = screenSize.getHeight() * 0.0075;

	float shortLineIntervalMin = screenSize.getWidth() * 0.01;

	float numFontHeight = screenSize.getHeight() * 0.015;
	float numAreaWidth = screenSize.getWidth() * 0.05;
	float numPaddingWidth = screenSize.getWidth() * 0.0015;
	float numPaddingHeight = screenSize.getHeight() * 0.001;

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour outlineColor = laf.findColour(
		juce::Label::ColourIds::outlineWhenEditingColourId);
	juce::Colour lineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);
	juce::Colour numColor = laf.findColour(
		juce::Label::ColourIds::textColourId);

	/** Font */
	juce::Font numFont(numFontHeight);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Outline */
	auto outlineRect = this->getLocalBounds();
	g.setColour(outlineColor);
	g.drawRect(outlineRect, outlineThickness);

	/** Lines */
	for (auto& [xPos, isLong, barId] : this->lineTemp) {
		/** Check Interval */
		if (!isLong) {
			if (this->minInterval < shortLineIntervalMin) {
				continue;
			}
		}

		/** Line */
		float lineLength = isLong ? longLineHeight : shortLineHeight;
		juce::Rectangle<float> lineRect(
			xPos - lineThickness / 2, this->getHeight() - lineLength,
			lineThickness, lineLength);

		g.setColour(lineColor);
		g.fillRect(lineRect);

		/** Num */
		if (isLong) {
			juce::Rectangle<float> numRect(
				xPos + numPaddingWidth,
				this->getHeight() - numPaddingHeight - numFontHeight,
				numAreaWidth, numFontHeight);

			g.setColour(numColor);
			g.setFont(numFont);
			g.drawFittedText(juce::String{ barId }, numRect.toNearestInt(),
				juce::Justification::centredLeft, 1, 0.5f);
		}
	}
}

void SeqTimeRuler::updateLevelMeter() {
	/** Get Play Position */
	this->playPosSec = quickAPI::getTimeInSecond();

	/** Get Loop Time */
	std::tie(this->loopStartSec, this->loopEndSec) = quickAPI::getLoopTimeSec();

	/** Repaint */
	this->repaint();
}

void SeqTimeRuler::resized() {
	/** Update Line Temp */
	std::tie(this->secStart, this->secEnd) = this->getViewArea(this->pos, this->itemSize);
	std::tie(this->lineTemp, this->minInterval) = this->createRulerLine(pos, itemSize);

	/** Update Ruler Temp */
	int width = this->getWidth(), height = this->getHeight();
	width = std::max(width, 1);
	height = std::max(height, 1);
	this->rulerTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, width, height, false);
	this->updateRulerTemp();
}

void SeqTimeRuler::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float cursorThickness = screenSize.getWidth() * 0.00075;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour cursorColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);
	juce::Colour offColor = laf.findColour(
		juce::Label::ColourIds::backgroundWhenEditingColourId);

	/** Ruler */
	if (!this->rulerTemp) { return; }
	g.drawImageAt(*(this->rulerTemp.get()), 0, 0);

	/** Cursor */
	float cursorPosX = ((this->playPosSec - this->secStart) / (this->secEnd - this->secStart)) * this->getWidth();
	juce::Rectangle<float> cursorRect(
		cursorPosX - cursorThickness / 2, 0,
		cursorThickness, this->getHeight());

	g.setColour(cursorColor);
	g.fillRect(cursorRect);

	/** Time Off */
	if (this->loopEndSec > this->loopStartSec) {
		/** Left */
		if (this->loopStartSec > this->secStart) {
			float xPos = (this->loopStartSec - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
			juce::Rectangle<float> offRect(
				0, 0, xPos, this->getHeight());

			g.setColour(offColor);
			g.fillRect(offRect);
		}

		/** Right */
		if (this->loopEndSec < this->secEnd) {
			float xPos = (this->loopEndSec - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
			juce::Rectangle<float> offRect(
				xPos, 0, this->getWidth() - xPos, this->getHeight());

			g.setColour(offColor);
			g.fillRect(offRect);
		}
	}
}

void SeqTimeRuler::mouseDown(const juce::MouseEvent& event) {
	float xPos = event.position.getX();

	/** Play Position Changed */
	if (event.mods.isLeftButtonDown()) {
		double per = xPos / (double)this->getWidth();
		double timeSec = this->secStart + (this->secEnd - this->secStart) * per;

		quickAPI::setPlayPosition(this->limitTimeSec(timeSec));
	}

	/** Loop Changed */
	else if (event.mods.isRightButtonDown()) {
		double per = xPos / (double)this->getWidth();
		this->mouseDownSecTemp = this->limitTimeSec(this->secStart + (this->secEnd - this->secStart) * per);
	}
}

void SeqTimeRuler::mouseDrag(const juce::MouseEvent& event) {
	/** Auto Scroll */
	float xPos = event.position.getX();
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

	/** Play Position Changed */
	if (event.mods.isLeftButtonDown()) {
		double per = xPos / (double)this->getWidth();
		double timeSec = this->secStart + (this->secEnd - this->secStart) * per;

		quickAPI::setPlayPosition(this->limitTimeSec(timeSec));
	}

	/** Loop Changed */
	else if (event.mods.isRightButtonDown()) {
		double per = xPos / (double)this->getWidth();
		double timeSec = this->limitTimeSec(this->secStart + (this->secEnd - this->secStart) * per);

		double loopStart = std::min(this->mouseDownSecTemp, timeSec);
		double loopEnd = std::max(this->mouseDownSecTemp, timeSec);
		quickAPI::setPlayLoop(loopStart, loopEnd);
	}
}

void SeqTimeRuler::mouseUp(const juce::MouseEvent& event) {
	/** Play Position Changed */
	/*if (event.mods.isLeftButtonDown()) {
		double per = event.position.getX() / (double)this->getWidth();
		double timeSec = this->secStart + (this->secEnd - this->secStart) * per;

		quickAPI::setPlayPosition(timeSec);
	}*/

	/** Loop Changed */
	if (event.mods.isRightButtonDown()) {
		if (!(event.mouseWasDraggedSinceMouseDown())) {
			quickAPI::setPlayLoop(0, 0);
		}
	}
}

std::tuple<double, double> SeqTimeRuler::getViewArea(
	double pos, double itemSize) const {
	double secStart = pos / itemSize;
	double secLength = this->getWidth() / itemSize;
	return { secStart, secStart + secLength };
}

const std::tuple<juce::Array<SeqTimeRuler::LineItem>, double>
SeqTimeRuler::createRulerLine(double pos, double itemSize) const {
	/** Get View Area */
	auto [secStart, secEnd] = this->getViewArea(pos, itemSize);
	double width = this->getWidth();

	/** Get Each Line */
	juce::Array<LineItem> result;
	double minInterval = DBL_MAX;

	double currentTime = secStart;
	int currentTempIndex = -1;
	
	double tempTimeSec = 0, tempTimeQuarter = 0, tempTimeBar = 0;
	double tempSecPerQuarter = 0.5;
	int tempNumerator = 4, tempDenominator = 4;

	while (true/*currentTime < secEnd*/) {
		/** Get Current Tempo */
		int tempIndex = quickAPI::getTempoTempIndexBySec(currentTime);
		if (tempIndex != currentTempIndex) {
			std::tie(tempTimeSec, tempTimeQuarter, tempTimeBar,
				tempSecPerQuarter, tempNumerator, tempDenominator)
				= quickAPI::getTempoData(tempIndex);
			currentTempIndex = tempIndex;
		}

		/** Get Current Bar */
		double secSinceTemp = currentTime - tempTimeSec;
		double quarterSinceTemp = secSinceTemp / tempSecPerQuarter;
		double quarterPerBar = ((4.0 / tempDenominator) * tempNumerator);
		double barSinceTemp = quarterSinceTemp / quarterPerBar;
		double currentTimeBar = tempTimeBar + barSinceTemp;

		double currentBar = std::floor(currentTimeBar);
		double currentBarTimeQuarterSinceTemp = (currentBar - tempTimeBar) * quarterPerBar;
		double currentBarTimeSec = tempTimeSec + currentBarTimeQuarterSinceTemp * tempSecPerQuarter;
		if (juce::approximatelyEqual(currentBarTimeSec, currentTime)) {
			currentBarTimeSec = currentTime;
		}

		/** Check End */
		if (currentBarTimeSec >= secEnd) { break; }

		/** Lines */
		double secPerLine = tempSecPerQuarter * (4.0 / tempDenominator);
		double lineInterval = secPerLine / (secEnd - secStart) * width;
		minInterval = std::min(minInterval, lineInterval);
		for (int i = 0; i < tempNumerator; i++) {
			double lineTime = currentBarTimeSec + i * secPerLine;
			double pos = (lineTime - secStart) / (secEnd - secStart) * width;
			result.add({ pos, i == 0, (int)currentBar });
		}

		/** Increase Time */
		currentTime += (secPerLine * tempNumerator);
	}

	/** Result */
	return { result, minInterval };
}

double SeqTimeRuler::limitTimeSec(double timeSec) {
	double level = Tools::getInstance()->getAdsorb();
	return quickAPI::limitTimeSec(timeSec, level);
}

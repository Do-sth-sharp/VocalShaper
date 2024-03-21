#include "SeqTimeRuler.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

SeqTimeRuler::SeqTimeRuler() {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forTimeRuler());
}

void SeqTimeRuler::updateHPos(double pos, double itemSize) {
	/** Set Pos */
	this->pos = pos;
	this->itemSize = itemSize;

	/** Update Line Temp */
	std::tie(this->lineTemp, this->minInterval) = this->createRulerLine(pos, itemSize);

	/** Repaint */
	this->repaint();
}

void SeqTimeRuler::resized() {
	/** Update Line Temp */
	std::tie(this->lineTemp, this->minInterval) = this->createRulerLine(pos, itemSize);
}

void SeqTimeRuler::paint(juce::Graphics& g) {
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

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
	/** TODO Update Line Temp */

	/** Repaint */
	this->repaint();
}

void SeqTimeRuler::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float outlineThickness = screenSize.getHeight() * 0.0015;

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour outlineColor = laf.findColour(
		juce::Label::ColourIds::outlineWhenEditingColourId);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Outline */
	auto outlineRect = this->getLocalBounds();
	g.setColour(outlineColor);
	g.drawRect(outlineRect, outlineThickness);
}

std::tuple<double, double> SeqTimeRuler::getViewArea(
	double pos, double itemSize) const {
	double secStart = pos / itemSize;
	double secLength = this->getWidth() / itemSize;
	return { secStart, secStart + secLength };
}

const juce::Array<SeqTimeRuler::LineItem> SeqTimeRuler::createRulerLine(
	double pos, double itemSize) const {
	/** Get View Area */
	auto [secStart, secEnd] = this->getViewArea(pos, itemSize);
	double width = this->getWidth();

	/** Get Each Line */
	juce::Array<LineItem> result;

	double currentTime = secStart;
	int currentTempIndex = -1;
	
	double tempTimeSec = 0, tempTimeQuarter = 0, tempTimeBar = 0;
	double tempSecPerQuarter = 0.5;
	int tempNumerator = 4, tempDenominator = 4;

	while (currentTime < secEnd) {
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

		/** Lines */
		double secPerLine = tempSecPerQuarter * (4.0 / tempDenominator);
		for (int i = 0; i < tempNumerator; i++) {
			double lineTime = currentBarTimeSec + i * secPerLine;
			double pos = (lineTime - secStart) / (secEnd - secStart) * width;
			result.add({ pos, i == 0 });
		}

		/** Increase Time */
		currentTime += (secPerLine * tempNumerator);
	}

	/** Result */
	return result;
}

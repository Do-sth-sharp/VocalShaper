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

	/** Get Each Line */
	const juce::Array<LineItem> result;

	double currentTime = secStart;
	int currentTempIndex = -1;
	
	double tempTime = 0;
	double tempSecPerQuarter = 0.5;
	int tempNumerator = 4, tempDenominator = 4;

	while (currentTime < secEnd) {
		/** Get Current Tempo */
		int tempIndex = quickAPI::getTempoTempIndexBySec(currentTime);
		if (tempIndex != currentTempIndex) {
			std::tie(tempTime, tempSecPerQuarter, tempNumerator, tempDenominator)
				= quickAPI::getTempoData(tempIndex);
			currentTempIndex = tempIndex;
		}

		/** TODO Get Next Line */

		/** TODO Increase Time */
		currentTime += 0;
	}

	/** Result */
	return result;
}

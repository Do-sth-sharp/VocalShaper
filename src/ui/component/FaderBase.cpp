#include "FaderBase.h"
#include "../Utils.h"

FaderBase::FaderBase(double defaultValue, const juce::Array<double>& hotDBValues,
	double minValue, double maxValue, int numberOfDecimalPlaces)
	: defaultValue(defaultValue),
	minValue(minValue), maxValue(maxValue),
	numberOfDecimalPlaces(numberOfDecimalPlaces),
	hotDBValues(hotDBValues),
	hotDBStrs(FaderBase::createDBStr(hotDBValues, numberOfDecimalPlaces)),
	hotLinearValues(FaderBase::createLinearValue(hotDBValues)),
	hotDisplayPercents(FaderBase::createDisplayPercent(hotLinearValues, minValue, maxValue)) {
	/** TODO */
}

void FaderBase::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.00175;
	int paddingHeight = screenSize.getHeight() * 0.002;

	float lineThickness = screenSize.getHeight() * 0.0015;
	float blockHeight = screenSize.getHeight() * 0.005;
	float blockWidth = screenSize.getWidth() * 0.005;

	int textShownWidth = screenSize.getWidth() * 0.0125;
	int textWidth = screenSize.getWidth() * 0.004;
	int textHeight = screenSize.getHeight() * 0.005;
	float textFontHeight = screenSize.getHeight() * 0.004;

	float lineSplitWidth = screenSize.getWidth() * 0.00175;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour lineColor = laf.findColour(
		juce::Label::ColourIds::outlineWhenEditingColourId);
	juce::Colour textColor = laf.findColour(
		juce::Label::ColourIds::textColourId);

	/** Font */
	juce::Font textFont(textFontHeight);

	/** TODO */
}

const juce::StringArray FaderBase::createDBStr(
	const juce::Array<double>& values, int numberOfDecimalPlaces) {
	juce::StringArray result;
	for (auto i : values) {
		result.add(juce::String{ i, numberOfDecimalPlaces });
	}
	return result;
}

const juce::Array<double> FaderBase::createLinearValue(
	const juce::Array<double>& values) {
	juce::Array<double> result;
	for (auto i : values) {
		result.add(FaderBase::convertDBToLinear(i));
	}
	return result;
}

const juce::Array<double> FaderBase::createDisplayPercent(
	const juce::Array<double>& values, double minValue, double maxValue) {
	juce::Array<double> result;
	for (auto i : values) {
		result.add(FaderBase::convertLinearToDisplayPercent(
			i, minValue, maxValue));
	}
	return result;
}

double FaderBase::convertDBToLinear(double value) {
	return std::pow(10.0, value / 20);
}

double FaderBase::convertLinearToDisplayPercent(
	double value, double minValue, double maxValue) {
	double per = (value - minValue) / (maxValue - minValue);
	return std::pow(per, 0.25);
}

double FaderBase::convertDisplayPercentToLinear(
	double value, double minValue, double maxValue) {
	double per = std::pow(value, 4.0);
	return minValue + per * (maxValue - minValue);
}

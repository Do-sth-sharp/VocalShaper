#pragma once

#include <JuceHeader.h>

class FaderBase
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	FaderBase() = delete;
	FaderBase(double defaultValue, const juce::Array<double>& hotDBValues,
		double minValue, double maxValue, int numberOfDecimalPlaces = 0);
	virtual ~FaderBase() = default;

	void paint(juce::Graphics& g) override;

private:
	const double defaultValue;
	const double minValue, maxValue;
	const int numberOfDecimalPlaces;
	double value = 0;

	const juce::Array<double> hotDBValues;
	const juce::StringArray hotDBStrs;
	const juce::Array<double> hotLinearValues;
	const juce::Array<double> hotDisplayPercents;

	static const juce::StringArray createDBStr(
		const juce::Array<double>& values, int numberOfDecimalPlaces);
	static const juce::Array<double> createLinearValue(
		const juce::Array<double>& values);
	static const juce::Array<double> createDisplayPercent(
		const juce::Array<double>& values, double minValue, double maxValue);

	static double convertDBToLinear(double value);
	static double convertLinearToDisplayPercent(
		double value, double minValue, double maxValue);
	static double convertDisplayPercentToLinear(
		double value, double minValue, double maxValue);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FaderBase)
};

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

	void setValue(double value, bool sendChange = false);
	double getValue() const;

	void paint(juce::Graphics& g) override;

	void mouseDown(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;

public:
	std::function<void(double)> onChange;

private:
	const double defaultValue;
	const double minValue, maxValue;
	const int numberOfDecimalPlaces;
	double value = 0;
	double valuePercent = 0;
	juce::String valueStr;
	bool pressed = false;

	const juce::Array<double> hotDBValues;
	const juce::StringArray hotDBStrs;
	const juce::Array<double> hotLinearValues;
	const juce::Array<double> hotDisplayPercents;

	double limitValue(double value) const;

	static const juce::StringArray createDBStr(
		const juce::Array<double>& values, int numberOfDecimalPlaces);
	static const juce::Array<double> createLinearValue(
		const juce::Array<double>& values);
	static const juce::Array<double> createDisplayPercent(
		const juce::Array<double>& values, double minValue, double maxValue);

	static double convertDBToLinear(double value);
	static double convertLinearToDB(double value);
	static double convertLinearToDisplayPercent(
		double value, double minValue, double maxValue);
	static double convertDisplayPercentToLinear(
		double value, double minValue, double maxValue);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FaderBase)
};

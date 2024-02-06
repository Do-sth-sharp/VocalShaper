#pragma once

#include <JuceHeader.h>

class KnobBase
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	KnobBase() = delete;
	KnobBase(const juce::String& name, double defaultValue,
		double minValue, double maxValue, int numberOfDecimalPlaces = 2);
	virtual ~KnobBase() = default;

	void setValue(double value, bool sendChange = false);
	double getValue() const;

	void paint(juce::Graphics& g) override;

public:
	std::function<void(double)> onChange;

private:
	const juce::String name;
	const double minValue, maxValue;
	const int numberOfDecimalPlaces;
	double value = 0;
	juce::String valueStr;

	double limitValue(double value) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobBase)
};

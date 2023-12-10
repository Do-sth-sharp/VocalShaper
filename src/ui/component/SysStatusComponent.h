#pragma once

#include <JuceHeader.h>

class SysStatusComponent final : public juce::AnimatedAppComponent {
public:
	SysStatusComponent();

	void update() override;
	void paint(juce::Graphics& g) override;
	void resized() override;

private:
	juce::String curveName;
	std::vector<double> curveData;
	int curveHead = 0;

	std::array<double, 3> watchData = { 0, 0 ,0 };
	std::array<juce::String, 3> watchName;

	void clearCurve(int size);
	void addCurve(double data);
	double getCurve(int index);

	double getData(const juce::String& name) const;
	std::tuple<double, double> getRange() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SysStatusComponent)
};

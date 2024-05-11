#pragma once

#include <JuceHeader.h>

class SysStatusComponent final : public juce::AnimatedAppComponent,
	public juce::SettableTooltipClient {
public:
	SysStatusComponent();

	void update() override;
	void paint(juce::Graphics& g) override;
	void resized() override;

	void mouseUp(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;

private:
	juce::String curveName;
	std::vector<double> curveData;
	int curveHead = 0;

	std::array<double, 3> watchData = { 0, 0 ,0 };
	std::array<juce::String, 3> watchName;

	std::map<juce::String, juce::String> nameTrans;

	void clearCurve(int size);
	void addCurve(double data);
	double getCurve(int index);

	double getData(const juce::String& name) const;
	std::tuple<double, double> getRange() const;
	bool getAlert(const juce::String& name, double value) const;
	juce::String getValueText(const juce::String& name, double value) const;

	void showCurveMenu();
	void showWatchMenu(int index);
	juce::PopupMenu createMenu(const juce::String& currentName,
		int currentPoints, bool isCurve = false);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SysStatusComponent)
};

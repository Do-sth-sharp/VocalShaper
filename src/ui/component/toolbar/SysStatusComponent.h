#pragma once

#include <JuceHeader.h>

class SysStatusComponent final : public juce::AnimatedAppComponent,
	public juce::SettableTooltipClient {
public:
	SysStatusComponent();

	void refresh();
	void update() override;
	void paint(juce::Graphics& g) override;
	void resized() override;

	void mouseUp(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;

private:
	enum WatchType {
		CPU, Audio, Memory, ProcessMem,
		TotalNum
	};

	WatchType currentCurve = WatchType::Audio;
	std::vector<double> curveData;
	int curveHead = 0;

	std::array<double, WatchType::TotalNum> watchData = { 0, 0, 0, 0 };
	std::array<juce::String, WatchType::TotalNum> watchName;
	std::array<WatchType, 3> currentWatchList;

	void clearCurve(int size);
	void addCurve(double data);
	double getCurve(int index);

	std::tuple<double, double> getRange(WatchType type) const;
	bool getAlert(WatchType type, double value) const;
	juce::String getValueText(WatchType type, double value) const;

	void showCurveMenu();
	void showWatchMenu(int index);
	juce::PopupMenu createMenu(WatchType type, bool isCurve = false);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SysStatusComponent)
};

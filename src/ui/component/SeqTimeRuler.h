#pragma once

#include <JuceHeader.h>
#include "../misc/LevelMeterHub.h"

class SeqTimeRuler final
	: public juce::Component,
	public LevelMeterHub::Target {
public:
	using ScrollFunc = std::function<void(double)>;
	using ScaleFunc = std::function<void(double, double, double)>;
	SeqTimeRuler(const ScrollFunc& scrollFunc,
		const ScaleFunc& scaleFunc);

	void updateBlock(int track, int index);
	void updateHPos(double pos, double itemSize);
	void updateRulerTemp();
	void updateLevelMeter() override;

	void resized() override;
	void paint(juce::Graphics& g) override;

	void mouseDown(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

	/** Place, IsBar, barId */
	using LineItem = std::tuple<double, bool, int>;
	std::tuple<double, double> getViewArea(double pos, double itemSize) const;

private:
	const ScrollFunc scrollFunc;
	const ScaleFunc scaleFunc;

	double pos = 0, itemSize = 0;
	double secStart = 0, secEnd = 0;
	juce::Array<LineItem> lineTemp;
	double minInterval = 0;
	std::unique_ptr<juce::Image> rulerTemp = nullptr;

	double playPosSec = 0;
	double totalLengthSec = 0;

	/** Line List, Min Interval */
	const std::tuple<juce::Array<LineItem>, double> createRulerLine(double pos, double itemSize) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTimeRuler)
};

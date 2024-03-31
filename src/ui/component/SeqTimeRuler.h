#pragma once

#include <JuceHeader.h>
#include "../misc/LevelMeterHub.h"

class SeqTimeRuler final
	: public juce::Component,
	public LevelMeterHub::Target {
public:
	using ScrollFunc = std::function<void(double)>;
	using ScaleFunc = std::function<void(double, double, double)>;
	using WheelFunc = std::function<void(float, bool)>;
	using WheelAltFunc = std::function<void(double, double, float, bool)>;
	SeqTimeRuler(const ScrollFunc& scrollFunc,
		const ScaleFunc& scaleFunc,
		const WheelFunc& wheelFunc,
		const WheelAltFunc& wheelAltFunc);

	void updateTempoLabel();
	void updateBlock(int track, int index);
	void updateHPos(double pos, double itemSize);
	void updateRulerTemp();
	void updateLevelMeter() override;

	void resized() override;
	void paint(juce::Graphics& g) override;

	void mouseDown(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseWheelMove(const juce::MouseEvent& event,
		const juce::MouseWheelDetails& wheel);

	/** Place, IsBar, barId */
	using LineItem = std::tuple<double, bool, int>;
	std::tuple<double, double> getViewArea(double pos, double itemSize) const;

private:
	const ScrollFunc scrollFunc;
	const ScaleFunc scaleFunc;
	const WheelFunc wheelFunc;
	const WheelAltFunc wheelAltFunc;

	double pos = 0, itemSize = 0;
	double secStart = 0, secEnd = 0;
	juce::Array<LineItem> lineTemp;
	double minInterval = 0;
	std::unique_ptr<juce::Image> rulerTemp = nullptr;
	/** timeInSec, tempo, numerator, denominator, isTempo */
	using TempoLabelData = std::tuple<double, double, int, int, bool>;
	juce::Array<TempoLabelData> tempoTemp;

	double playPosSec = 0;
	double totalLengthSec = 0;
	double loopStartSec = 0, loopEndSec = 0;

	double mouseDownSecTemp = 0;

	/** Line List, Min Interval */
	const std::tuple<juce::Array<LineItem>, double> createRulerLine(double pos, double itemSize) const;

	double limitTimeSec(double timeSec);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTimeRuler)
};

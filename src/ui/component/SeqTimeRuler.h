#pragma once

#include <JuceHeader.h>

class SeqTimeRuler final : public juce::Component {
public:
	SeqTimeRuler();

	void updateHPos(double pos, double itemSize);
	void updateRulerTemp();

	void resized() override;
	void paint(juce::Graphics& g) override;

	/** Place, IsBar, barId */
	using LineItem = std::tuple<double, bool, int>;
	std::tuple<double, double> getViewArea(double pos, double itemSize) const;

private:
	double pos = 0, itemSize = 0;
	juce::Array<LineItem> lineTemp;
	double minInterval = 0;
	std::unique_ptr<juce::Image> rulerTemp = nullptr;

	/** Line List, Min Interval */
	const std::tuple<juce::Array<LineItem>, double> createRulerLine(double pos, double itemSize) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTimeRuler)
};

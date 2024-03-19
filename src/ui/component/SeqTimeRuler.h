#pragma once

#include <JuceHeader.h>

class SeqTimeRuler final : public juce::Component {
public:
	SeqTimeRuler();

	void updateHPos(double pos, double itemSize);

	void paint(juce::Graphics& g) override;

	/** Place, IsBar */
	using LineItem = std::tuple<double, bool>;
	std::tuple<double, double> getViewArea(double pos, double itemSize) const;

private:
	const juce::Array<LineItem> createRulerLine(double pos, double itemSize) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTimeRuler)
};

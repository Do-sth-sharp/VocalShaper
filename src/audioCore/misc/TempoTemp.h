#pragma once

#include <JuceHeader.h>

class TempoTemp final {
public:
	TempoTemp() = default;

	void update(juce::MidiMessageSequence& tempoMessages);

private:
	/** xInQuarter, xInBar, beat, base */
	using BeatTempItem = std::tuple<double, double, int, int>;
	/** xs, xe, ts, te, T */
	using TempoTempItem = std::tuple<double, double, double, double, double>;

	juce::Array<BeatTempItem> beatTemp;
	juce::Array<TempoTempItem> tempoTemp;
	int lastBeatIndex = -1, lastTempoIndex = -1;

	void updateTempo(juce::MidiMessageSequence& tempoMessages);
	void updateBeat(juce::MidiMessageSequence& beatMessages);

	static double tFuncBeat(double x, double xs, double T, double ts);
	static double xFuncBeat(double t, double ts, double T, double xs);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoTemp)
};

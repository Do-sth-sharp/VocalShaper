#pragma once

#include <JuceHeader.h>

class TempoTemp final {
public:
	TempoTemp() = default;

	void update(juce::MidiMessageSequence& tempoMessages);
	int selectBySec(double time) const;

private:
	/** timeInSec, timeInQuarter, timeInBar, secPerQuarter, quarterPerBar */
	using TempoTempItem = std::tuple<double, double, double, double, double>;

	juce::Array<TempoTempItem> temp;
	mutable int lastIndex = -1;

	enum class CompareResult {
		EQ, GTR, LSS
	};
	template<typename Func, typename T>
	int search(int low, int high, T value, Func func) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoTemp)
};

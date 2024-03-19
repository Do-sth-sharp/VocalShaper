#pragma once

#include <JuceHeader.h>

class TempoTemp final {
public:
	TempoTemp();

	void update(const juce::MidiMessageSequence& tempoMessages);
	int selectBySec(double time) const;
	int selectByTick(double timeTick, short timeFormat) const;
	int selectByQuarter(double timeQuarter) const;
	int selectByBar(double timeBar) const;

	double secToQuarter(double timeSec, int tempIndex) const;
	double quarterToSec(double timeQuarter, int tempIndex) const;
	double secToTick(double timeSec, int tempIndex, short timeFormat) const;
	double tickToSec(double timeTick, int tempIndex, short timeFormat) const;
	double quarterToBar(double timeQuarter, int tempIndex) const;
	double barToQuarter(double timeBar, int tempIndex) const;
	double secToBar(double timeSec, int tempIndex) const;
	double barToSec(double timeBar, int tempIndex) const;

	double getSecPerQuarter(int tempIndex) const;
	double getQuarterPerBar(int tempIndex) const;
	/** numerator, denominator */
	std::tuple<int, int> getTimeSignature(int tempIndex) const;

private:
	/** timeInSec, timeInQuarter, timeInBar, secPerQuarter, quarterPerBar, numerator, denominator */
	using TempoTempItem = std::tuple<double, double, double, double, double, int, int>;

	juce::Array<TempoTempItem> temp;
	mutable int lastIndex = -1;

	enum class CompareResult {
		EQ, GTR, LSS
	};
	template<typename Func, typename T>
	int search(int low, int high, T value, Func func) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoTemp)
};

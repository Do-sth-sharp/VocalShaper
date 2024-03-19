#include "TempoTemp.h"

TempoTemp::TempoTemp() {
	this->update(juce::MidiMessageSequence{});
}

void TempoTemp::update(
	const juce::MidiMessageSequence& tempoMessages) {
	/** Clear Temp */
	this->temp.clear();
	this->lastIndex = -1;

	/** Corrected Time */
	double correctedQuarter = 0;
	double lastValidQuarter = 0;

	/** Event Time In Bar */
	double lastValidBar = 0;

	/** Event Time In Seconds */
	double eventTime = 0;
	double lastTime = 0;

	/** State Temp */
	double secsPerQuarter = 0.5, quarterPerBar = 4.0;
	int numeratorLastValid = 4, denominatorLastValid = 4;

	/** State Wait For Valid Temp */
	double tempValidSec = 0;
	double tempValidQuarter = 0;
	double tempValidBar = 0;
	double secPerQuarterWaitForValid = 0.5;
	double quarterPerBarWaitForValid = 4.0;
	int numeratorForValid = 4;
	int denominatorForValid = 4;

	/** Add First Event */
	this->temp.add({ tempValidSec, tempValidQuarter, tempValidBar,
		secPerQuarterWaitForValid, quarterPerBarWaitForValid,
		numeratorForValid, denominatorForValid });

	/** Build Temporary */
	int numEvents = tempoMessages.getNumEvents();
	for (int i = 0; i < numEvents; ++i) {
		auto& m = tempoMessages.getEventPointer(i)->message;
		eventTime = m.getTimeStamp();

		/** Temp Valid */
		if (correctedQuarter + (eventTime - lastTime) / secsPerQuarter > tempValidQuarter) {
			if (std::get<0>(this->temp.getLast()) < tempValidQuarter) {
				this->temp.add({ tempValidSec, tempValidQuarter, tempValidBar,
					secPerQuarterWaitForValid, quarterPerBarWaitForValid,
					numeratorForValid, denominatorForValid });
			}
			else {
				this->temp.getReference(this->temp.size() - 1)
					= std::make_tuple(tempValidSec, tempValidQuarter, tempValidBar,
						secPerQuarterWaitForValid, quarterPerBarWaitForValid,
						numeratorForValid, denominatorForValid);
			}

			lastValidQuarter = tempValidQuarter;
			lastValidBar = tempValidBar;
			secsPerQuarter = secPerQuarterWaitForValid;
			quarterPerBar = quarterPerBarWaitForValid;
			numeratorLastValid = numeratorForValid;
			denominatorLastValid = denominatorForValid;
		}

		/** Get Time */
		correctedQuarter += (eventTime - lastTime) / secsPerQuarter;
		lastTime = eventTime;
		tempValidSec = eventTime;

		/** Tempo Changed */
		if (m.isTempoMetaEvent()) {
			secPerQuarterWaitForValid = m.getTempoSecondsPerQuarterNote();

			/** Check Delay Valid */
			double quarterDistanceFromLastValid = correctedQuarter - lastValidQuarter;
			double barCountFromLastValid = quarterDistanceFromLastValid / quarterPerBar;
			double barDistance = barCountFromLastValid;

			/** Set Wait For Valid Temp */
			tempValidQuarter = lastValidQuarter + barDistance * quarterPerBar;
		}

		/** Time Signature Changed */
		if (m.isTimeSignatureMetaEvent()) {
			int numerator = 4, denominator = 4;
			m.getTimeSignatureInfo(numerator, denominator);

			/** Check Delay Valid */
			double quarterDistanceFromLastValid = correctedQuarter - lastValidQuarter;
			double barCountFromLastValid = quarterDistanceFromLastValid / quarterPerBar;
			double barDistance = std::floor(barCountFromLastValid);
			if (!juce::approximatelyEqual(barDistance, barCountFromLastValid)) {
				barDistance++;
			}

			/** Set Wait For Valid Temp */
			numeratorForValid = numerator;
			denominatorForValid = denominator;
			quarterPerBarWaitForValid = numerator * (4.0 / denominator);
			tempValidBar = lastValidBar + barDistance;
			tempValidQuarter = lastValidQuarter + barDistance * quarterPerBar;
		}

		/** Skip Events With Same Time */
		while (i + 1 < numEvents) {
			auto& m2 = tempoMessages.getEventPointer(i + 1)->message;

			if (!juce::approximatelyEqual(m2.getTimeStamp(), eventTime))
				break;

			if (m2.isTempoMetaEvent()) {
				secPerQuarterWaitForValid = m2.getTempoSecondsPerQuarterNote();

				/** Check Delay Valid */
				double quarterDistanceFromLastValid = correctedQuarter - lastValidQuarter;
				double barCountFromLastValid = quarterDistanceFromLastValid / quarterPerBar;
				double barDistance = barCountFromLastValid;

				/** Set Wait For Valid Temp */
				tempValidBar = lastValidBar + barDistance;
				tempValidQuarter = lastValidQuarter + barDistance * quarterPerBar;
			}

			if (m2.isTimeSignatureMetaEvent()) {
				int numerator = 4, denominator = 4;
				m2.getTimeSignatureInfo(numerator, denominator);

				/** Check Delay Valid */
				double quarterDistanceFromLastValid = correctedQuarter - lastValidQuarter;
				double barCountFromLastValid = quarterDistanceFromLastValid / quarterPerBar;
				double barDistance = std::floor(barCountFromLastValid);
				if (!juce::approximatelyEqual(barDistance, barCountFromLastValid)) {
					barDistance++;
				}

				/** Set Wait For Valid Temp */
				numeratorForValid = numerator;
				denominatorForValid = denominator;
				quarterPerBarWaitForValid = numerator * (4.0 / denominator);
				tempValidBar = lastValidBar + barDistance;
				tempValidQuarter = lastValidQuarter + barDistance * quarterPerBar;
			}

			++i;
		}
	}

	/** Temp Valid */
	if (correctedQuarter + (eventTime - lastTime) / secsPerQuarter > tempValidQuarter) {
		if (std::get<0>(this->temp.getLast()) < tempValidQuarter) {
			this->temp.add({ tempValidSec, tempValidQuarter, tempValidBar,
				secPerQuarterWaitForValid, quarterPerBarWaitForValid,
				numeratorForValid, denominatorForValid });
		}
		else {
			this->temp.getReference(this->temp.size() - 1)
				= std::make_tuple(tempValidSec, tempValidQuarter, tempValidBar,
					secPerQuarterWaitForValid, quarterPerBarWaitForValid,
					numeratorForValid, denominatorForValid);
		}

		lastValidQuarter = tempValidQuarter;
		lastValidBar = tempValidBar;
		secsPerQuarter = secPerQuarterWaitForValid;
		quarterPerBar = quarterPerBarWaitForValid;
		numeratorLastValid = numeratorForValid;
		denominatorLastValid = denominatorForValid;
	}
}

int TempoTemp::selectBySec(double time) const {
	if (this->lastIndex >= 0 && this->lastIndex < this->temp.size()) {
		/** Get Temp */
		auto& [timeInSec, timeInQuarter, timeInBar,
			secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(this->lastIndex);

		/** Hit Temp */
		if (this->lastIndex == this->temp.size() - 1) {
			if (time >= timeInSec) {
				return this->lastIndex;
			}
		}
		else {
			auto& [timeInSecNext, timeInQuarterNext, timeInBarNext,
				secPerQuarterNext, quarterPerBarNext, numeratorNext, denominatorNext] = this->temp.getReference(this->lastIndex + 1);
			if ((time >= timeInSec) && (time < timeInSecNext)) {
				return this->lastIndex;
			}
		}
	}
	
	/** Get Next Temp */
	if (this->lastIndex >= -1 && this->lastIndex < this->temp.size() - 1) {
		/** Get Next */
		auto& [timeInSec, timeInQuarter, timeInBar,
			secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(this->lastIndex + 1);
		
		/** Check Hit */
		if (this->lastIndex < this->temp.size() - 2) {
			auto& [timeInSecNext, timeInQuarterNext, timeInBarNext,
				secPerQuarterNext, quarterPerBarNext, numeratorNext, denominatorNext] = this->temp.getReference(this->lastIndex + 2);
			if ((time >= timeInSec) && (time < timeInSecNext)) {
				return ++(this->lastIndex);
			}
		}
		else {
			if (time >= timeInSec) {
				return ++(this->lastIndex);
			}
		}
	}

	/** Compare Function */
	auto cFunc = [](double x, const TempoTempItem& curr, const TempoTempItem& next)->CompareResult {
		if (x >= std::get<0>(curr) && x < std::get<0>(next)) {
			return CompareResult::EQ;
		}
		else if (x >= std::get<0>(next)) {
			return CompareResult::GTR;
		}
		else {
			return CompareResult::LSS;
		}
		};

	/** Binary Search */
	return this->lastIndex = this->search(0, this->temp.size() - 1, time, cFunc);
}

int TempoTemp::selectByTick(double timeTick, short timeFormat) const {
	/** Ticks Per Sec */
	if (timeFormat < 0) {
		double timeSec = timeTick / (-(timeFormat >> 8) * (timeFormat & 0xff));
		return this->selectBySec(timeSec);
	}

	/** Ticks Per Quarter */
	double timeQuarter = timeTick / timeFormat;
	return this->selectByQuarter(timeQuarter);
}

int TempoTemp::selectByQuarter(double timeQuarter) const {
	if (this->lastIndex >= 0 && this->lastIndex < this->temp.size()) {
		/** Get Temp */
		auto& [timeInSec, timeInQuarter, timeInBar,
			secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(this->lastIndex);

		/** Hit Temp */
		if (this->lastIndex == this->temp.size() - 1) {
			if (timeQuarter >= timeInQuarter) {
				return this->lastIndex;
			}
		}
		else {
			auto& [timeInSecNext, timeInQuarterNext, timeInBarNext,
				secPerQuarterNext, quarterPerBarNext, numeratorNext, denominatorNext] = this->temp.getReference(this->lastIndex + 1);
			if ((timeQuarter >= timeInQuarter) && (timeQuarter < timeInQuarterNext)) {
				return this->lastIndex;
			}
		}
	}

	/** Get Next Temp */
	if (this->lastIndex >= -1 && this->lastIndex < this->temp.size() - 1) {
		/** Get Next */
		auto& [timeInSec, timeInQuarter, timeInBar,
			secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(this->lastIndex + 1);

		/** Check Hit */
		if (this->lastIndex < this->temp.size() - 2) {
			auto& [timeInSecNext, timeInQuarterNext, timeInBarNext,
				secPerQuarterNext, quarterPerBarNext, numeratorNext, denominatorNext] = this->temp.getReference(this->lastIndex + 2);
			if ((timeQuarter >= timeInQuarter) && (timeQuarter < timeInQuarterNext)) {
				return ++(this->lastIndex);
			}
		}
		else {
			if (timeQuarter >= timeInQuarter) {
				return ++(this->lastIndex);
			}
		}
	}

	/** Compare Function */
	auto cFunc = [](double x, const TempoTempItem& curr, const TempoTempItem& next)->CompareResult {
		if (x >= std::get<1>(curr) && x < std::get<1>(next)) {
			return CompareResult::EQ;
		}
		else if (x >= std::get<1>(next)) {
			return CompareResult::GTR;
		}
		else {
			return CompareResult::LSS;
		}
		};

	/** Binary Search */
	return this->lastIndex = this->search(0, this->temp.size() - 1, timeQuarter, cFunc);
}

int TempoTemp::selectByBar(double timeBar) const {
	if (this->lastIndex >= 0 && this->lastIndex < this->temp.size()) {
		/** Get Temp */
		auto& [timeInSec, timeInQuarter, timeInBar,
			secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(this->lastIndex);

		/** Hit Temp */
		if (this->lastIndex == this->temp.size() - 1) {
			if (timeBar >= timeInBar) {
				return this->lastIndex;
			}
		}
		else {
			auto& [timeInSecNext, timeInQuarterNext, timeInBarNext,
				secPerQuarterNext, quarterPerBarNext, numeratorNext, denominatorNext] = this->temp.getReference(this->lastIndex + 1);
			if ((timeBar >= timeInBar) && (timeBar < timeInBarNext)) {
				return this->lastIndex;
			}
		}
	}

	/** Get Next Temp */
	if (this->lastIndex >= -1 && this->lastIndex < this->temp.size() - 1) {
		/** Get Next */
		auto& [timeInSec, timeInQuarter, timeInBar,
			secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(this->lastIndex + 1);

		/** Check Hit */
		if (this->lastIndex < this->temp.size() - 2) {
			auto& [timeInSecNext, timeInQuarterNext, timeInBarNext,
				secPerQuarterNext, quarterPerBarNext, numeratorNext, denominatorNext] = this->temp.getReference(this->lastIndex + 2);
			if ((timeBar >= timeInBar) && (timeBar < timeInBarNext)) {
				return ++(this->lastIndex);
			}
		}
		else {
			if (timeBar >= timeInBar) {
				return ++(this->lastIndex);
			}
		}
	}

	/** Compare Function */
	auto cFunc = [](double x, const TempoTempItem& curr, const TempoTempItem& next)->CompareResult {
		if (x >= std::get<2>(curr) && x < std::get<2>(next)) {
			return CompareResult::EQ;
		}
		else if (x >= std::get<1>(next)) {
			return CompareResult::GTR;
		}
		else {
			return CompareResult::LSS;
		}
		};

	/** Binary Search */
	return this->lastIndex = this->search(0, this->temp.size() - 1, timeBar, cFunc);
}

double TempoTemp::secToQuarter(double timeSec, int tempIndex) const {
	if (tempIndex < 0 || tempIndex >= this->temp.size()) { return 0; }
	const auto& [timeInSec, timeInQuarter, timeInBar,
		secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(tempIndex);
	return timeInQuarter + (timeSec - timeInSec) / secPerQuarter;
}

double TempoTemp::quarterToSec(double timeQuarter, int tempIndex) const {
	if (tempIndex < 0 || tempIndex >= this->temp.size()) { return 0; }
	const auto& [timeInSec, timeInQuarter, timeInBar,
		secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(tempIndex);
	return timeInSec + (timeQuarter - timeInQuarter) * secPerQuarter;
}

double TempoTemp::secToTick(double timeSec, int tempIndex, short timeFormat) const {
	/** Ticks Per Sec */
	if (timeFormat < 0) {
		return timeSec * (-(timeFormat >> 8) * (timeFormat & 0xff));
	}

	/** Ticks Per Quarter */
	double timeQuarter = this->secToQuarter(timeSec, tempIndex);
	return timeQuarter * timeFormat;
}

double TempoTemp::tickToSec(double timeTick, int tempIndex, short timeFormat) const {
	/** Ticks Per Sec */
	if (timeFormat < 0) {
		return timeTick / (-(timeFormat >> 8) * (timeFormat & 0xff));
	}

	/** Ticks Per Quarter */
	double timeQuarter = timeTick / timeFormat;
	return this->quarterToSec(timeQuarter, tempIndex);
}

double TempoTemp::quarterToBar(double timeQuarter, int tempIndex) const {
	if (tempIndex < 0 || tempIndex >= this->temp.size()) { return 0; }
	const auto& [timeInSec, timeInQuarter, timeInBar,
		secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(tempIndex);
	return timeInBar + (timeQuarter - timeInQuarter) / quarterPerBar;
}

double TempoTemp::barToQuarter(double timeBar, int tempIndex) const {
	if (tempIndex < 0 || tempIndex >= this->temp.size()) { return 0; }
	const auto& [timeInSec, timeInQuarter, timeInBar,
		secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(tempIndex);
	return timeInQuarter + (timeBar - timeInBar) * quarterPerBar;
}

double TempoTemp::secToBar(double timeSec, int tempIndex) const {
	double timeQuarter = this->secToQuarter(timeSec, tempIndex);
	return this->quarterToBar(timeQuarter, tempIndex);
}

double TempoTemp::barToSec(double timeBar, int tempIndex) const {
	double timeQuarter = this->barToQuarter(timeBar, tempIndex);
	return this->quarterToSec(timeQuarter, tempIndex);
}

double TempoTemp::getSecPerQuarter(int tempIndex) const {
	if (tempIndex < 0 || tempIndex >= this->temp.size()) { return 0.5; }
	const auto& [timeInSec, timeInQuarter, timeInBar,
		secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(tempIndex);
	return secPerQuarter;
}

double TempoTemp::getQuarterPerBar(int tempIndex) const {
	if (tempIndex < 0 || tempIndex >= this->temp.size()) { return 4.0; }
	const auto& [timeInSec, timeInQuarter, timeInBar,
		secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(tempIndex);
	return quarterPerBar;
}

std::tuple<int, int> TempoTemp::getTimeSignature(int tempIndex) const {
	if (tempIndex < 0 || tempIndex >= this->temp.size()) { return { 4, 4 }; }
	const auto& [timeInSec, timeInQuarter, timeInBar,
		secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(tempIndex);
	return { numerator, denominator };
}

template<typename Func, typename T>
int TempoTemp::search(int low, int high, T value, Func func) const {
	int mid = low + (high - low) / 2;
	if (func(value, this->temp.getReference(mid),
		this->temp.getReference(mid + 1)) == CompareResult::EQ) {
		return mid;
	}
	else if (func(value, this->temp.getReference(mid),
		this->temp.getReference(mid + 1)) == CompareResult::GTR) {
		return this->search(mid + 1, high, value, func);
	}
	else {
		return this->search(low, mid - 1, value, func);
	}
}

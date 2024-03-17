﻿#include "TempoTemp.h"

void TempoTemp::update(
	juce::MidiMessageSequence& tempoMessages) {
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

	/** State Wait For Valid Temp */
	double tempValidSec = 0;
	double tempValidQuarter = 0;
	double tempValidBar = 0;
	double secPerQuarterWaitForValid = 0.5;
	double quarterPerBarWaitForValid = 4.0;

	/** Add First Event */
	this->temp.add({ tempValidSec, tempValidQuarter, tempValidBar, secPerQuarterWaitForValid, quarterPerBarWaitForValid });

	/** Build Temporary */
	int numEvents = tempoMessages.getNumEvents();
	for (int i = 0; i < numEvents; ++i) {
		auto& m = tempoMessages.getEventPointer(i)->message;
		eventTime = m.getTimeStamp();

		/** Temp Valid */
		if (correctedQuarter + (eventTime - lastTime) / secsPerQuarter > tempValidQuarter) {
			if (std::get<0>(this->temp.getLast()) < tempValidQuarter) {
				this->temp.add({ tempValidSec, tempValidQuarter, tempValidBar, secPerQuarterWaitForValid, quarterPerBarWaitForValid });
			}
			else {
				this->temp.getReference(this->temp.size() - 1)
					= std::make_tuple(tempValidSec, tempValidQuarter, tempValidBar, secPerQuarterWaitForValid, quarterPerBarWaitForValid);
			}

			lastValidQuarter = tempValidQuarter;
			lastValidBar = tempValidBar;
			secsPerQuarter = secPerQuarterWaitForValid;
			quarterPerBar = quarterPerBarWaitForValid;
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
			this->temp.add({ tempValidSec, tempValidQuarter, tempValidBar, secPerQuarterWaitForValid, quarterPerBarWaitForValid });
		}
		else {
			this->temp.getReference(this->temp.size() - 1)
				= std::make_tuple(tempValidSec, tempValidQuarter, tempValidBar, secPerQuarterWaitForValid, quarterPerBarWaitForValid);
		}

		lastValidQuarter = tempValidQuarter;
		lastValidBar = tempValidBar;
		secsPerQuarter = secPerQuarterWaitForValid;
		quarterPerBar = quarterPerBarWaitForValid;
	}
}

int TempoTemp::selectBySec(double time) const {
	{
		/** Get Temp */
		auto& [timeInSec, timeInQuarter, timeInBar, secPerQuarter, quarterPerBar] = this->temp.getReference(this->lastIndex);

		/** Hit Temp */
		if (this->lastIndex == this->temp.size() - 1) {
			if (time >= timeInSec) {
				return this->lastIndex;
			}
		}
		else {
			auto& [timeInSecNext, timeInQuarterNext, timeInBarNext, secPerQuarterNext, quarterPerBarNext] = this->temp.getReference(this->lastIndex + 1);
			if ((time >= timeInSec) && (time < timeInSecNext)) {
				return this->lastIndex;
			}
		}
	}
	
	/** Get Next Temp */
	if (this->lastIndex < this->temp.size() - 1) {
		/** Get Next */
		auto& [timeInSec, timeInQuarter, timeInBar, secPerQuarter, quarterPerBar] = this->temp.getReference(this->lastIndex + 1);
		
		/** Check Hit */
		if (this->lastIndex < this->temp.size() - 2) {
			auto& [timeInSecNext, timeInQuarterNext, timeInBarNext, secPerQuarterNext, quarterPerBarNext] = this->temp.getReference(this->lastIndex + 2);
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

#include "TempoTemp.h"

TempoTemp::TempoTemp() {
	this->update(juce::MidiMessageSequence{});
}

void TempoTemp::update(
	const juce::MidiMessageSequence& tempoMessages) {
	/** Clear Temp */
	this->temp.clear();
	this->lastIndex = -1;

	/** Create Tempo Temp */
	/** timeInSec, timeInQuarter, secsPerQuarter */
	juce::Array<std::tuple<double, double, double>> tempoTemp;
	{
		/** Init Temp */
		double lastSec = 0, lastQuarter = 0, lastSPQ = 0.5;
		double thisSec = 0, thisQuarter = 0, thisSPQ = 0.5;

		/** Default Tempo */
		tempoTemp.add({ thisSec, thisQuarter, thisSPQ });

		/** Get Each Tempo Meta */
		int numEvents = tempoMessages.getNumEvents();
		for (int i = 0; i < numEvents; i++) {
			/** Get Event */
			auto& m = tempoMessages.getEventPointer(i)->message;
			double eventTime = m.getTimeStamp();

			/** Store Event */
			if (eventTime > thisSec) {
				if (std::get<0>(tempoTemp.getLast()) < thisSec) {
					tempoTemp.add({ thisSec, thisQuarter, thisSPQ });
				}
				else {
					tempoTemp.getReference(tempoTemp.size() - 1)
						= std::make_tuple(thisSec, thisQuarter, thisSPQ);
				}

				lastSec = thisSec;
				lastQuarter = thisQuarter;
				lastSPQ = thisSPQ;
			}

			/** Current Is Tempo Meta */
			if (m.isTempoMetaEvent()) {
				thisSPQ = m.getTempoSecondsPerQuarterNote();
				thisSec = eventTime;
				thisQuarter = lastQuarter + (thisSec - lastSec) / lastSPQ;
			}

			/** Skip Same Time Forward */
			while (i + 1 < numEvents) {
				auto& m2 = tempoMessages.getEventPointer(i + 1)->message;

				if (!juce::approximatelyEqual(m2.getTimeStamp(), eventTime))
					break;

				/** Next Is Tempo Meta */
				if (m2.isTempoMetaEvent()) {
					thisSPQ = m2.getTempoSecondsPerQuarterNote();
					thisSec = eventTime;
					thisQuarter = lastQuarter + (thisSec - lastSec) / lastSPQ;
				}

				i++;
			}
		}

		/** Write The Last Tempo */
		if (std::get<0>(tempoTemp.getLast()) < thisSec) {
			tempoTemp.add({ thisSec, thisQuarter, thisSPQ });
		}
		else {
			tempoTemp.getReference(tempoTemp.size() - 1)
				= std::make_tuple(thisSec, thisQuarter, thisSPQ);
		}
	}
	
	/** Tempo Temp Index */
	int tempoTempIndex = 0;

	/** Create Beat Temp */
	{
		/** Init Temp */
		double lastQuarter = 0, lastBar = 0;
		int lastNumerator = 4, lastDenominator = 4;
		double thisQuarter = 0, thisBar = 0;
		int thisNumerator = 4, thisDenominator = 4;

		/** Init Tempo Temp Data */
		double tempoSec = 0, tempoQuarter = 0, tempoSPQ = 0.5;
		std::tie(tempoSec, tempoQuarter, tempoSPQ) = tempoTemp.getReference(tempoTempIndex);

		/** Default Data */
		this->temp.add({ tempoSec + (thisQuarter - tempoQuarter) * tempoSPQ,
			thisQuarter, thisBar, tempoSPQ,
			thisNumerator * (4.0 / thisDenominator),
			thisNumerator, thisDenominator });

		/** Get Each Beat Meta */
		int numEvents = tempoMessages.getNumEvents();
		for (int i = 0; i < numEvents; i++) {
			/** Get Event */
			auto& m = tempoMessages.getEventPointer(i)->message;
			double eventTime = m.getTimeStamp();

			/** Store Event */
			{
				double thisSec = tempoSec + (thisQuarter - tempoQuarter) * tempoSPQ;
				if (eventTime > thisSec) {
					if (std::get<0>(this->temp.getLast()) < thisSec) {
						this->temp.add({ thisSec, thisQuarter, thisBar, tempoSPQ,
							thisNumerator * (4.0 / thisDenominator),
							thisNumerator, thisDenominator });
					}
					else {
						this->temp.getReference(this->temp.size() - 1)
							= std::make_tuple(thisSec, thisQuarter, thisBar, tempoSPQ,
								thisNumerator * (4.0 / thisDenominator),
								thisNumerator, thisDenominator);
					}

					lastQuarter = thisQuarter;
					lastBar = thisBar;
					lastNumerator = thisNumerator;
					lastDenominator = thisDenominator;
				}
			}

			/** Current Is Beat Meta */
			if (m.isTimeSignatureMetaEvent()) {
				m.getTimeSignatureInfo(thisNumerator, thisDenominator);

				/** Next Tempo Temp */
				while ((tempoTemp.size() > (tempoTempIndex + 1)) &&
					(eventTime > std::get<0>(tempoTemp.getReference(tempoTempIndex + 1)))) {
					std::tie(tempoSec, tempoQuarter, tempoSPQ) = tempoTemp.getReference(++tempoTempIndex);

					/** Add Tempo Temp */
					double tempoBar = lastBar + (tempoQuarter - lastQuarter) * (lastDenominator / 4.0) / lastNumerator;
					if (std::get<1>(this->temp.getLast()) < tempoQuarter) {
						this->temp.add({ tempoSec, tempoQuarter, tempoBar, tempoSPQ,
							lastNumerator * (4.0 / lastDenominator),
							lastNumerator, lastDenominator });
					}
					else {
						this->temp.getReference(this->temp.size() - 1)
							= std::make_tuple(tempoSec, tempoQuarter, tempoBar, tempoSPQ,
								lastNumerator * (4.0 / lastDenominator),
								lastNumerator, lastDenominator);
					}
				}

				/** Get Event Valid Quarter */
				double quarter = tempoQuarter + (eventTime - tempoSec) / tempoSPQ;
				double bar = lastBar + (quarter - lastQuarter) * (lastDenominator / 4.0) / lastNumerator;
				thisBar = std::floor(bar);
				if (!juce::approximatelyEqual(thisBar, bar)) {
					thisBar++;
				}
				thisQuarter = lastQuarter + (thisBar - lastBar) / (lastDenominator / 4.0) * lastNumerator;

				/** Next Tempo Temp */
				while ((tempoTemp.size() > (tempoTempIndex + 1)) &&
					(thisQuarter > std::get<1>(tempoTemp.getReference(tempoTempIndex + 1)))) {
					std::tie(tempoSec, tempoQuarter, tempoSPQ) = tempoTemp.getReference(++tempoTempIndex);

					/** Add Tempo Temp */
					double tempoBar = lastBar + (tempoQuarter - lastQuarter) * (lastDenominator / 4.0) / lastNumerator;
					if (std::get<1>(this->temp.getLast()) < tempoQuarter) {
						this->temp.add({ tempoSec, tempoQuarter, tempoBar, tempoSPQ,
							lastNumerator * (4.0 / lastDenominator),
							lastNumerator, lastDenominator });
					}
					else {
						this->temp.getReference(this->temp.size() - 1)
							= std::make_tuple(tempoSec, tempoQuarter, tempoBar, tempoSPQ,
								lastNumerator * (4.0 / lastDenominator),
								lastNumerator, lastDenominator);
					}
				}
			}

			/** Skip Same Time Forward */
			while (i + 1 < numEvents) {
				auto& m2 = tempoMessages.getEventPointer(i + 1)->message;

				if (!juce::approximatelyEqual(m2.getTimeStamp(), eventTime))
					break;

				/** Next Is Beat Meta */
				if (m2.isTimeSignatureMetaEvent()) {
					m2.getTimeSignatureInfo(thisNumerator, thisDenominator);

					/** Next Tempo Temp */
					while ((tempoTemp.size() > (tempoTempIndex + 1)) &&
						(eventTime > std::get<0>(tempoTemp.getReference(tempoTempIndex + 1)))) {
						std::tie(tempoSec, tempoQuarter, tempoSPQ) = tempoTemp.getReference(++tempoTempIndex);

						/** Add Tempo Temp */
						double tempoBar = lastBar + (tempoQuarter - lastQuarter) * (lastDenominator / 4.0) / lastNumerator;
						if (std::get<1>(this->temp.getLast()) < tempoQuarter) {
							this->temp.add({ tempoSec, tempoQuarter, tempoBar, tempoSPQ,
								lastNumerator * (4.0 / lastDenominator),
								lastNumerator, lastDenominator });
						}
						else {
							this->temp.getReference(this->temp.size() - 1)
								= std::make_tuple(tempoSec, tempoQuarter, tempoBar, tempoSPQ,
									lastNumerator * (4.0 / lastDenominator),
									lastNumerator, lastDenominator);
						}
					}

					/** Get Event Valid Quarter */
					double quarter = tempoQuarter + (eventTime - tempoSec) / tempoSPQ;
					double bar = lastBar + (quarter - lastQuarter) * (lastDenominator / 4.0) / lastNumerator;
					thisBar = std::floor(bar);
					if (!juce::approximatelyEqual(thisBar, bar)) {
						thisBar++;
					}
					thisQuarter = lastQuarter + (thisBar - lastBar) / (lastDenominator / 4.0) * lastNumerator;

					/** Next Tempo Temp */
					while ((tempoTemp.size() > (tempoTempIndex + 1)) &&
						(thisQuarter > std::get<1>(tempoTemp.getReference(tempoTempIndex + 1)))) {
						std::tie(tempoSec, tempoQuarter, tempoSPQ) = tempoTemp.getReference(++tempoTempIndex);

						/** Add Tempo Temp */
						double tempoBar = lastBar + (tempoQuarter - lastQuarter) * (lastDenominator / 4.0) / lastNumerator;
						if (std::get<1>(this->temp.getLast()) < tempoQuarter) {
							this->temp.add({ tempoSec, tempoQuarter, tempoBar, tempoSPQ,
								lastNumerator * (4.0 / lastDenominator),
								lastNumerator, lastDenominator });
						}
						else {
							this->temp.getReference(this->temp.size() - 1)
								= std::make_tuple(tempoSec, tempoQuarter, tempoBar, tempoSPQ,
									lastNumerator * (4.0 / lastDenominator),
									lastNumerator, lastDenominator);
						}
					}
				}

				i++;
			}
		}

		/** Write The Last Beat */
		{
			double thisSec = tempoSec + (thisQuarter - tempoQuarter) * tempoSPQ;
			if (std::get<0>(this->temp.getLast()) < thisSec) {
				this->temp.add({ thisSec, thisQuarter, thisBar, tempoSPQ,
					thisNumerator * (4.0 / thisDenominator),
					thisNumerator, thisDenominator });
			}
			else {
				this->temp.getReference(this->temp.size() - 1)
					= std::make_tuple(thisSec, thisQuarter, thisBar, tempoSPQ,
						thisNumerator * (4.0 / thisDenominator),
						thisNumerator, thisDenominator);
			}
		}
	}

	/** Add Remained Tempos */
	{
		int lastNumerator = 4, lastDenominator = 4;
		double lastQPB = 0.5;
		double lastQuarter = 0, lastBar = 0;
		std::tie(std::ignore, lastQuarter, lastBar, std::ignore, lastQPB, lastNumerator, lastDenominator) = this->temp.getReference(this->temp.size() - 1);

		for (int i = tempoTempIndex + 1; i < tempoTemp.size(); i++) {
			auto& [tempoSec, tempoQuarter, tempoSPQ] = tempoTemp.getReference(i);
			double tempoBar = lastBar + (tempoQuarter - lastQuarter) / lastQPB;

			this->temp.add({ tempoSec, tempoQuarter, tempoBar, tempoSPQ, lastQPB, lastNumerator, lastDenominator });
		}
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

	/** Check Last */
	if (this->temp.size() > 0) {
		auto& [timeInSec, timeInQuarter, timeInBar,
			secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(this->temp.size() - 1);
		if (time >= timeInSec) {
			return this->lastIndex = (this->temp.size() - 1);
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
	return this->lastIndex = this->search(0, this->temp.size() - 2, time, cFunc);
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

	/** Check Last */
	if (this->temp.size() > 0) {
		auto& [timeInSec, timeInQuarter, timeInBar,
			secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(this->temp.size() - 1);
		if (timeQuarter >= timeInQuarter) {
			return this->lastIndex = (this->temp.size() - 1);
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
	return this->lastIndex = this->search(0, this->temp.size() - 2, timeQuarter, cFunc);
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

	/** Check Last */
	if (this->temp.size() > 0) {
		auto& [timeInSec, timeInQuarter, timeInBar,
			secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(this->temp.size() - 1);
		if (timeBar >= timeInBar) {
			return this->lastIndex = (this->temp.size() - 1);
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
	return this->lastIndex = this->search(0, this->temp.size() - 2, timeBar, cFunc);
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

TempoTemp::TempoDataMini TempoTemp::getTempoDataMini(int tempIndex) const {
	if (tempIndex < 0 || tempIndex >= this->temp.size()) { return { 0, 0, 0, 0.5, 4, 4 }; }
	const auto& [timeInSec, timeInQuarter, timeInBar,
		secPerQuarter, quarterPerBar, numerator, denominator] = this->temp.getReference(tempIndex);
	return { timeInSec, timeInQuarter, timeInBar, secPerQuarter, numerator, denominator };
}

template<typename Func, typename T>
int TempoTemp::search(int low, int high, T value, Func func) const {
	if (high < low) { return -1; }
	
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

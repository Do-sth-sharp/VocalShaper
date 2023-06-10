#include "Utils.h"

/** To Fix Symbol Export Error Of juce::dsp::Panner<float> */
#include <juce_dsp/processors/juce_Panner.cpp>

/** To Fix Symbol Export Of juce::AudioProcessorParameterGroup */
//#include <juce_audio_processors/processors/juce_AudioProcessorParameterGroup.cpp>

namespace utils {
	std::tuple<int, int> getChannelIndexAndNumOfBus(
		const juce::AudioProcessor* processor, int busIndex, bool isInput) {
		if (!processor) { return std::tuple<int, int>(); }
		if (busIndex < 0 || busIndex >= processor->getBusCount(isInput)) {
			return std::tuple<int, int>();
		}

		int index = 0, num = 0;
		for (int i = 0; i <= busIndex; i++) {
			num = processor->getChannelCountOfBus(isInput, i);
			if (i != busIndex) {
				index += num;
			}
		}

		return std::make_tuple(index, num);
	}

	double convertSecondsToTicks(double time,
		const juce::MidiMessageSequence& tempoEvents,
		int timeFormat) {
		if (timeFormat < 0)
			return time * (-(timeFormat >> 8) * (timeFormat & 0xff));

		double lastTime = 0, correctedTime = 0;
		auto tickLen = 1.0 / (timeFormat & 0x7fff);
		auto secsPerTick = 0.5 * tickLen;
		auto numEvents = tempoEvents.getNumEvents();

		for (int i = 0; i < numEvents; ++i) {
			auto& m = tempoEvents.getEventPointer(i)->message;
			auto eventTime = m.getTimeStamp();

			if (eventTime >= time)
				break;

			correctedTime += (eventTime - lastTime) / secsPerTick;
			lastTime = eventTime;

			if (m.isTempoMetaEvent())
				secsPerTick = tickLen * m.getTempoSecondsPerQuarterNote();

			while (i + 1 < numEvents) {
				auto& m2 = tempoEvents.getEventPointer(i + 1)->message;

				if (!juce::approximatelyEqual(m2.getTimeStamp(), eventTime))
					break;

				if (m2.isTempoMetaEvent())
					secsPerTick = tickLen * m2.getTempoSecondsPerQuarterNote();

				++i;
			}
		}

		return correctedTime + (time - lastTime) / secsPerTick;
	}

	double convertTicksToSeconds(double time,
		const juce::MidiMessageSequence& tempoEvents,
		int timeFormat) {
		if (timeFormat < 0)
			return time / (-(timeFormat >> 8) * (timeFormat & 0xff));

		double lastTime = 0, correctedTime = 0;
		auto tickLen = 1.0 / (timeFormat & 0x7fff);
		auto secsPerTick = 0.5 * tickLen;
		auto numEvents = tempoEvents.getNumEvents();

		for (int i = 0; i < numEvents; ++i) {
			auto& m = tempoEvents.getEventPointer(i)->message;
			auto eventTime = m.getTimeStamp();

			if (eventTime >= time)
				break;

			correctedTime += (eventTime - lastTime) * secsPerTick;
			lastTime = eventTime;

			if (m.isTempoMetaEvent())
				secsPerTick = tickLen * m.getTempoSecondsPerQuarterNote();

			while (i + 1 < numEvents) {
				auto& m2 = tempoEvents.getEventPointer(i + 1)->message;

				if (!juce::approximatelyEqual(m2.getTimeStamp(), eventTime))
					break;

				if (m2.isTempoMetaEvent())
					secsPerTick = tickLen * m2.getTempoSecondsPerQuarterNote();

				++i;
			}
		}

		return correctedTime + (time - lastTime) * secsPerTick;
	}

	double convertSecondsToTicksWithObjectiveTempoTime(double time,
		const juce::MidiMessageSequence& tempoEvents,
		int timeFormat) {
		if (timeFormat < 0)
			return time * (-(timeFormat >> 8) * (timeFormat & 0xff));

		double lastTime = 0, correctedTime = 0;
		auto tickLen = 1.0 / (timeFormat & 0x7fff);
		auto secsPerTick = 0.5 * tickLen;
		auto numEvents = tempoEvents.getNumEvents();

		for (int i = 0; i < numEvents; ++i) {
			auto& m = tempoEvents.getEventPointer(i)->message;
			auto eventTime = m.getTimeStamp();

			double srcTimeDistance = (eventTime - correctedTime) * secsPerTick;
			if (lastTime + srcTimeDistance >= time)
				break;

			lastTime += srcTimeDistance;
			correctedTime = eventTime;

			if (m.isTempoMetaEvent())
				secsPerTick = tickLen * m.getTempoSecondsPerQuarterNote();

			while (i + 1 < numEvents) {
				auto& m2 = tempoEvents.getEventPointer(i + 1)->message;

				if (!juce::approximatelyEqual(m2.getTimeStamp(), eventTime))
					break;

				if (m2.isTempoMetaEvent())
					secsPerTick = tickLen * m2.getTempoSecondsPerQuarterNote();

				++i;
			}
		}

		return correctedTime + (time - lastTime) / secsPerTick;
	}

	double convertTicksToSecondsWithObjectiveTempoTime(double time,
		const juce::MidiMessageSequence& tempoEvents,
		int timeFormat) {
		if (timeFormat < 0)
			return time / (-(timeFormat >> 8) * (timeFormat & 0xff));

		double lastTime = 0, correctedTime = 0;
		auto tickLen = 1.0 / (timeFormat & 0x7fff);
		auto secsPerTick = 0.5 * tickLen;
		auto numEvents = tempoEvents.getNumEvents();

		for (int i = 0; i < numEvents; ++i) {
			auto& m = tempoEvents.getEventPointer(i)->message;
			auto eventTime = m.getTimeStamp();

			double srcTimeDistance = (eventTime - correctedTime) / secsPerTick;
			if (lastTime + srcTimeDistance >= time)
				break;

			lastTime += srcTimeDistance;
			correctedTime = eventTime;

			if (m.isTempoMetaEvent())
				secsPerTick = tickLen * m.getTempoSecondsPerQuarterNote();

			while (i + 1 < numEvents) {
				auto& m2 = tempoEvents.getEventPointer(i + 1)->message;

				if (!juce::approximatelyEqual(m2.getTimeStamp(), eventTime))
					break;

				if (m2.isTempoMetaEvent())
					secsPerTick = tickLen * m2.getTempoSecondsPerQuarterNote();

				++i;
			}
		}

		return correctedTime + (time - lastTime) * secsPerTick;
	}

	std::tuple<int, double, double> getBarBySecond(double time,
		const juce::MidiMessageSequence& tempoEvents) {
		auto numEvents = tempoEvents.getNumEvents();

		/** Event Time In Quarter */
		double correctedQuarter = 0;
		double lastValidQuarter = 0;

		/** Event Time In Seconds */
		double lastTime = 0;
		double lastValidSecond = 0;

		/** Event Time In Bar */
		double lastValidBar = 0;

		/** State Temp */
		double secsPerQuarter = 0.5, quarterPerBar = 4.0;

		/** State Wait For Valid Temp */
		double tempValidQuarter = 0;
		double tempValidSecond = 0;
		double tempValidBar = 0;
		double quarterPerBarWaitForValid = 4.0;

		for (int i = 0; i < numEvents; ++i) {
			auto& m = tempoEvents.getEventPointer(i)->message;
			auto eventTime = m.getTimeStamp();

			if (eventTime >= time)
				break;

			correctedQuarter += (eventTime - lastTime) / secsPerQuarter;
			lastTime = eventTime;

			/** Temp Valid */
			if (correctedQuarter > tempValidQuarter) {
				lastValidQuarter = tempValidQuarter;
				lastValidSecond = tempValidSecond;
				lastValidBar = tempValidBar;
				quarterPerBar = quarterPerBarWaitForValid;
			}

			if (m.isTempoMetaEvent()) {
				secsPerQuarter = m.getTempoSecondsPerQuarterNote();

				/** Check Delay Valid */
				double quarterDistanceFromLastValid = correctedQuarter - lastValidQuarter;
				double barCountFromLastValid = quarterDistanceFromLastValid / quarterPerBar;
				double barDistance = std::floor(barCountFromLastValid);
				if (!juce::approximatelyEqual(barDistance, barCountFromLastValid)) {
					barDistance++;
				}

				/** Set Wait For Valid Temp */
				tempValidBar = lastValidBar + barDistance;
				tempValidQuarter = lastValidQuarter + barDistance * quarterPerBar;
				tempValidSecond = lastValidSecond + barDistance * quarterPerBar * secsPerQuarter;
			}

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
				tempValidSecond = lastValidSecond + barDistance * quarterPerBar * secsPerQuarter;
			}

			while (i + 1 < numEvents) {
				auto& m2 = tempoEvents.getEventPointer(i + 1)->message;

				if (!juce::approximatelyEqual(m2.getTimeStamp(), eventTime))
					break;

				if (m2.isTempoMetaEvent()) {
					secsPerQuarter = m2.getTempoSecondsPerQuarterNote();

					/** Check Delay Valid */
					double quarterDistanceFromLastValid = correctedQuarter - lastValidQuarter;
					double barCountFromLastValid = quarterDistanceFromLastValid / quarterPerBar;
					double barDistance = std::floor(barCountFromLastValid);
					if (!juce::approximatelyEqual(barDistance, barCountFromLastValid)) {
						barDistance++;
					}

					/** Set Wait For Valid Temp */
					tempValidBar = lastValidBar + barDistance;
					tempValidQuarter = lastValidQuarter + barDistance * quarterPerBar;
					tempValidSecond = lastValidSecond + barDistance * quarterPerBar * secsPerQuarter;
				}

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
					tempValidSecond = lastValidSecond + barDistance * quarterPerBar * secsPerQuarter;
				}

				++i;
			}
		}

		/** Get Time In Quarter */
		double timeQuarter = correctedQuarter + (time - lastTime) / secsPerQuarter;

		/** Temp Valid */
		if (timeQuarter > tempValidQuarter) {
			lastValidQuarter = tempValidQuarter;
			lastValidSecond = tempValidSecond;
			lastValidBar = tempValidBar;
			quarterPerBar = quarterPerBarWaitForValid;
		}

		int barResult = lastValidBar + std::floor((timeQuarter - lastValidQuarter) / quarterPerBar);
		return std::make_tuple(
			barResult, 
			lastValidQuarter + (barResult - lastValidBar) * quarterPerBar,
			lastValidSecond + (barResult - lastValidBar) * quarterPerBar * secsPerQuarter);
	}
}

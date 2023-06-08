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

        for (int i = 0; i < numEvents; ++i)
        {
            auto& m = tempoEvents.getEventPointer(i)->message;
            auto eventTime = m.getTimeStamp();

            if (eventTime >= time)
                break;

            correctedTime += (eventTime - lastTime) / secsPerTick;
            lastTime = eventTime;

            if (m.isTempoMetaEvent())
                secsPerTick = tickLen * m.getTempoSecondsPerQuarterNote();

            while (i + 1 < numEvents)
            {
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

        for (int i = 0; i < numEvents; ++i)
        {
            auto& m = tempoEvents.getEventPointer(i)->message;
            auto eventTime = m.getTimeStamp();

            if (eventTime >= time)
                break;

            correctedTime += (eventTime - lastTime) * secsPerTick;
            lastTime = eventTime;

            if (m.isTempoMetaEvent())
                secsPerTick = tickLen * m.getTempoSecondsPerQuarterNote();

            while (i + 1 < numEvents)
            {
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

        auto resFunc = [&correctedTime, &time, &secsPerTick, &lastTime] () -> double {
            return correctedTime + (time / secsPerTick - lastTime);
        };

        for (int i = 0; i < numEvents; ++i)
        {
            auto& m = tempoEvents.getEventPointer(i)->message;
            auto eventTime = m.getTimeStamp();

            double result = resFunc();
            if (eventTime >= result)
                return result;

            correctedTime += (eventTime - lastTime);
            lastTime = eventTime;

            if (m.isTempoMetaEvent())
                secsPerTick = tickLen * m.getTempoSecondsPerQuarterNote();

            while (i + 1 < numEvents)
            {
                auto& m2 = tempoEvents.getEventPointer(i + 1)->message;

                if (!juce::approximatelyEqual(m2.getTimeStamp(), eventTime))
                    break;

                if (m2.isTempoMetaEvent())
                    secsPerTick = tickLen * m2.getTempoSecondsPerQuarterNote();

                ++i;
            }
        }

        return resFunc();
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

        auto resFunc = [&correctedTime, &time, &secsPerTick, &lastTime]() -> double {
            return correctedTime + (time * secsPerTick - lastTime);
        };

        for (int i = 0; i < numEvents; ++i)
        {
            auto& m = tempoEvents.getEventPointer(i)->message;
            auto eventTime = m.getTimeStamp();

            double result = resFunc();
            if (eventTime >= result)
                return result;

            correctedTime += (eventTime - lastTime);
            lastTime = eventTime;

            if (m.isTempoMetaEvent())
                secsPerTick = tickLen * m.getTempoSecondsPerQuarterNote();

            while (i + 1 < numEvents)
            {
                auto& m2 = tempoEvents.getEventPointer(i + 1)->message;

                if (!juce::approximatelyEqual(m2.getTimeStamp(), eventTime))
                    break;

                if (m2.isTempoMetaEvent())
                    secsPerTick = tickLen * m2.getTempoSecondsPerQuarterNote();

                ++i;
            }
        }

        return resFunc();
    }
}

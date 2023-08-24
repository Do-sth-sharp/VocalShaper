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

	void convertSecondsToTicks(juce::MidiFile& file) {
		/** Get Tempo Events */
		juce::MidiMessageSequence tempoSeq;
		file.findAllTempoEvents(tempoSeq);
		file.findAllTimeSigEvents(tempoSeq);

		auto timeFormat = file.getTimeFormat();
		if (timeFormat != 0)
		{
			for (int i = 0; i < file.getNumTracks(); i++) {
				if (auto track = file.getTrack(i)) {
					for (int j = track->getNumEvents(); --j >= 0;)
					{
						auto& m = track->getEventPointer(j)->message;
						m.setTimeStamp(utils::convertSecondsToTicks(m.getTimeStamp(), tempoSeq, timeFormat));
					}
				}
			}
		}
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

	std::tuple<int, double> getBarBySecond(double time,
		const juce::MidiMessageSequence& tempoEvents) {
		auto numEvents = tempoEvents.getNumEvents();

		/** Temporary */
		juce::Array<std::tuple<double, double, double, double>> tempList;

		/** Corrected Time */
		double correctedQuarter = 0;
		double lastValidQuarter = 0;

		/** Event Time In Bar */
		double lastValidBar = 0;

		/** Event Time In Seconds */
		double lastTime = 0;

		/** State Temp */
		double secsPerQuarter = 0.5, quarterPerBar = 4.0;

		/** State Wait For Valid Temp */
		double tempValidQuarter = 0;
		double tempValidBar = 0;
		double secPerQuarterWaitForValid = 0.5;
		double quarterPerBarWaitForValid = 4.0;

		/** Add First Event */
		tempList.add({ tempValidQuarter, tempValidBar, secPerQuarterWaitForValid, quarterPerBarWaitForValid });

		/** Build Temporary */
		for (int i = 0; i < numEvents; ++i) {
			auto& m = tempoEvents.getEventPointer(i)->message;
			auto eventTime = m.getTimeStamp();

			if (eventTime >= time)
				break;

			/** Temp Valid */
			if (correctedQuarter + (eventTime - lastTime) / secsPerQuarter > tempValidQuarter) {
				if (std::get<0>(tempList.getLast()) < tempValidQuarter) {
					tempList.add({ tempValidQuarter, tempValidBar, secPerQuarterWaitForValid, quarterPerBarWaitForValid });
				}
				else {
					tempList.getReference(tempList.size() - 1)
						= std::make_tuple(tempValidQuarter, tempValidBar, secPerQuarterWaitForValid, quarterPerBarWaitForValid);
				}

				lastValidQuarter = tempValidQuarter;
				lastValidBar = tempValidBar;
				secsPerQuarter = secPerQuarterWaitForValid;
				quarterPerBar = quarterPerBarWaitForValid;
			}

			correctedQuarter += (eventTime - lastTime) / secsPerQuarter;
			lastTime = eventTime;

			if (m.isTempoMetaEvent()) {
				secPerQuarterWaitForValid = m.getTempoSecondsPerQuarterNote();

				/** Check Delay Valid */
				double quarterDistanceFromLastValid = correctedQuarter - lastValidQuarter;
				double barCountFromLastValid = quarterDistanceFromLastValid / quarterPerBar;
				double barDistance = barCountFromLastValid;

				/** Set Wait For Valid Temp */
				tempValidQuarter = lastValidQuarter + barDistance * quarterPerBar;
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
			}

			while (i + 1 < numEvents) {
				auto& m2 = tempoEvents.getEventPointer(i + 1)->message;

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

				++i;
			}
		}

		/** Temp Valid */
		if (correctedQuarter + (time - lastTime) / secsPerQuarter > tempValidQuarter) {
			if (std::get<0>(tempList.getLast()) < tempValidQuarter) {
				tempList.add({ tempValidQuarter, tempValidBar, secPerQuarterWaitForValid, quarterPerBarWaitForValid });
			}
			else {
				tempList.getReference(tempList.size() - 1)
					= std::make_tuple(tempValidQuarter, tempValidBar, secPerQuarterWaitForValid, quarterPerBarWaitForValid);
			}

			lastValidQuarter = tempValidQuarter;
			lastValidBar = tempValidBar;
			secsPerQuarter = secPerQuarterWaitForValid;
			quarterPerBar = quarterPerBarWaitForValid;
		}

		/** Get Time In Quarter */
		double timeQuarter = correctedQuarter + (time - lastTime) / secsPerQuarter;

		/** Find Bar */
		int barResult = std::floor(lastValidBar + (timeQuarter - lastValidQuarter) / quarterPerBar);

		/** Find Last Match Temp */
		for (int i = tempList.size() - 1; i >= 0; i--) {
			auto& [currentQuarter, currentBar, currentSPQ, currentQPB] = tempList.getReference(i);

			if (currentBar > barResult) {
				continue;
			}

			return { barResult, currentQuarter + (barResult - currentBar) * currentQPB };
		}

		return { 0, 0. };
	}

	class SingletonAudioFormatManager : public juce::AudioFormatManager,
		private juce::DeletedAtShutdown {
	public:
		SingletonAudioFormatManager();

	public:
		static SingletonAudioFormatManager* getInstance();

	private:
		static SingletonAudioFormatManager* instance;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SingletonAudioFormatManager)
	};

	SingletonAudioFormatManager::SingletonAudioFormatManager()
		: AudioFormatManager() {
		this->registerBasicFormats();
	}

	SingletonAudioFormatManager* SingletonAudioFormatManager::instance = nullptr;

	SingletonAudioFormatManager* SingletonAudioFormatManager::getInstance() {
		return SingletonAudioFormatManager::instance ? SingletonAudioFormatManager::instance : SingletonAudioFormatManager::instance = new SingletonAudioFormatManager();
	}

	juce::AudioFormat* findAudioFormat(const juce::File& file) {
		return SingletonAudioFormatManager::getInstance()->findFormatForFileExtension(file.getFileExtension());
	}

	std::unique_ptr<juce::AudioFormatReader> createAudioReader(const juce::File& file) {
		auto format = utils::findAudioFormat(file);
		if (!format) { return nullptr; }

		return std::unique_ptr<juce::AudioFormatReader>(format->createReaderFor(new juce::FileInputStream(file), true));
	}

	std::unique_ptr<juce::AudioFormatWriter> createAudioWriter(const juce::File& file,
		double sampleRateToUse, const juce::AudioChannelSet& channelLayout,
		int bitsPerSample, const juce::StringPairArray& metadataValues, int qualityOptionIndex) {
		auto format = utils::findAudioFormat(file);
		if (!format) { return nullptr; }

		auto outStream = new juce::FileOutputStream(file);
		if (outStream->openedOk()) {
			outStream->setPosition(0);
			outStream->truncate();
		}

		auto writer = format->createWriterFor(outStream,
			sampleRateToUse, channelLayout, bitsPerSample, metadataValues, qualityOptionIndex);
		if (!writer) {
			delete outStream;
			return nullptr;
		}

		return std::unique_ptr<juce::AudioFormatWriter>(writer);
	}
}

#include "Utils.h"

/** To Fix Symbol Export Error Of juce::dsp::Panner<float> */
#include <juce_dsp/processors/juce_Panner.cpp>

/** To Fix Symbol Export Of juce::AudioProcessorParameterGroup */
//#include <juce_audio_processors/processors/juce_AudioProcessorParameterGroup.cpp>

/** LAME Path */
#if JUCE_WINDOWS
#define LAMEPath "./lame.exe"
#elif JUCE_LINUX
#define LAMEPath "./lame"
#elif JUCE_MAC
#define LAMEPath "./lame"
#else
#define LAMEPath "./lame"
#endif // JUCE_WINDOWS

#include "Platform.h"

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

	const int AudioSaveConfig::getBitsPerSample(const juce::String& format) const {
		juce::ScopedReadLock locker(this->lock);

		auto it = this->bitsPerSample.find(format);
		return (it != this->bitsPerSample.end())
			? it->second : 24;
	}

	const juce::StringPairArray AudioSaveConfig::getMetaData(const juce::String& format) const {
		juce::ScopedReadLock locker(this->lock);

		auto it = this->metaData.find(format);
		return (it != this->metaData.end())
			? it->second : juce::StringPairArray{};
	}

	const int AudioSaveConfig::getQualityOptionIndex(const juce::String& format) const {
		juce::ScopedReadLock locker(this->lock);

		auto it = this->qualityOptionIndex.find(format);
		return (it != this->qualityOptionIndex.end())
			? it->second : 0;
	}

	void AudioSaveConfig::setBitsPerSample(const juce::String& format, int value) {
		juce::ScopedWriteLock locker(this->lock);
		this->bitsPerSample[format] = value;
	}

	void AudioSaveConfig::setMetaData(
		const juce::String& format, const juce::StringPairArray& data) {
		juce::ScopedWriteLock locker(this->lock);
		this->metaData[format] = data;
	}

	void AudioSaveConfig::setQualityOptionIndex(
		const juce::String& format, int value) {
		juce::ScopedWriteLock locker(this->lock);
		AudioSaveConfig::getInstance()->qualityOptionIndex[format] = value;
	}

	AudioSaveConfig* AudioSaveConfig::getInstance() {
		return AudioSaveConfig::instance
			? AudioSaveConfig::instance
			: (AudioSaveConfig::instance = new AudioSaveConfig);
	}

	AudioSaveConfig* AudioSaveConfig::instance = nullptr;

	class SingletonAudioFormatManager : public juce::AudioFormatManager,
		private juce::DeletedAtShutdown {
	public:
		SingletonAudioFormatManager();

		juce::AudioFormat* getLAMEEncoderFormat() const;

	public:
		static SingletonAudioFormatManager* getInstance();

	private:
		std::unique_ptr<juce::AudioFormat> LAMEFormat = nullptr;

	private:
		static SingletonAudioFormatManager* instance;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SingletonAudioFormatManager)
	};

	SingletonAudioFormatManager::SingletonAudioFormatManager()
		: AudioFormatManager() {
		/** Basic Formats */
		this->registerBasicFormats();

		/** LAME Encoder */
		juce::File LAMEExecutable = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
			.getParentDirectory().getChildFile(LAMEPath);
		this->LAMEFormat = std::unique_ptr<juce::AudioFormat>(
			new juce::LAMEEncoderAudioFormat(LAMEExecutable));
	}

	juce::AudioFormat* SingletonAudioFormatManager::getLAMEEncoderFormat() const {
		return this->LAMEFormat.get();
	}

	SingletonAudioFormatManager* SingletonAudioFormatManager::instance = nullptr;

	SingletonAudioFormatManager* SingletonAudioFormatManager::getInstance() {
		return SingletonAudioFormatManager::instance ? SingletonAudioFormatManager::instance : SingletonAudioFormatManager::instance = new SingletonAudioFormatManager();
	}

	juce::AudioFormat* findAudioFormat(const juce::File& file, bool isWrite) {
		auto extension = file.getFileExtension();

		if (isWrite) {
			auto LAMEFormat =
				SingletonAudioFormatManager::getInstance()->getLAMEEncoderFormat();
			if (LAMEFormat && LAMEFormat->getFileExtensions().contains(extension)) {
				return LAMEFormat;
			}
		}

		return SingletonAudioFormatManager::getInstance()
			->findFormatForFileExtension(extension);
	}

	std::unique_ptr<juce::AudioFormatReader> createAudioReader(const juce::File& file) {
		auto format = utils::findAudioFormat(file, false);
		if (!format) { return nullptr; }

		return std::unique_ptr<juce::AudioFormatReader>(format->createReaderFor(new juce::FileInputStream(file), true));
	}

	std::unique_ptr<juce::AudioFormatWriter> createAudioWriter(const juce::File& file,
		double sampleRateToUse, const juce::AudioChannelSet& channelLayout) {
		auto format = utils::findAudioFormat(file, true);
		if (!format) { return nullptr; }

		juce::String formatName = format->getFormatName();
		auto bitsPerSample =
			AudioSaveConfig::getInstance()->getBitsPerSample(formatName);
		auto metadataValues =
			AudioSaveConfig::getInstance()->getMetaData(formatName);
		auto qualityOptionIndex =
			AudioSaveConfig::getInstance()->getQualityOptionIndex(formatName);

		auto outStream = new juce::FileOutputStream(file);
		if (outStream->openedOk()) {
			outStream->setPosition(0);
			outStream->truncate();
		}

		auto writer = format->createWriterFor(outStream,
			sampleRateToUse, channelLayout, bitsPerSample, metadataValues, qualityOptionIndex);
		if (!writer) {
			delete outStream;
			file.deleteFile();
			return nullptr;
		}

		return std::unique_ptr<juce::AudioFormatWriter>(writer);
	}

	const juce::AudioChannelSet getChannelSet(TrackType type) {
		juce::AudioChannelSet trackType;

		switch (type) {
		case TrackType::DISABLED:
			trackType = juce::AudioChannelSet::disabled();
			break;
		case TrackType::MONO:
			trackType = juce::AudioChannelSet::mono();
			break;
		case TrackType::STEREO:
			trackType = juce::AudioChannelSet::stereo();
			break;
		case TrackType::LCR:
			trackType = juce::AudioChannelSet::createLCR();
			break;
		case TrackType::LRS:
			trackType = juce::AudioChannelSet::createLRS();
			break;
		case TrackType::LCRS:
			trackType = juce::AudioChannelSet::createLCRS();
			break;
		case TrackType::SUR_5_0:
			trackType = juce::AudioChannelSet::create5point0();
			break;
		case TrackType::SUR_5_1:
			trackType = juce::AudioChannelSet::create5point1();
			break;
		case TrackType::SUR_5_0_2:
			trackType = juce::AudioChannelSet::create5point0point2();
			break;
		case TrackType::SUR_5_1_2:
			trackType = juce::AudioChannelSet::create5point1point2();
			break;
		case TrackType::SUR_5_0_4:
			trackType = juce::AudioChannelSet::create5point0point4();
			break;
		case TrackType::SUR_5_1_4:
			trackType = juce::AudioChannelSet::create5point1point4();
			break;
		case TrackType::SUR_6_0:
			trackType = juce::AudioChannelSet::create6point0();
			break;
		case TrackType::SUR_6_1:
			trackType = juce::AudioChannelSet::create6point1();
			break;
		case TrackType::SUR_6_0_M:
			trackType = juce::AudioChannelSet::create6point0Music();
			break;
		case TrackType::SUR_6_1_M:
			trackType = juce::AudioChannelSet::create6point1Music();
			break;
		case TrackType::SUR_7_0:
			trackType = juce::AudioChannelSet::create7point0();
			break;
		case TrackType::SUR_7_0_SDSS:
			trackType = juce::AudioChannelSet::create7point0SDDS();
			break;
		case TrackType::SUR_7_1:
			trackType = juce::AudioChannelSet::create7point1();
			break;
		case TrackType::SUR_7_1_SDSS:
			trackType = juce::AudioChannelSet::create7point1SDDS();
			break;
		case TrackType::SUR_7_0_2:
			trackType = juce::AudioChannelSet::create7point0point2();
			break;
		case TrackType::SUR_7_1_2:
			trackType = juce::AudioChannelSet::create7point1point2();
			break;
		case TrackType::SUR_7_0_4:
			trackType = juce::AudioChannelSet::create7point0point4();
			break;
		case TrackType::SUR_7_1_4:
			trackType = juce::AudioChannelSet::create7point1point4();
			break;
		case TrackType::SUR_7_0_6:
			trackType = juce::AudioChannelSet::create7point0point6();
			break;
		case TrackType::SUR_7_1_6:
			trackType = juce::AudioChannelSet::create7point1point6();
			break;
		case TrackType::SUR_9_0_4:
			trackType = juce::AudioChannelSet::create9point0point4();
			break;
		case TrackType::SUR_9_1_4:
			trackType = juce::AudioChannelSet::create9point1point4();
			break;
		case TrackType::SUR_9_0_6:
			trackType = juce::AudioChannelSet::create9point0point6();
			break;
		case TrackType::SUR_9_1_6:
			trackType = juce::AudioChannelSet::create9point1point6();
			break;
		case TrackType::QUADRAPHONIC:
			trackType = juce::AudioChannelSet::quadraphonic();
			break;
		case TrackType::PENTAGONAL:
			trackType = juce::AudioChannelSet::pentagonal();
			break;
		case TrackType::HEXAGONAL:
			trackType = juce::AudioChannelSet::hexagonal();
			break;
		case TrackType::OCTAGONAL:
			trackType = juce::AudioChannelSet::octagonal();
			break;
		case TrackType::AMBISONIC_0:
		case TrackType::AMBISONIC_1:
		case TrackType::AMBISONIC_2:
		case TrackType::AMBISONIC_3:
		case TrackType::AMBISONIC_4:
		case TrackType::AMBISONIC_5:
		case TrackType::AMBISONIC_6:
		case TrackType::AMBISONIC_7:
			trackType = juce::AudioChannelSet::ambisonic(static_cast<int>(type) - 100);
			break;
		default:
			trackType = juce::AudioChannelSet::stereo();
			break;
		}

		return trackType;
	}

	TrackType getTrackType(const juce::AudioChannelSet& channels) {
		if (channels == juce::AudioChannelSet::disabled()) {
			return TrackType::DISABLED;
		}
		else if (channels == juce::AudioChannelSet::mono()) {
			return TrackType::MONO;
		}
		else if (channels == juce::AudioChannelSet::stereo()) {
			return TrackType::STEREO;
		}
		else if (channels == juce::AudioChannelSet::createLCR()) {
			return TrackType::LCR;
		}
		else if (channels == juce::AudioChannelSet::createLRS()) {
			return TrackType::LRS;
		}
		else if (channels == juce::AudioChannelSet::createLCRS()) {
			return TrackType::LCRS;
		}
		else if (channels == juce::AudioChannelSet::create5point0()) {
			return TrackType::SUR_5_0;
		}
		else if (channels == juce::AudioChannelSet::create5point1()) {
			return TrackType::SUR_5_1;
		}
		else if (channels == juce::AudioChannelSet::create5point0point2()) {
			return TrackType::SUR_5_0_2;
		}
		else if (channels == juce::AudioChannelSet::create5point1point2()) {
			return TrackType::SUR_5_1_2;
		}
		else if (channels == juce::AudioChannelSet::create5point0point4()) {
			return TrackType::SUR_5_0_4;
		}
		else if (channels == juce::AudioChannelSet::create5point1point4()) {
			return TrackType::SUR_5_1_4;
		}
		else if (channels == juce::AudioChannelSet::create6point0()) {
			return TrackType::SUR_6_0;
		}
		else if (channels == juce::AudioChannelSet::create6point1()) {
			return TrackType::SUR_6_1;
		}
		else if (channels == juce::AudioChannelSet::create6point0Music()) {
			return TrackType::SUR_6_0_M;
		}
		else if (channels == juce::AudioChannelSet::create6point1Music()) {
			return TrackType::SUR_6_1_M;
		}
		else if (channels == juce::AudioChannelSet::create7point0()) {
			return TrackType::SUR_7_0;
		}
		else if (channels == juce::AudioChannelSet::create7point0SDDS()) {
			return TrackType::SUR_7_0_SDSS;
		}
		else if (channels == juce::AudioChannelSet::create7point1()) {
			return TrackType::SUR_7_1;
		}
		else if (channels == juce::AudioChannelSet::create7point1SDDS()) {
			return TrackType::SUR_7_1_SDSS;
		}
		else if (channels == juce::AudioChannelSet::create7point0point2()) {
			return TrackType::SUR_7_0_2;
		}
		else if (channels == juce::AudioChannelSet::create7point1point2()) {
			return TrackType::SUR_7_1_2;
		}
		else if (channels == juce::AudioChannelSet::create7point0point4()) {
			return TrackType::SUR_7_0_4;
		}
		else if (channels == juce::AudioChannelSet::create7point1point4()) {
			return TrackType::SUR_7_1_4;
		}
		else if (channels == juce::AudioChannelSet::create7point0point6()) {
			return TrackType::SUR_7_0_6;
		}
		else if (channels == juce::AudioChannelSet::create7point1point6()) {
			return TrackType::SUR_7_1_6;
		}
		else if (channels == juce::AudioChannelSet::create9point0point4()) {
			return TrackType::SUR_9_0_4;
		}
		else if (channels == juce::AudioChannelSet::create9point1point4()) {
			return TrackType::SUR_9_1_4;
		}
		else if (channels == juce::AudioChannelSet::create9point0point6()) {
			return TrackType::SUR_9_0_6;
		}
		else if (channels == juce::AudioChannelSet::create9point1point6()) {
			return TrackType::SUR_9_1_6;
		}
		else if (channels == juce::AudioChannelSet::quadraphonic()) {
			return TrackType::QUADRAPHONIC;
		}
		else if (channels == juce::AudioChannelSet::pentagonal()) {
			return TrackType::PENTAGONAL;
		}
		else if (channels == juce::AudioChannelSet::hexagonal()) {
			return TrackType::HEXAGONAL;
		}
		else if (channels == juce::AudioChannelSet::octagonal()) {
			return TrackType::OCTAGONAL;
		}
		else if (channels == juce::AudioChannelSet::ambisonic(0)) {
			return TrackType::AMBISONIC_0;
		}
		else if (channels == juce::AudioChannelSet::ambisonic(1)) {
			return TrackType::AMBISONIC_1;
		}
		else if (channels == juce::AudioChannelSet::ambisonic(2)) {
			return TrackType::AMBISONIC_2;
		}
		else if (channels == juce::AudioChannelSet::ambisonic(3)) {
			return TrackType::AMBISONIC_3;
		}
		else if (channels == juce::AudioChannelSet::ambisonic(4)) {
			return TrackType::AMBISONIC_4;
		}
		else if (channels == juce::AudioChannelSet::ambisonic(5)) {
			return TrackType::AMBISONIC_5;
		}
		else if (channels == juce::AudioChannelSet::ambisonic(6)) {
			return TrackType::AMBISONIC_6;
		}
		else if (channels == juce::AudioChannelSet::ambisonic(7)) {
			return TrackType::AMBISONIC_7;
		}

		return static_cast<TrackType>(-1);
	}

	int versionCompare(const Version& v1, const Version& v2) {
		auto& [major1, minor1, patch1] = v1;
		auto& [major2, minor2, patch2] = v2;

		if (major1 > major2) { return 1; }
		else if (major1 < major2) { return -1; }

		if (minor1 > minor2) { return 1; }
		else if (minor1 < minor2) { return -1; }

		if (patch1 > patch2) { return 1; }
		else if (patch1 < patch2) { return -1; }

		return 0;
	}

	uint32_t getCurrentTime() {
		return juce::Time::currentTimeMillis() / 1000;
	}

	juce::String getAudioPlatformName() {
		return VS_AUDIO_PLATFORM_NAME;
	}

	Version getAudioPlatformVersion() {
		return Version{
			VS_MAJOR_VERSION,
			VS_MINOR_VERSION,
			VS_PATCH_VERSION };
	}

	Version getAudioPlatformVersionMinimumSupported() {
		return Version{
			VS_MINIMUM_SUPPORTED_MAJOR_VERSION,
			VS_MINIMUM_SUPPORTED_MINOR_VERSION,
			VS_MINIMUM_SUPPORTED_PATCH_VERSION };
	}

	juce::String getAudioPlatformVersionString() {
		auto [major, minor, patch] = getAudioPlatformVersion();
		return juce::String(major) + "." + juce::String(minor) + "." + juce::String(patch);
	}

	juce::String getAudioPlatformComplieTime() {
		return __DATE__ " " __TIME__;
	}

	juce::String getSystemNameAndVersion() {
		return juce::SystemStats::getOperatingSystemName();
	}

	juce::String getReleaseBranch() {
		return VS_RELEASE_BRANCH;
	}

	juce::String getReleaseName() {
		return VS_RELEASE_NAME;
	}

	juce::String createPlatformInfoString() {
		return getAudioPlatformName() + ", "
			+ getAudioPlatformVersionString() + ", "
			+ getAudioPlatformComplieTime() + "; "
			+ getReleaseBranch() + ", "
			+ getReleaseName() + "; "
			+ getSystemNameAndVersion();
	}

	juce::String getUserName() {
		return juce::SystemStats::getLogonName();
	}

	juce::String getLegalFileName(const juce::String& name) {
		return name.replaceCharacters(
			"\\/:*?\"<>|", "_________");
	}

	juce::String getSourceDefaultPathForAudio(int id, const juce::String& name) {
		juce::String legalName = getLegalFileName(name);
		return "./" + juce::String(id) + "_" + legalName + ".wav";
	}

	juce::String getSourceDefaultPathForMIDI(int id, const juce::String& name) {
		juce::String legalName = getLegalFileName(name);
		return "./" + juce::String(id) + "_" + legalName + ".mid";
	}

	bool projectVersionHighEnough(const Version& version) {
		return versionCompare(version, getAudioPlatformVersionMinimumSupported()) >= 0;
	}

	bool projectVersionLowEnough(const Version& version) {
		return versionCompare(version, getAudioPlatformVersion()) <= 0;
	}

	static juce::File projectDirectory;

	juce::File getProjectDir() {
		return projectDirectory;
	}

	bool setProjectDir(const juce::File& dir) {
		if (!dir.isDirectory()) { return false; }
		projectDirectory = dir;
		return true;
	}

	juce::File getSourceFile(const juce::String& path) {
		return utils::getProjectDir().getChildFile(path);
	}

	juce::File getDefaultWorkingDir() {
		return juce::File::getSpecialLocation(
			juce::File::SpecialLocationType::userDocumentsDirectory)
			.getChildFile("./VocalShaperProjectTemp/");
	}
}

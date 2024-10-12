#include "Utils.h"
#include "misc/VMath.h"

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
#include "AudioConfig.h"
#include "misc/AudioFormatMeta.h"

#if JUCE_PLUGINHOST_VST3
#include <pluginterfaces/vst/vsttypes.h>
#endif //JUCE_PLUGINHOST_VST3
#if JUCE_PLUGINHOST_VST
#include <pluginterfaces/vst2.x/aeffect.h>
#endif //JUCE_PLUGINHOST_VST
#if JUCE_PLUGINHOST_LV2
#include <juce_audio_processors/format_types/LV2_SDK/juce_lv2_config.h>
#endif //JUCE_PLUGINHOST_LV2
#if JUCE_PLUGINHOST_ARA
#define ARA_VERSION "2.2.0"
#endif //JUCE_PLUGINHOST_ARA

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
		juce::File LAMEExecutable = juce::File::getSpecialLocation(juce::File::hostApplicationPath)
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

	const juce::StringArray getAudioFormatsSupported(bool isWrite) {
		juce::StringArray result{ "*.wav", "*.bwf", "*.flac", "*.mp3", "*.ogg", "*.aiff", "*.aif" };

		if (isWrite) {
#if JUCE_MAC
			result.addArray({ "*.aac", "*.m4a", "*.3gp" });
#endif
		}
		else {
#if JUCE_WINDOWS
			result.addArray({ "*.wmv", "*.asf", "*.wm", "*.wma" });

#elif JUCE_MAC
			result.addArray({ "*.aac", "*.m4a", "*.3gp" });

#endif
		}

		return result;
	}

	const juce::StringArray getMidiFormatsSupported(bool /*isWrite*/) {
		return juce::StringArray{ "*.mid", "*.midi", "*.smf" };
	}

	juce::AudioFormat* findAudioFormatForExtension(const juce::String& extension, bool isWrite) {
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

	juce::AudioFormat* findAudioFormat(const juce::File& file, bool isWrite) {
		auto extension = file.getFileExtension();
		return findAudioFormatForExtension(extension, isWrite);
	}

	std::unique_ptr<juce::AudioFormatReader> createAudioReader(const juce::File& file) {
		auto format = utils::findAudioFormat(file, false);
		if (!format) { return nullptr; }

		return std::unique_ptr<juce::AudioFormatReader>(format->createReaderFor(new juce::FileInputStream(file), true));
	}

	std::unique_ptr<juce::AudioFormatWriter> createAudioWriter(const juce::File& file,
		double sampleRateToUse, const juce::AudioChannelSet& channelLayout,
		const juce::StringPairArray& metaData, int bitDepth, int quality) {
		auto format = utils::findAudioFormat(file, true);
		if (!format) { return nullptr; }

		/** Limit Bit Depth To Solve MP3 Bit Depth Problem */
		auto bitDepths = format->getPossibleBitDepths();
		if (!bitDepths.contains(bitDepth)) {
			bitDepth = bitDepths.getLast();
		}

		auto outStream = new juce::FileOutputStream(file);
		if (outStream->openedOk()) {
			outStream->setPosition(0);
			outStream->truncate();
		}

		auto writer = format->createWriterFor(outStream,
			sampleRateToUse, channelLayout, bitDepth, metaData, quality);
		if (!writer) {
			delete outStream;
			file.deleteFile();
			return nullptr;
		}

		return std::unique_ptr<juce::AudioFormatWriter>(writer);
	}

	juce::StringArray getQualityOptionsForExtension(const juce::String& extension) {
		auto format = findAudioFormatForExtension(extension, true);
		if (!format) { return {}; }
		return format->getQualityOptions();
	}

	juce::Array<int> getPossibleBitDepthsForExtension(const juce::String& extension) {
		auto format = findAudioFormatForExtension(extension, true);
		if (!format) { return {}; }
		return format->getPossibleBitDepths();
	}

	juce::Array<int> getPossibleSampleRatesForExtension(const juce::String& extension) {
		auto format = findAudioFormatForExtension(extension, true);
		if (!format) { return {}; }
		return format->getPossibleSampleRates();
	}

	juce::StringArray getPossibleMetaKeyForExtension(const juce::String& extension) {
		auto format = findAudioFormatForExtension(extension, true);
		if (!format) { return {}; }
		juce::String formatName = format->getFormatName();

		if (formatName == "WAV file") {
			return WAV_META_KEYS;
		}
		else if (formatName == "AIFF file") {
			return AIFF_META_KEYS;
		}
		else if (formatName == "FLAC file") {
			return FLAC_META_KEYS;
		}
		else if (formatName == "MP3 file") {
			return MP3_META_KEYS;
		}
		else if (formatName == "Ogg-Vorbis file") {
			return OGG_META_KEYS;
		}

		return {};
	}

	int getBestQualityOptionIndexForExtension(const juce::String& extension) {
		auto format = findAudioFormatForExtension(extension, true);
		if (!format) { return {}; }
		juce::String formatName = format->getFormatName();

		if (formatName == "WAV file") {
			return 0;
		}
		else if (formatName == "AIFF file") {
			return 0;
		}
		else if (formatName == "FLAC file") {
			return 8;
		}
		else if (formatName == "MP3 file") {
			return 23;
		}
		else if (formatName == "Ogg-Vorbis file") {
			return 10;
		}

		return 0;
	}

	const AudioFormatInfo getAudioFormatData(const juce::File& file) {
		if (auto reader = createAudioReader(file)) {
			return AudioFormatInfo{
				reader->sampleRate,
				reader->bitsPerSample,
				reader->lengthInSamples,
				reader->numChannels,
				reader->usesFloatingPointData,
				reader->metadataValues
			};
		}
		return AudioFormatInfo{};
	}

	const juce::Array<TrackType> getAllTrackTypes() {
		return {
			TrackType::DISABLED,
			TrackType::MONO,
			TrackType::STEREO,
			TrackType::LCR,
			TrackType::LRS,
			TrackType::LCRS,
			TrackType::SUR_5_0,
			TrackType::SUR_5_1,
			TrackType::SUR_5_0_2,
			TrackType::SUR_5_1_2,
			TrackType::SUR_5_0_4,
			TrackType::SUR_5_1_4,
			TrackType::SUR_6_0,
			TrackType::SUR_6_1,
			TrackType::SUR_6_0_M,
			TrackType::SUR_6_1_M,
			TrackType::SUR_7_0,
			TrackType::SUR_7_0_SDSS,
			TrackType::SUR_7_1,
			TrackType::SUR_7_1_SDSS,
			TrackType::SUR_7_0_2,
			TrackType::SUR_7_1_2,
			TrackType::SUR_7_0_4,
			TrackType::SUR_7_1_4,
			TrackType::SUR_7_0_6,
			TrackType::SUR_7_1_6,
			TrackType::SUR_9_0_4,
			TrackType::SUR_9_1_4,
			TrackType::SUR_9_0_6,
			TrackType::SUR_9_1_6,
			TrackType::QUADRAPHONIC,
			TrackType::PENTAGONAL,
			TrackType::HEXAGONAL,
			TrackType::OCTAGONAL,
			TrackType::AMBISONIC_0,
			TrackType::AMBISONIC_1,
			TrackType::AMBISONIC_2,
			TrackType::AMBISONIC_3,
			TrackType::AMBISONIC_4,
			TrackType::AMBISONIC_5,
			TrackType::AMBISONIC_6,
			TrackType::AMBISONIC_7
		};
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

	Version getAudioProjectVersionMinimumSupported() {
		return Version{
			VS_PROJECT_MINIMUM_SUPPORTED_MAJOR_VERSION,
			VS_PROJECT_MINIMUM_SUPPORTED_MINOR_VERSION,
			VS_PROJECT_MINIMUM_SUPPORTED_PATCH_VERSION };
	}

	juce::String createVersionString(const Version& version) {
		auto& [major, minor, patch] = version;
		return juce::String(major) + "." + juce::String(minor) + "." + juce::String(patch);
	}

	juce::String getAudioPlatformVersionString() {
		return createVersionString(getAudioPlatformVersion());
	}

	juce::String getAudioPlatformCompileTime() {
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
			+ getAudioPlatformCompileTime() + "; "
			+ getReleaseBranch() + ", "
			+ getReleaseName() + "; "
			+ getSystemNameAndVersion();
	}

	juce::String getUserName() {
		return AudioConfig::getAnonymous()
			? "Anonymous" : juce::SystemStats::getLogonName();
	}

	juce::String getJUCEVersion() {
		return juce::SystemStats::getJUCEVersion();
	}

	juce::String getVST3SDKVersion() {
#if JUCE_PLUGINHOST_VST3
		return kVstVersionString;

#else //JUCE_PLUGINHOST_VST3
		return "";

#endif //JUCE_PLUGINHOST_VST3
	}

	juce::String getVST2SDKVersion() {
#if JUCE_PLUGINHOST_VST
		int version = kVstVersion;
		return juce::String{ version / 1000 } + "." + juce::String{ version % 1000 / 100 };

#else //JUCE_PLUGINHOST_VST
		return "";

#endif //JUCE_PLUGINHOST_VST
	}

	juce::String getARASDKVersion() {
#if JUCE_PLUGINHOST_ARA
		return ARA_VERSION;

#else //JUCE_PLUGINHOST_ARA
		return "";

#endif //JUCE_PLUGINHOST_ARA
	}

	juce::String getLV2Version() {
#if JUCE_PLUGINHOST_LV2
		return "LILV v" LILV_VERSION " SERD v" SERD_VERSION " SORD v" SORD_VERSION;

#else //JUCE_PLUGINHOST_LV2
		return "";

#endif //JUCE_PLUGINHOST_LV2
	}

	juce::String getCompilerVersion() {
#if JUCE_MSVC
		return juce::String{ "MSVC " } + juce::String{ (uint64_t)_MSC_FULL_VER };

#elif JUCE_GCC
		return juce::String{ "GCC " } + juce::String{ __GNUC__ } + "." + juce::String{ __GNUC_MINOR__ } + "." + juce::String{ __GNUC_PATCHLEVEL__ };

#elif JUCE_CLANG
		return juce::String{ "Clang " } + juce::String{ __clang_major__ } + "." + juce::String{ __clang_minor__ } + "." + juce::String{ __clang_patchlevel__ };

#else
		return "Unknown";
#endif
	}

	juce::String getLegalFileName(const juce::String& name) {
		return name.replaceCharacters(
			"\\/:*?\"<>|", "_________");
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
		if (!dir.isDirectory()) {
			dir.createDirectory();
		}
		projectDirectory = dir;
		return true;
	}

	juce::File getDefaultWorkingDir() {
		return juce::File::getSpecialLocation(
			juce::File::SpecialLocationType::userDocumentsDirectory)
			.getChildFile("./VocalShaperProjectTemp/");
	}

	juce::File getARADataDir(const juce::String& projectDir, const juce::String& projectFileName) {
		return juce::File{ projectDir }.getChildFile("./" + projectFileName + ".ara/");
	}

	juce::File getARADataFile(const juce::String& araDir, const juce::String& id) {
		return juce::File{ araDir }.getChildFile("./" + id + ".dat");
	}

	const juce::StringArray getProjectFormatsSupported(bool /*isWrite*/) {
		return juce::StringArray{ "*.vsp4" };
	}

	const juce::StringArray getPluginFormatsSupported() {
		juce::StringArray result{ "*.vst3", "*.lv2" };

#if JUCE_WINDOWS
		result.addArray({ "*.dll" });

#elif JUCE_LINUX
		result.addArray({ "*.so" });

#elif JUCE_MAC
		result.addArray({ "*.dylib", "*.component", "*.appex" });

#endif 

		return result;
	}

	const juce::Array<double> getSourceSampleRateSupported() {
		return { 8000, 11025, 16000, 22050, 44100, 48000, 
			88200, 96000, 176400, 192000, 352800, 384000 };
	}

	juce::Colour getDefaultColour() {
		return juce::Colour::fromRGB(98, 111, 252);
	}

	void bufferOutputResampledFixed(juce::AudioSampleBuffer& dst, const juce::AudioSampleBuffer& src,
		juce::AudioSampleBuffer& temp1, juce::AudioSampleBuffer& temp2,
		double resampleRatio, int channels, double dstSampleRate,
		int srcStart, int dstStart, int length) {
		/** Change Record Buffer */
		{
			/** Get Buffer Size */
			int rbSize = temp1.getNumSamples();
			int rbChannel = temp1.getNumChannels();

			/** Copy Old Data to Temp */
			if ((temp2.getNumSamples() != length) || (temp2.getNumChannels() != rbChannel)) {
				temp2.setSize(rbChannel, length, true, false, true);
			}
			if (rbSize < length * 3) {
				for (int i = 0; i < rbChannel; i++) {
					vMath::copyAudioData(temp2, temp1,
						length - rbSize / 3, length, i, i, rbSize / 3);
				}
			}
			else {
				for (int i = 0; i < rbChannel; i++) {
					vMath::copyAudioData(temp2, temp1,
						0, rbSize / 3, i, i, length);
				}
			}

			/** Change Buffer Size */
			if ((rbSize != length * 3) || (rbChannel != channels)) {
				temp1.setSize(channels, length * 3, true, true, true);
			}

			/** Copy Old Data */
			for (int i = 0; i < rbChannel; i++) {
				vMath::copyAudioData(temp1, temp2,
					0, 0, i, i, length);
			}

			/** Copy New Data */
			for (int i = 0; i < channels; i++) {
				vMath::copyAudioData(temp1, src,
					length, 0, i, i, length);
			}

			/** Fill Tails Data */
			for (int i = 0; i < channels; i++) {
				vMath::fillAudioData(temp1,
					src.getSample(i, srcStart + length - 1),
					length * 2, i, length);
			}
		}

		juce::MemoryAudioSource memSource(temp1, false, false);
		juce::ResamplingAudioSource resampleSource(&memSource, false, channels);
		memSource.setNextReadPosition(length + srcStart);
		resampleSource.setResamplingRatio(resampleRatio);
		int dstBufferSize = std::floor(length / resampleRatio);
		resampleSource.prepareToPlay(dstBufferSize, dstSampleRate);

		int dstStartSample = std::floor(dstStart / resampleRatio);
		if (dstStartSample < dst.getNumSamples()) {
			int dstClipSize = dstBufferSize;
			if (dstStartSample + dstClipSize >= dst.getNumSamples()) {
				dstClipSize = dst.getNumSamples() - dstStartSample;
				if (dstClipSize <= 0) {
					return;
				}
			}

			int trueStartSample = dstStartSample - 1;/** To Avoid sss Noise In Buffer Head */
			int trueLength = dstClipSize + 2;
			if (trueStartSample < 0) {
				trueStartSample = 0;
				trueLength = dstClipSize + 1;
			}
			trueLength = std::min(dst.getNumSamples() - trueStartSample, trueLength);

			if (dstClipSize > 0) {
				resampleSource.getNextAudioBlock(juce::AudioSourceChannelInfo{
					&dst, trueStartSample, trueLength  });
			}
		}
	}

	const juce::String getMIDICCChannelName(int channel) {
		return juce::MidiMessage::getControllerName(channel);
	}

	bool readFileToBlock(
		const juce::String& path, juce::MemoryBlock& block, const juce::File& base) {
		juce::File file = base.getChildFile(path);
		if (!file.existsAsFile()) {
			return false;
		}

		juce::FileInputStream stream(file);
		if (!stream.openedOk()) {
			return false;
		}

		block.setSize(stream.getTotalLength());
		if (!stream.read(block.getData(), block.getSize())) {
			return false;
		}
		return true;
	}

	bool writeBlockToFile(
		const juce::String& path, const juce::MemoryBlock& block, const juce::File& base) {
		juce::File file = base.getChildFile(path);

		juce::FileOutputStream stream(file);
		if (!stream.openedOk()) { return false; }
		stream.setPosition(0);
		stream.truncate();

		if (!stream.write(block.getData(), block.getSize())) {
			return false;
		}
		return true;
	}
}

#include "SourceIO.h"
#include "PlayPosition.h"
#include "AudioLock.h"
#include "../Utils.h"

SourceIO::SourceIO()
	: Thread("Source IO"),
	audioFormatsIn(SourceIO::trimFormat(utils::getAudioFormatsSupported(false))),
	midiFormatsIn(SourceIO::trimFormat(utils::getMidiFormatsSupported(false))),
	audioFormatsOut(SourceIO::trimFormat(utils::getAudioFormatsSupported(true))),
	midiFormatsOut(SourceIO::trimFormat(utils::getMidiFormatsSupported(true))) {}

SourceIO::~SourceIO() {
	this->stopThread(30000);
}

void SourceIO::addTask(const Task& task) {
	juce::GenericScopedLock locker(this->lock);
	this->list.push(task);
}

void SourceIO::run() {
	while (!this->threadShouldExit()) {
		/** Get Next Task */
		Task task;
		{
			juce::GenericScopedLock locker(this->lock);

			/** Check Empty */
			if (this->list.empty()) { break; }

			/** Dequque Task */
			task = this->list.front();
			this->list.pop();
		}

		/** Process Task */
		{
			/** Check Source Type */
			auto& [type, ptr, path, getTempo] = task;
			if (!ptr) { continue; }

			juce::File file = utils::getProjectDir().getChildFile(path);
			juce::String extension = file.getFileExtension();
			juce::String name = file.getFileNameWithoutExtension();
			
			auto& audioTypes = ((type == TaskType::Read) ? this->audioFormatsIn : this->audioFormatsOut);
			auto& midiTypes = ((type == TaskType::Read) ? this->midiFormatsIn : this->midiFormatsOut);

			if (audioTypes.contains(extension)) {
				if (type == TaskType::Read) {
					/** Load Audio Data */
					auto [sampleRate, buffer] = SourceIO::loadAudio(file);
					if (sampleRate <= 0) { continue; }

					/** Set Data */
					juce::MessageManager::callAsync(
						[sampleRate, buffer, name, ptr] {
							if (ptr) {
								ptr->setAudio(sampleRate, buffer, name);
							}
						}
					);
				}
				else if (type == TaskType::Write) {
					/** Get Data */
					double sampleRate = 0;
					juce::AudioSampleBuffer buffer;
					{
						juce::ScopedReadLock locker(audioLock::getSourceLock());
						if (ptr) {
							std::tie(sampleRate, buffer) = ptr->getAudio();
						}
					}
					if (sampleRate <= 0) { continue; }

					/** Save Audio Data */
					SourceIO::saveAudio(file, sampleRate, buffer);
				}
			}
			else if (midiTypes.contains(extension)) {
				if (type == TaskType::Read) {
					/** Load MIDI Data */
					auto [valid, data] = SourceIO::loadMIDI(file);
					if (!valid) { continue; }

					/** Split Data */
					auto [tempo, buffer] = SourceIO::splitMIDI(data);

					/** Set Tempo */
					if (getTempo) {
						juce::ScopedWriteLock locker(audioLock::getPositionLock());
						auto& tempoSeq = PlayPosition::getInstance()->getTempoSequence();
						tempoSeq.addSequence(tempo, 0);
					}

					/** Set Data */
					juce::MessageManager::callAsync(
						[buffer, name, ptr] {
							if (ptr) {
								ptr->setMIDI(buffer, name);
							}
						}
					);
				}
				else if (type == TaskType::Write) {
					/** Get Data */
					juce::MidiFile buffer;
					{
						juce::ScopedReadLock locker(audioLock::getSourceLock());
						if (ptr) {
							buffer = ptr->getMIDI();
						}
					}
					if (buffer.getNumTracks() <= 0) { continue; }

					/** Get Tempo */
					juce::MidiMessageSequence tempo;
					{
						juce::ScopedReadLock locker(audioLock::getPositionLock());
						tempo = PlayPosition::getInstance()->getTempoSequence();
					}

					/** Merge Data */
					auto data = SourceIO::mergeMIDI(buffer, tempo);

					/** Save Audio Data */
					SourceIO::saveMIDI(file, data);
				}
			}
		}
	}
}

const juce::StringArray SourceIO::trimFormat(const juce::StringArray& list) {
	juce::StringArray result;
	for (auto& s : list) {
		result.add(s.trimCharactersAtStart("*"));
	}
	return result;
}

const std::tuple<double, juce::AudioSampleBuffer> SourceIO::loadAudio(const juce::File& file) {
	/** Create Audio Reader */
	auto audioReader = utils::createAudioReader(file);
	if (!audioReader) { return { 0, juce::AudioSampleBuffer{} }; }

	/** Read Data */
	juce::AudioSampleBuffer buffer(
		(int)audioReader->numChannels, (int)audioReader->lengthInSamples);
	audioReader->read(&buffer, 0, audioReader->lengthInSamples, 0, true, true);

	return { audioReader->sampleRate, buffer };
}

const std::tuple<bool, juce::MidiFile> SourceIO::loadMIDI(const juce::File& file) {
	/** Create Input Stream */
	juce::FileInputStream stream(file);
	if (stream.failedToOpen()) { return { false, juce::MidiFile{} }; }

	/** Read MIDI File */
	juce::MidiFile midiFile;
	if (!midiFile.readFrom(stream, true)) { return { false, juce::MidiFile{} }; }

	/** Convert To Seconds */
	midiFile.convertTimestampTicksToSeconds();

	return { true, midiFile };
}

bool SourceIO::saveAudio(const juce::File& file,
	double sampleRate, const juce::AudioSampleBuffer& buffer) {
	/** Create Audio Writer */
	auto audioWriter = utils::createAudioWriter(file, sampleRate,
		juce::AudioChannelSet::canonicalChannelSet(buffer.getNumChannels()));
	if (!audioWriter) { return false; }

	/** Write Data */
	audioWriter->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());

	return true;
}

bool SourceIO::saveMIDI(const juce::File& file, const juce::MidiFile& data) {
	/** Create Output Stream */
	auto stream = std::make_unique<juce::FileOutputStream>(file);
	if (stream->failedToOpen()) { return false; }
	stream->setPosition(0);
	stream->truncate();

	/** Copy Data */
	juce::MidiFile midiFile{ data };
	utils::convertSecondsToTicks(midiFile);

	/** Write MIDI File */
	if (!midiFile.writeTo(*(stream.get()))) {
		stream = nullptr;
		file.deleteFile();
		return false;
	}

	return true;
}

const std::tuple<juce::MidiMessageSequence, juce::MidiFile>
SourceIO::splitMIDI(const juce::MidiFile& data) {
	juce::MidiFile result;
	SourceIO::copyMIDITimeFormat(result, data);

	juce::MidiMessageSequence timeSeq;
	for (int i = 0; i < data.getNumTracks(); i++) {
		juce::MidiMessageSequence seqTemp;
		auto seq = data.getTrack(i);

		for (auto j : *seq) {
			if (j->message.isTempoMetaEvent()
				|| j->message.isTimeSignatureMetaEvent()) {
				timeSeq.addEvent(j->message);
			}
			else {
				seqTemp.addEvent(j->message);
			}
		}

		if (seqTemp.getNumEvents() > 0) {
			result.addTrack(seqTemp);
		}
	}

	return { timeSeq, result };
}

const juce::MidiFile SourceIO::mergeMIDI(const juce::MidiFile& data,
	const juce::MidiMessageSequence& timeSeq) {
	juce::MidiFile result;
	SourceIO::copyMIDITimeFormat(result, data);

	result.addTrack(timeSeq);

	return result;
}

void SourceIO::copyMIDITimeFormat(juce::MidiFile& dst, const juce::MidiFile& src) {
	short timeFormat = src.getTimeFormat();
	if (timeFormat >= 0) {
		dst.setTicksPerQuarterNote(timeFormat);
	}
	else {
		auto p = (signed char*)(&timeFormat) ;
		int fps = -p[0];
		int tpf = p[1];
		dst.setSmpteTimeFormat(fps, tpf);
	}
}

SourceIO* SourceIO::getInstance() {
	return SourceIO::instance
		? SourceIO::instance : (SourceIO::instance = new SourceIO);
}

void SourceIO::releaseInstance() {
	if (SourceIO::instance) {
		delete SourceIO::instance;
		SourceIO::instance = nullptr;
	}
}

SourceIO* SourceIO::instance = nullptr;

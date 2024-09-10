#include "SourceInternalContainer.h"
#include "../misc/VMath.h"

SourceInternalContainer::SourceInternalContainer(
	const SourceType type, const juce::String& name)
	: type(type), name(name) {}

SourceInternalContainer::SourceInternalContainer(
	const SourceInternalContainer& other)
	: type(other.type), name(SourceInternalContainer::getForkName(other.name)), forked(true) {
	if (&other != this) {
		if (other.midiData) {
			this->midiData = std::make_unique<SourceMIDITemp>();
			this->midiData->setData(*(other.midiData->getSourceData()));
		}
		if (other.audioData) {
			this->audioData = std::make_unique<juce::AudioSampleBuffer>(*(other.audioData));
		}

		this->audioSampleRate = other.audioSampleRate;
		this->savedFlag = false;

		this->format = other.format;
		this->metaData = other.metaData;
		this->bitsPerSample = other.bitsPerSample;
		this->quality = other.quality;
	}
}

SourceInternalContainer::SourceType SourceInternalContainer::getType() const {
	return this->type;
}

const juce::String SourceInternalContainer::getName() const {
	return this->name;
}

juce::MidiFile* SourceInternalContainer::getMidiData() const {
	if (this->midiData) {
		return this->midiData->getSourceData();
	}
	return nullptr;
}

juce::AudioSampleBuffer* SourceInternalContainer::getAudioData() const {
	return this->audioData.get();
}

double SourceInternalContainer::getAudioSampleRate() const {
	return this->audioSampleRate;
}

void SourceInternalContainer::changed() {
	this->savedFlag = false;
}

void SourceInternalContainer::saved() {
	this->savedFlag = true;
}

bool SourceInternalContainer::isSaved() const {
	return this->savedFlag;
}

const juce::String SourceInternalContainer::getFormat() const {
	return this->format;
}

const juce::StringPairArray SourceInternalContainer::getMetaData() const {
	return this->metaData;
}

int SourceInternalContainer::getBitsPerSample() const {
	return this->bitsPerSample;
}

int SourceInternalContainer::getQuality() const {
	return this->quality;
}

void SourceInternalContainer::initMidiData() {
	if (this->type == SourceType::MIDI) {
		if (!this->midiData) {
			this->midiData = std::make_unique<SourceMIDITemp>();
		}

		this->midiData->addTrack(juce::MidiMessageSequence{});

		this->changed();
	}
}

void SourceInternalContainer::initAudioData(
	int channelNum, double sampleRate, double length) {
	if (this->type == SourceType::Audio) {
		this->audioData = std::make_unique<juce::AudioSampleBuffer>(
			channelNum, (int)std::ceil(length * sampleRate));
		vMath::zeroAllAudioData(*(this->audioData.get()));
		this->audioSampleRate = sampleRate;

		this->initAudioFormat();

		this->changed();
	}
}

void SourceInternalContainer::setMIDI(const juce::MidiFile& data) {
	if (this->type == SourceType::MIDI) {
		if (!this->midiData) {
			this->midiData = std::make_unique<SourceMIDITemp>();
		}
		this->midiData->setData(data);

		this->changed();
	}
}

void SourceInternalContainer::setAudio(
	double sampleRate, const juce::AudioSampleBuffer& data) {
	if (this->type == SourceType::Audio) {
		this->audioData = std::make_unique<juce::AudioSampleBuffer>(data);
		this->audioSampleRate = sampleRate;

		this->changed();
	}
}

void SourceInternalContainer::setAudioFormat(const AudioFormat& format) {
	if (this->type == SourceType::Audio) {
		std::tie(this->format, this->metaData, this->bitsPerSample, this->quality) = format;
	}
}

const SourceInternalContainer::AudioFormat SourceInternalContainer::getAudioFormat() const {
	if (this->type == SourceType::Audio) {
		return { this->format, this->metaData, this->bitsPerSample, this->quality };
	}
	return {};
}

bool SourceInternalContainer::isForked() const {
	return this->forked;
}

int SourceInternalContainer::getTrackNum() const {
	if (!this->midiData) { return 0; }
	return this->midiData->getTrackNum();
}

int SourceInternalContainer::getMIDINoteNum(int track) const {
	if (!this->midiData) { return 0; }
	return this->midiData->getNoteNum(track);
}

int SourceInternalContainer::getMIDISustainPedalNum(int track) const {
	if (!this->midiData) { return 0; }
	return this->midiData->getSustainPedalNum(track);
}

int SourceInternalContainer::getMIDISostenutoPedalNum(int track) const {
	if (!this->midiData) { return 0; }
	return this->midiData->getSostenutoPedalNum(track);
}

int SourceInternalContainer::getMIDISoftPedalNum(int track) const {
	if (!this->midiData) { return 0; }
	return this->midiData->getSoftPedalNum(track);
}

int SourceInternalContainer::getMIDIPitchWheelNum(int track) const {
	if (!this->midiData) { return 0; }
	return this->midiData->getPitchWheelNum(track);
}

int SourceInternalContainer::getMIDIAfterTouchNum(int track) const {
	if (!this->midiData) { return 0; }
	return this->midiData->getAfterTouchNum(track);
}

int SourceInternalContainer::getMIDIChannelPressureNum(int track) const {
	if (!this->midiData) { return 0; }
	return this->midiData->getChannelPressureNum(track);
}

const std::set<uint8_t> SourceInternalContainer::getMIDIControllerNumbers(int track) const {
	if (!this->midiData) { return {}; }
	return this->midiData->getControllerNumbers(track);
}

int SourceInternalContainer::getMIDIControllerNum(int track, uint8_t number) const {
	if (!this->midiData) { return 0; }
	return this->midiData->getControllerNum(track, number);
}

int SourceInternalContainer::getMIDIMiscNum(int track) const {
	if (!this->midiData) { return 0; }
	return this->midiData->getMiscNum(track);
}

const SourceMIDITemp::Note SourceInternalContainer::getMIDINote(int track, int index) const {
	if (!this->midiData) { return {}; }
	return this->midiData->getNote(track, index);
}

const SourceMIDITemp::Pedal SourceInternalContainer::getMIDISustainPedal(int track, int index) const {
	if (!this->midiData) { return {}; }
	return this->midiData->getSustainPedal(track, index);
}

const SourceMIDITemp::Pedal SourceInternalContainer::getMIDISostenutoPedal(int track, int index) const {
	if (!this->midiData) { return {}; }
	return this->midiData->getSostenutoPedal(track, index);
}

const SourceMIDITemp::Pedal SourceInternalContainer::getMIDISoftPedal(int track, int index) const {
	if (!this->midiData) { return {}; }
	return this->midiData->getSoftPedal(track, index);
}

const SourceMIDITemp::IntParam SourceInternalContainer::getMIDIPitchWheel(int track, int index) const {
	if (!this->midiData) { return {}; }
	return this->midiData->getPitchWheel(track, index);
}

const SourceMIDITemp::IntParam SourceInternalContainer::getMIDIAfterTouch(int track, int index) const {
	if (!this->midiData) { return {}; }
	return this->midiData->getAfterTouch(track, index);
}

const SourceMIDITemp::IntParam SourceInternalContainer::getMIDIChannelPressure(int track, int index) const {
	if (!this->midiData) { return {}; }
	return this->midiData->getChannelPressure(track, index);
}

const SourceMIDITemp::Controller SourceInternalContainer::getMIDIController(int track, uint8_t number, int index) const {
	if (!this->midiData) { return {}; }
	return this->midiData->getController(track, number, index);
}

const SourceMIDITemp::Misc SourceInternalContainer::getMIDIMisc(int track, int index) const {
	if (!this->midiData) { return {}; }
	return this->midiData->getMisc(track, index);
}

void SourceInternalContainer::initAudioFormat() {
	this->format.clear();
	this->metaData.clear();
	this->bitsPerSample = 0;
	this->quality = 0;
}

const juce::String SourceInternalContainer::getForkName(const juce::String& name) {
	auto file = juce::File::createFileWithoutCheckingPath(name);

	juce::String nameTemp = file.getFileNameWithoutExtension();
	int lastSplit = name.lastIndexOf("_");
	if (lastSplit > -1) {
		juce::String timeStr = name.substring(lastSplit + 1, name.length() - 1);
		if (juce::Time::fromISO8601(timeStr).toMilliseconds() != 0) {
			nameTemp = nameTemp.substring(0, lastSplit);
		}
	}
	nameTemp = nameTemp.trimCharactersAtEnd("_");

	return nameTemp + "_" + juce::Time::getCurrentTime().toISO8601(false) + file.getFileExtension();
}

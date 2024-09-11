#include "ARAObjects.h"
#include "../graph/SeqSourceProcessor.h"
#include "../source/SourceManager.h"
#include "../misc/AudioLock.h"
#include "../misc/PlayPosition.h"
#include "../AudioCore.h"

class GlobalMidiEventHelper {
	static MovablePlayHead* getPlayHead(SeqSourceProcessor* seq) {
		return dynamic_cast<MovablePlayHead*>(seq->getPlayHead());
	}

	static double getTotalTime() {
		return AudioCore::getInstance()->getGraph()->getTailLengthSeconds();
	}

public:
	using MidiEventType = ARAExtension::ARAContentType;
	
	static int32_t getCount(SeqSourceProcessor* seq) {
		if (auto ph = GlobalMidiEventHelper::getPlayHead(seq)) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			return ph->getTempoLabelNum() + 1;/**< Last Tempo Sync Point */
		}
		return 0;
	}
	static MidiEventType getType(SeqSourceProcessor* seq, int32_t index) {
		if (auto ph = GlobalMidiEventHelper::getPlayHead(seq)) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			if (index < ph->getTempoLabelNum()) {
				if (ph->isTempoLabelTempoEvent(index)) {
					return MidiEventType::ARAContentTypeTempoEntry;
				}
				else {
					return MidiEventType::ARAContentTypeBarSignature;
				}
			}
			else {
				/** Last Tempo Sync Point */
				return MidiEventType::ARAContentTypeTempoEntry;
			}
		}
		return MidiEventType::ARAContentTypeUnknown;
	}
	static ARA::ARAContentTempoEntry getTempoEvent(SeqSourceProcessor* seq, int32_t index) {
		if (auto ph = GlobalMidiEventHelper::getPlayHead(seq)) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			if (index < ph->getTempoLabelNum()) {
				double timeSec = ph->getTempoLabelTime(index);
				double timeQuarter = ph->toQuarter(timeSec);
				return { timeSec, timeQuarter };
			}
			else {
				/** Last Tempo Sync Point */
				double timeSec = 0;
				if (int num = ph->getTempoLabelNum()) {
					timeSec = ph->getTempoLabelTime(num - 1);
				}
				timeSec += 1;

				double timeQuarter = ph->toQuarter(timeSec);
				return { timeSec, timeQuarter };
			}
		}
		return {};
	}
	static ARA::ARAContentBarSignature getBarEvent(SeqSourceProcessor* seq, int32_t index) {
		if (auto ph = GlobalMidiEventHelper::getPlayHead(seq)) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			if (index < ph->getTempoLabelNum()) {
				auto beat = ph->getTempoLabelBeat(index);
				return { std::get<0>(beat), std::get<1>(beat), ph->getTempoLabelTime(index) };
			}
		}
		return {};
	}
	static ARA::ARAContentKeySignature getKeyEvent(SeqSourceProcessor* seq, int32_t index) {
		if (auto ph = GlobalMidiEventHelper::getPlayHead(seq)) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			/** Nothing To Do */
		}
		return {};
	}
};

ARAVirtualAudioSource::ARAVirtualAudioSource(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: seq(seq), audioSource(
		Converter::toHostRef(this), dc, ARAVirtualAudioSource::createProperties(seq)) {
	this->audioSource.enableAudioSourceSamplesAccess(true);
}

void ARAVirtualAudioSource::update() {
	this->audioSource.update(ARAVirtualAudioSource::createProperties(this->seq));
}

bool ARAVirtualAudioSource::readAudioSamples(
	float* const* buffers, int64_t startSample, int64_t numSamples) const {
	if (this->seq) {
		if (auto ref = seq->getAudioRef()) {
			juce::AudioSampleBuffer bufferTemp(
				buffers, seq->getAudioChannelSet().size(), (int)0, (int)numSamples);
			SourceManager::getInstance()->readAudioData(
				ref, bufferTemp, 0, startSample, numSamples);
			return true;
		}
	}
	return false;
}

int32_t ARAVirtualAudioSource::getGlobalMidiEventCount() {
	return GlobalMidiEventHelper::getCount(this->seq);
}

ARAVirtualAudioSource::MidiEventType
ARAVirtualAudioSource::getGlobalMidiEventType(int32_t index) {
	return GlobalMidiEventHelper::getType(this->seq, index);
}

ARA::ARAContentTempoEntry
ARAVirtualAudioSource::getGlobalTempoEvent(int32_t index) {
	return GlobalMidiEventHelper::getTempoEvent(this->seq, index);
}

ARA::ARAContentBarSignature
ARAVirtualAudioSource::getGlobalBarEvent(int32_t index) {
	return GlobalMidiEventHelper::getBarEvent(this->seq, index);
}

ARA::ARAContentKeySignature
ARAVirtualAudioSource::getGlobalKeyEvent(int32_t index) {
	return GlobalMidiEventHelper::getKeyEvent(this->seq, index);
}

const ARA::ARAAudioSourceProperties 
ARAVirtualAudioSource::createProperties(SeqSourceProcessor* seq) {
	auto properties = juce::ARAHostModel::AudioSource::getEmptyProperties();

	if (seq) {
		properties.name = seq->getAudioName().toRawUTF8();
		properties.persistentID = seq->getAudioName().toRawUTF8();
		properties.sampleCount = seq->getAudioLength() * seq->getSampleRate();
		properties.sampleRate = seq->getSampleRate();
		properties.channelCount = seq->getAudioChannelSet().size();
		properties.merits64BitSamples = seq->isUsingDoublePrecision();
	}

	return properties;
}

ARAVirtualMusicalContext::ARAVirtualMusicalContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq,
	ARAVirtualMusicalContextType type)
	: seq(seq), type(type), musicalContext(
		Converter::toHostRef(this), dc,
		ARAVirtualMusicalContext::createProperties(seq, &(this->color), type)) {}

const ARA::ARAMusicalContextProperties
ARAVirtualMusicalContext::createProperties(
	SeqSourceProcessor* seq, ARA::ARAColor* color, ARAVirtualMusicalContextType type) {
	auto properties = juce::ARAHostModel::MusicalContext::getEmptyProperties();

	if (seq) {
		properties.name = seq->getMIDIName().toRawUTF8();
		properties.orderIndex = (int32_t)type;

		auto seqColor = seq->getTrackColor();
		color->r = seqColor.getFloatRed();
		color->g = seqColor.getFloatGreen();
		color->b = seqColor.getFloatBlue();
		properties.color = color;
	}

	return properties;
}

void ARAVirtualMusicalContext::update() {
	this->musicalContext.update(ARAVirtualMusicalContext::createProperties(
		this->seq, &(this->color), this->type));
}

ARAVirtualMusicalContextType ARAVirtualMusicalContext::getType() const {
	return this->type;
}

SeqSourceProcessor* ARAVirtualMusicalContext::getSeq() const {
	return this->seq;
}

ARAVirtualGlobalContext::ARAVirtualGlobalContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAVirtualMusicalContextType::Global) {}

int32_t ARAVirtualGlobalContext::getEventCount() {
	return GlobalMidiEventHelper::getCount(this->getSeq());
}

ARAVirtualGlobalContext::MidiEventType
ARAVirtualGlobalContext::getEventType(int32_t index) {
	return GlobalMidiEventHelper::getType(this->getSeq(), index);
}

ARA::ARAContentTempoEntry
ARAVirtualGlobalContext::getTempo(int32_t index) {
	return GlobalMidiEventHelper::getTempoEvent(this->getSeq(), index);
}

ARA::ARAContentBarSignature
ARAVirtualGlobalContext::getBar(int32_t index) {
	return GlobalMidiEventHelper::getBarEvent(this->getSeq(), index);
}

ARA::ARAContentKeySignature
ARAVirtualGlobalContext::getKey(int32_t index) {
	return GlobalMidiEventHelper::getKeyEvent(this->getSeq(), index);
}

ARAVirtualNoteContext::ARAVirtualNoteContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAVirtualMusicalContextType::Note) {}

int32_t ARAVirtualNoteContext::getNoteCount() {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDINoteNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARA::ARAContentNote ARAVirtualNoteContext::getNote(int32_t index) {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		auto note = SourceManager::getInstance()->getMIDINote(
			ref, this->getSeq()->getCurrentMIDITrack(), index);

		ARA::ARAContentNote result{};
		result.frequency = (float)juce::MidiMessage::getMidiNoteInHertz(note.pitch);
		result.pitchNumber = note.pitch;
		result.volume = note.vel / (float)UINT8_MAX;
		result.startPosition = note.startSec;
		result.attackDuration = 0;
		result.noteDuration = note.endSec - note.startSec;
		result.signalDuration = result.noteDuration;

		return result;
	}
	return {};
}

ARAVirtualNotePlusContext::ARAVirtualNotePlusContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAVirtualMusicalContextType::NotePlus) {}

int32_t ARAVirtualNotePlusContext::getNoteCount() {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDINoteNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentNote ARAVirtualNotePlusContext::getNote(int32_t index) {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		auto note = SourceManager::getInstance()->getMIDINote(
			ref, this->getSeq()->getCurrentMIDITrack(), index);

		ARAExtension::ARAContentNote result{};
		result.channel = note.channel;
		result.startSec = note.startSec;
		result.endSec = note.endSec;
		result.pitch = note.pitch;
		result.vel = note.vel;
		result.lyrics = note.lyrics;

		return result;
	}
	return {};
}

ARAVirtualSustainPedalContext::ARAVirtualSustainPedalContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAVirtualMusicalContextType::SustainPedal) {}

int32_t ARAVirtualSustainPedalContext::getSustainPedalCount() {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDISustainPedalNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentPedal ARAVirtualSustainPedalContext::getSustainPedal(int32_t index) {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		auto pedal = SourceManager::getInstance()->getMIDISustainPedal(
			ref, this->getSeq()->getCurrentMIDITrack(), index);

		ARAExtension::ARAContentPedal result{};
		result.channel = pedal.channel;
		result.timeSec = pedal.timeSec;
		result.pedalOn = pedal.value;

		return result;
	}
	return {};
}

ARAVirtualSostenutoPedalContext::ARAVirtualSostenutoPedalContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAVirtualMusicalContextType::SostenutoPedal) {}

int32_t ARAVirtualSostenutoPedalContext::getSostenutoPedalCount() {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDISostenutoPedalNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentPedal ARAVirtualSostenutoPedalContext::getSostenutoPedal(int32_t index) {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		auto pedal = SourceManager::getInstance()->getMIDISostenutoPedal(
			ref, this->getSeq()->getCurrentMIDITrack(), index);

		ARAExtension::ARAContentPedal result{};
		result.channel = pedal.channel;
		result.timeSec = pedal.timeSec;
		result.pedalOn = pedal.value;

		return result;
	}
	return {};
}

ARAVirtualSoftPedalContext::ARAVirtualSoftPedalContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAVirtualMusicalContextType::SoftPedal) {}

int32_t ARAVirtualSoftPedalContext::getSoftPedalCount() {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDISoftPedalNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentPedal ARAVirtualSoftPedalContext::getSoftPedal(int32_t index) {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		auto pedal = SourceManager::getInstance()->getMIDISoftPedal(
			ref, this->getSeq()->getCurrentMIDITrack(), index);

		ARAExtension::ARAContentPedal result{};
		result.channel = pedal.channel;
		result.timeSec = pedal.timeSec;
		result.pedalOn = pedal.value;

		return result;
	}
	return {};
}

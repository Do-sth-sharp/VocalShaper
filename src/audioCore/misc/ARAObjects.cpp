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

public:
	static int32_t getTempoCount(SeqSourceProcessor* seq) {
		if (auto ph = GlobalMidiEventHelper::getPlayHead(seq)) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			return ph->getTempoTypeLabelNum() + 1;/**< Last Tempo Sync Point */
		}
		return 0;
	}
	static ARA::ARAContentTempoEntry getTempoEvent(SeqSourceProcessor* seq, int32_t index) {
		if (auto ph = GlobalMidiEventHelper::getPlayHead(seq)) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			if (index < ph->getTempoTypeLabelNum()) {
				int labelIndex = ph->getTempoTypeLabelIndex(index);
				if (labelIndex > -1) {
					double timeSec = ph->getTempoLabelTime(labelIndex);
					double timeQuarter = ph->toQuarter(timeSec);
					return { timeSec, timeQuarter };
				}
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
	static int32_t getBarCount(SeqSourceProcessor* seq) {
		if (auto ph = GlobalMidiEventHelper::getPlayHead(seq)) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			return ph->getBeatTypeLabelNum();
		}
		return 0;
	}
	static ARA::ARAContentBarSignature getBarEvent(SeqSourceProcessor* seq, int32_t index) {
		if (auto ph = GlobalMidiEventHelper::getPlayHead(seq)) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			int labelIndex = ph->getBeatTypeLabelIndex(index);
			if (labelIndex > -1) {
				auto beat = ph->getTempoLabelBeat(labelIndex);
				return { std::get<0>(beat), std::get<1>(beat), ph->getTempoLabelTime(index) };
			}
		}
		return {};
	}
	static int32_t getKeyCount(SeqSourceProcessor* seq) {
		/** Nothing To Do */
		/*if (auto ph = GlobalMidiEventHelper::getPlayHead(seq)) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			return 0;
		}*/
		return 0;
	}
	static ARA::ARAContentKeySignature getKeyEvent(SeqSourceProcessor* seq, int32_t index) {
		/** Nothing To Do */
		/*if (auto ph = GlobalMidiEventHelper::getPlayHead(seq)) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			return {};
		}*/
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
	ARAExtension::ARAContentType type)
	: seq(seq), type(type), musicalContext(
		Converter::toHostRef(this), dc,
		ARAVirtualMusicalContext::createProperties(seq, &(this->color), type)) {}

const ARA::ARAMusicalContextProperties
ARAVirtualMusicalContext::createProperties(
	SeqSourceProcessor* seq, ARA::ARAColor* color, ARAExtension::ARAContentType type) {
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

ARAExtension::ARAContentType ARAVirtualMusicalContext::getType() const {
	return this->type;
}

SeqSourceProcessor* ARAVirtualMusicalContext::getSeq() const {
	return this->seq;
}

ARAVirtualTempoContext::ARAVirtualTempoContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAExtension::ARAContentTypeTempoEntry) {}

int32_t ARAVirtualTempoContext::getEventCount() {
	return GlobalMidiEventHelper::getTempoCount(this->getSeq());
}

ARA::ARAContentTempoEntry
ARAVirtualTempoContext::getTempo(int32_t index) {
	return GlobalMidiEventHelper::getTempoEvent(this->getSeq(), index);
}

ARAVirtualBarContext::ARAVirtualBarContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAExtension::ARAContentTypeBarSignature) {}

int32_t ARAVirtualBarContext::getEventCount() {
	return GlobalMidiEventHelper::getBarCount(this->getSeq());
}

ARA::ARAContentBarSignature
ARAVirtualBarContext::getBar(int32_t index) {
	return GlobalMidiEventHelper::getBarEvent(this->getSeq(), index);
}

ARAVirtualKeyContext::ARAVirtualKeyContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAExtension::ARAContentTypeKeySignature) {}

int32_t ARAVirtualKeyContext::getEventCount() {
	return GlobalMidiEventHelper::getKeyCount(this->getSeq());
}

ARA::ARAContentKeySignature
ARAVirtualKeyContext::getKey(int32_t index) {
	return GlobalMidiEventHelper::getKeyEvent(this->getSeq(), index);
}

ARAVirtualNoteContext::ARAVirtualNoteContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAExtension::ARAContentTypeNote) {}

int32_t ARAVirtualNoteContext::getEventCount() {
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
	: ARAVirtualMusicalContext(dc, seq, ARAExtension::ARAContentTypeNotePlus) {}

int32_t ARAVirtualNotePlusContext::getEventCount() {
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
	: ARAVirtualMusicalContext(dc, seq, ARAExtension::ARAContentTypeSustainPedal) {}

int32_t ARAVirtualSustainPedalContext::getEventCount() {
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
	: ARAVirtualMusicalContext(dc, seq, ARAExtension::ARAContentTypeSostenutoPedal) {}

int32_t ARAVirtualSostenutoPedalContext::getEventCount() {
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
	: ARAVirtualMusicalContext(dc, seq, ARAExtension::ARAContentTypeSoftPedal) {}

int32_t ARAVirtualSoftPedalContext::getEventCount() {
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

ARAVirtualPitchWheelContext::ARAVirtualPitchWheelContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAExtension::ARAContentTypePitchWheel) {}

int32_t ARAVirtualPitchWheelContext::getEventCount() {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDIPitchWheelNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentIntParam ARAVirtualPitchWheelContext::getPitchWheel(int32_t index) {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		auto param = SourceManager::getInstance()->getMIDIPitchWheel(
			ref, this->getSeq()->getCurrentMIDITrack(), index);

		ARAExtension::ARAContentIntParam result{};
		result.channel = param.channel;
		result.timeSec = param.timeSec;
		result.value = param.value;

		return result;
	}
	return {};
}

ARAVirtualAfterTouchContext::ARAVirtualAfterTouchContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAExtension::ARAContentTypeAfterTouch) {}

int32_t ARAVirtualAfterTouchContext::getEventCount() {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDIAfterTouchNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentAfterTouch ARAVirtualAfterTouchContext::getAfterTouch(int32_t index) {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		auto param = SourceManager::getInstance()->getMIDIAfterTouch(
			ref, this->getSeq()->getCurrentMIDITrack(), index);

		ARAExtension::ARAContentAfterTouch result{};
		result.channel = param.channel;
		result.timeSec = param.timeSec;
		result.notePitch = param.notePitch;
		result.value = param.value;

		return result;
	}
	return {};
}

ARAVirtualChannelPressureContext::ARAVirtualChannelPressureContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAExtension::ARAContentTypeChannelPressure) {}

int32_t ARAVirtualChannelPressureContext::getEventCount() {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDIChannelPressureNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentIntParam ARAVirtualChannelPressureContext::getChannelPressure(int32_t index) {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		auto param = SourceManager::getInstance()->getMIDIChannelPressure(
			ref, this->getSeq()->getCurrentMIDITrack(), index);

		ARAExtension::ARAContentIntParam result{};
		result.channel = param.channel;
		result.timeSec = param.timeSec;
		result.value = param.value;

		return result;
	}
	return {};
}

ARAVirtualControllerContext::ARAVirtualControllerContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAExtension::ARAContentTypeController) {}

int32_t ARAVirtualControllerContext::getEventCount() {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		auto controllerSet = SourceManager::getInstance()->getMIDIControllerNumbers(
			ref, this->getSeq()->getCurrentMIDITrack());

		int32_t total = 0;
		for (auto i : controllerSet) {
			total += SourceManager::getInstance()->getMIDIControllerNum(
				ref, this->getSeq()->getCurrentMIDITrack(), i);
		}

		return total;
	}
	return 0;
}

ARAExtension::ARAContentController ARAVirtualControllerContext::getController(int32_t index) {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		auto controllerSet = SourceManager::getInstance()->getMIDIControllerNumbers(
			ref, this->getSeq()->getCurrentMIDITrack());

		int32_t total = 0;
		for (auto i : controllerSet) {
			int num = SourceManager::getInstance()->getMIDIControllerNum(
				ref, this->getSeq()->getCurrentMIDITrack(), i);

			if (index >= total && index < (total + num)) {
				auto controller = SourceManager::getInstance()->getMIDIController(
					ref, this->getSeq()->getCurrentMIDITrack(), i, index);

				ARAExtension::ARAContentController result{};
				result.channel = controller.channel;
				result.timeSec = controller.timeSec;
				result.number = controller.number;
				result.value = controller.value;

				return result;
			}

			total += num;
		}
	}
	return {};
}

ARAVirtualMiscContext::ARAVirtualMiscContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: ARAVirtualMusicalContext(dc, seq, ARAExtension::ARAContentTypeMisc) {}

int32_t ARAVirtualMiscContext::getEventCount() {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDIMiscNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentMisc ARAVirtualMiscContext::getMisc(int32_t index) {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		auto data = SourceManager::getInstance()->getMIDIMisc(
			ref, this->getSeq()->getCurrentMIDITrack(), index);

		ARAExtension::ARAContentMisc result{};
		result.channel = data.channel;
		result.timeSec = data.timeSec;
		result.message = data.message;

		return result;
	}
	return {};
}

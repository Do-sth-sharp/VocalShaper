#include "ARAObjects.h"
#include "../graph/SeqSourceProcessor.h"
#include "../source/SourceManager.h"
#include "../misc/PlayPosition.h"
#include "../misc/AudioLock.h"
#include "../AudioCore.h"

class GlobalMidiEventHelper {
	static MovablePlayHead* getPlayHead() {
		return PlayPosition::getInstance();
	}

public:
	static int32_t getTempoCount() {
		if (auto ph = GlobalMidiEventHelper::getPlayHead()) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());

			/** At Least 2 Tempo Sync Points */
			if (ph->getTempoTypeLabelNum() < 1) {
				return 2;
			}

			return ph->getTempoTypeLabelNum() + 1;/**< Last Tempo Sync Point */
		}
		return 0;
	}
	static ARA::ARAContentTempoEntry getTempoEvent(int32_t index) {
		if (auto ph = GlobalMidiEventHelper::getPlayHead()) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			if (index < ph->getTempoTypeLabelNum()) {
				int labelIndex = ph->getTempoTypeLabelIndex(index);
				if (labelIndex > -1) {
					double timeSec = ph->getTempoLabelTime(labelIndex);
					double timeQuarter = ph->toQuarter(timeSec);
					return { timeSec, timeQuarter };
				}
			}
			else if (index == 0) {
				return { 0, 0 };
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
		return { 0, 0 };
	}
	static int32_t getBarCount() {
		if (auto ph = GlobalMidiEventHelper::getPlayHead()) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			return std::max(ph->getBeatTypeLabelNum(), 1);
		}
		return 0;
	}
	static ARA::ARAContentBarSignature getBarEvent(int32_t index) {
		if (auto ph = GlobalMidiEventHelper::getPlayHead()) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			int labelIndex = ph->getBeatTypeLabelIndex(index);
			if (labelIndex > -1) {
				auto beat = ph->getTempoLabelBeat(labelIndex);
				return { std::get<0>(beat), std::get<1>(beat), ph->getTempoLabelTime(index) };
			}
		}
		return { 4, 4, 0 };
	}
	static int32_t getKeyCount() {
		/** Nothing To Do */
		/*if (auto ph = GlobalMidiEventHelper::getPlayHead()) {
			juce::ScopedReadLock locker(audioLock::getPositionLock());
			return 0;
		}*/
		return 0;
	}
	static ARA::ARAContentKeySignature getKeyEvent(int32_t index) {
		/** Nothing To Do */
		/*if (auto ph = GlobalMidiEventHelper::getPlayHead()) {
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
	this->audioSource.enableAudioSourceSamplesAccess(false);
	this->audioSource.update(ARAVirtualAudioSource::createProperties(this->seq));
	this->audioSource.enableAudioSourceSamplesAccess(true);
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

double ARAVirtualAudioSource::getLength() const {
	if (this->seq) {
		return this->seq->getAudioLength();
	}
	return 0;
}

juce::ARAHostModel::AudioSource& ARAVirtualAudioSource::getProperties() {
	return this->audioSource;
}

const char* ARAVirtualAudioSource::defaultID = "-";

const ARA::ARAAudioSourceProperties 
ARAVirtualAudioSource::createProperties(SeqSourceProcessor* seq) {
	auto properties = juce::ARAHostModel::AudioSource::getEmptyProperties();

	if (seq) {
		properties.name = seq->getAudioName().toRawUTF8();
		properties.persistentID = ARAVirtualAudioSource::defaultID;
		properties.sampleRate = seq->isSourceInfoValid()
			? seq->getAudioSampleRateTemped()
			: seq->getAudioSampleRate();
		if (properties.sampleRate <= 0) {
			properties.sampleRate = seq->getSampleRate();
		}
		properties.sampleCount = std::max((uint64_t)(
			(seq->isSourceInfoValid() ? seq->getAudioLengthTemped() : seq->getAudioLength())
			* properties.sampleRate), (uint64_t)2);/**< At Least 2 Samples In Audio Source */
		properties.channelCount = seq->getAudioChannelSet().size();
		properties.merits64BitSamples = seq->isUsingDoublePrecision();
	}

	properties.channelArrangementDataType = 0;
	properties.channelArrangement = nullptr;

	return properties;
}

ARAVirtualMusicalContext::ARAVirtualMusicalContext(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq)
	: seq(seq), musicalContext(
		Converter::toHostRef(this), dc,
		ARAVirtualMusicalContext::createProperties(seq, &(this->color))) {}

void ARAVirtualMusicalContext::update() {
	this->musicalContext.update(ARAVirtualMusicalContext::createProperties(
		this->seq, &(this->color)));
}

double ARAVirtualMusicalContext::getLength() const {
	if (this->seq) {
		return this->seq->getMIDILength();
	}
	return 0;
}

juce::ARAHostModel::MusicalContext& ARAVirtualMusicalContext::getProperties() {
	return this->musicalContext;
}

int32_t ARAVirtualMusicalContext::getTempoCount() const {
	return GlobalMidiEventHelper::getTempoCount();
}

ARA::ARAContentTempoEntry ARAVirtualMusicalContext::getTempo(int32_t index) const {
	return GlobalMidiEventHelper::getTempoEvent(index);
}

int32_t ARAVirtualMusicalContext::getBarCount() const {
	return GlobalMidiEventHelper::getBarCount();
}

ARA::ARAContentBarSignature ARAVirtualMusicalContext::getBar(int32_t index) const {
	return GlobalMidiEventHelper::getBarEvent(index);
}

int32_t ARAVirtualMusicalContext::getKeyCount() const {
	return GlobalMidiEventHelper::getKeyCount();
}

ARA::ARAContentKeySignature ARAVirtualMusicalContext::getKey(int32_t index) const {
	return GlobalMidiEventHelper::getKeyEvent(index);
}

int32_t ARAVirtualMusicalContext::getNoteCount() const {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDINoteNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARA::ARAContentNote ARAVirtualMusicalContext::getNote(int32_t index) const {
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

int32_t ARAVirtualMusicalContext::getNotePlusCount() const {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDINoteNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentNote ARAVirtualMusicalContext::getNotePlus(int32_t index) const {
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

int32_t ARAVirtualMusicalContext::getSustainPedalCount() const {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDISustainPedalNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentPedal ARAVirtualMusicalContext::getSustainPedal(int32_t index) const {
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

int32_t ARAVirtualMusicalContext::getSostenutoPedalCount() const {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDISostenutoPedalNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentPedal ARAVirtualMusicalContext::getSostenutoPedal(int32_t index) const {
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

int32_t ARAVirtualMusicalContext::getSoftPedalCount() const {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDISoftPedalNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentPedal ARAVirtualMusicalContext::getSoftPedal(int32_t index) const {
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

int32_t ARAVirtualMusicalContext::getPitchWheelCount() const {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDIPitchWheelNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentIntParam ARAVirtualMusicalContext::getPitchWheel(int32_t index) const {
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

int32_t ARAVirtualMusicalContext::getAfterTouchCount() const {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDIAfterTouchNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentAfterTouch ARAVirtualMusicalContext::getAfterTouch(int32_t index) const {
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

int32_t ARAVirtualMusicalContext::getChannelPressureCount() const {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDIChannelPressureNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentIntParam ARAVirtualMusicalContext::getChannelPressure(int32_t index) const {
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

int32_t ARAVirtualMusicalContext::getControllerCount() const {
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

ARAExtension::ARAContentController ARAVirtualMusicalContext::getController(int32_t index) const {
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

int32_t ARAVirtualMusicalContext::getMiscCount() const {
	if (auto ref = this->getSeq()->getMIDIRef()) {
		return SourceManager::getInstance()->getMIDIMiscNum(
			ref, this->getSeq()->getCurrentMIDITrack());
	}
	return 0;
}

ARAExtension::ARAContentMisc ARAVirtualMusicalContext::getMisc(int32_t index) const {
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

SeqSourceProcessor* ARAVirtualMusicalContext::getSeq() const {
	return this->seq;
}

const ARA::ARAMusicalContextProperties
ARAVirtualMusicalContext::createProperties(
	SeqSourceProcessor* seq, ARA::ARAColor* color) {
	auto properties = juce::ARAHostModel::MusicalContext::getEmptyProperties();

	properties.orderIndex = 0;

	if (seq) {
		properties.name = seq->getMIDIName().toRawUTF8();
		
		auto seqColor = seq->getTrackColor();
		color->r = seqColor.getFloatRed();
		color->g = seqColor.getFloatGreen();
		color->b = seqColor.getFloatBlue();
		properties.color = color;
	}

	return properties;
}

ARAVirtualAudioModification::ARAVirtualAudioModification(
	ARA::Host::DocumentController& dc,
	ARAVirtualAudioSource& source)
	: source(source), audioModification(
		Converter::toHostRef(this), dc, source.getProperties(),
		ARAVirtualAudioModification::createProperties()) {}

ARAVirtualAudioSource& ARAVirtualAudioModification::getSource() {
	return this->source;
}

juce::ARAHostModel::AudioModification& ARAVirtualAudioModification::getProperties() {
	return this->audioModification;
}

const char* ARAVirtualAudioModification::defaultID = "-";

const ARA::ARAAudioModificationProperties
ARAVirtualAudioModification::createProperties() {
	auto properties = juce::ARAHostModel::AudioModification::getEmptyProperties();

	properties.name = nullptr;
	properties.persistentID = ARAVirtualAudioModification::defaultID;

	return properties;
}

juce::ARAHostModel::RegionSequence& ARAVirtualRegionSequence::getProperties() {
	return this->regionSequence;
}

SeqSourceProcessor* ARAVirtualRegionSequence::getSeq() const {
	return this->seq;
}

ARAVirtualRegionSequence::ARAVirtualRegionSequence(
	ARA::Host::DocumentController& dc,
	SeqSourceProcessor* seq,
	ARAVirtualMusicalContext& context)
	: seq(seq), context(context), regionSequence(Converter::toHostRef(this), dc,
		ARAVirtualRegionSequence::createProperties(seq, &(this->color), context)) {}

void ARAVirtualRegionSequence::update() {
	this->regionSequence.update(
		ARAVirtualRegionSequence::createProperties(
			this->seq, &(this->color), this->context));
}

ARAVirtualMusicalContext& ARAVirtualRegionSequence::getContext() {
	return this->context;
}

const ARA::ARARegionSequenceProperties
ARAVirtualRegionSequence::createProperties(
	SeqSourceProcessor* seq, ARA::ARAColor* color, ARAVirtualMusicalContext& context) {
	auto properties = juce::ARAHostModel::RegionSequence::getEmptyProperties();

	if (seq) {
		auto trackName = seq->getTrackName();
		properties.name = trackName.isNotEmpty() ? trackName.toRawUTF8() : "VocalShaper Track";

		auto seqColor = seq->getTrackColor();
		color->r = seqColor.getFloatRed();
		color->g = seqColor.getFloatGreen();
		color->b = seqColor.getFloatBlue();
		properties.color = color;
	}
	properties.orderIndex = 0;
	properties.musicalContextRef = context.getProperties().getPluginRef();

	return properties;
}

ARAVirtualPlaybackRegion::ARAVirtualPlaybackRegion(
	TimeRangeMap time,
	ARA::Host::DocumentController& dc,
	ARAVirtualRegionSequence& sequence,
	ARAVirtualAudioModification& modification)
	: time(time), sequence(sequence), modification(modification),
	playbackRegion(Converter::toHostRef(this), dc, modification.getProperties(),
		ARAVirtualPlaybackRegion::createProperties(
			time, sequence, modification, &(this->color))) {}

ARAVirtualAudioModification& ARAVirtualPlaybackRegion::getModification() {
	return this->modification;
}

juce::ARAHostModel::PlaybackRegion& ARAVirtualPlaybackRegion::getProperties() {
	return this->playbackRegion;
}

void ARAVirtualPlaybackRegion::update() {
	this->playbackRegion.update(
		ARAVirtualPlaybackRegion::createProperties(
			this->time, this->sequence, this->modification, &(this->color)));
}

const ARA::ARAPlaybackRegionProperties
ARAVirtualPlaybackRegion::createProperties(
	TimeRangeMap time,
	ARAVirtualRegionSequence& sequence,
	ARAVirtualAudioModification& modification,
	ARA::ARAColor* color) {
	auto properties = juce::ARAHostModel::PlaybackRegion::getEmptyProperties();

	auto [seqStartTime, contextStartTime, length] = time;

	properties.transformationFlags = ARA::kARAPlaybackTransformationNoChanges;
	properties.startInModificationTime = contextStartTime;
	properties.durationInModificationTime = length;
	properties.startInPlaybackTime = seqStartTime;
	properties.durationInPlaybackTime = length;
	properties.musicalContextRef = sequence.getContext().getProperties().getPluginRef();
	properties.regionSequenceRef = sequence.getProperties().getPluginRef();

	if (sequence.getSeq()) {
		auto trackName = sequence.getSeq()->getTrackName();
		properties.name = trackName.isNotEmpty() ? trackName.toRawUTF8() : "VocalShaper Region";

		auto seqColor = sequence.getSeq()->getTrackColor();
		color->r = seqColor.getFloatRed();
		color->g = seqColor.getFloatGreen();
		color->b = seqColor.getFloatBlue();
		properties.color = color;
	}

	return properties;
}

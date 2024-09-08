﻿#include "ARAObjects.h"
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
	using MidiEventType = ARAVirtualMidiEventType;
	
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
					return MidiEventType::TempoEntry;
				}
				else {
					return MidiEventType::BarSignature;
				}
			}
			else {
				/** Last Tempo Sync Point */
				return MidiEventType::TempoEntry;
			}
		}
		return MidiEventType::Unknown;
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
	SeqSourceProcessor* seq)
	: seq(seq), musicalContext(
		Converter::toHostRef(this), dc, ARAVirtualMusicalContext::createProperties(seq, &(this->color))) {}

void ARAVirtualMusicalContext::update() {
	this->musicalContext.update(ARAVirtualMusicalContext::createProperties(this->seq, &(this->color)));
}

int32_t ARAVirtualMusicalContext::getGlobalMidiEventCount() {
	return GlobalMidiEventHelper::getCount(this->seq);
}

ARAVirtualMusicalContext::MidiEventType
ARAVirtualMusicalContext::getGlobalMidiEventType(int32_t index) {
	return GlobalMidiEventHelper::getType(this->seq, index);
}

ARA::ARAContentTempoEntry
ARAVirtualMusicalContext::getGlobalTempoEvent(int32_t index) {
	return GlobalMidiEventHelper::getTempoEvent(this->seq, index);
}

ARA::ARAContentBarSignature
ARAVirtualMusicalContext::getGlobalBarEvent(int32_t index) {
	return GlobalMidiEventHelper::getBarEvent(this->seq, index);
}

ARA::ARAContentKeySignature
ARAVirtualMusicalContext::getGlobalKeyEvent(int32_t index) {
	return GlobalMidiEventHelper::getKeyEvent(this->seq, index);
}

const ARA::ARAMusicalContextProperties
ARAVirtualMusicalContext::createProperties(SeqSourceProcessor* seq, ARA::ARAColor* color) {
	auto properties = juce::ARAHostModel::MusicalContext::getEmptyProperties();

	if (seq) {
		properties.name = seq->getMIDIName().toRawUTF8();
		properties.orderIndex = 0;

		auto seqColor = seq->getTrackColor();
		color->r = seqColor.getFloatRed();
		color->g = seqColor.getFloatGreen();
		color->b = seqColor.getFloatBlue();
		properties.color = color;
	}

	return properties;
}

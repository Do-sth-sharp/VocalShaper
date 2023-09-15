#include "SeqSourceProcessor.h"
#include "../misc/PlayPosition.h"
#include "../source/CloneableSourceManager.h"
#include "../source/CloneableAudioSource.h"
#include "../source/CloneableMIDISource.h"
#include "../source/CloneableSynthSource.h"

SeqSourceProcessor::SeqSourceProcessor(const juce::AudioChannelSet& type)
	: audioChannels(type) {
	/** Set Channel Layout */
	this->setChannelLayoutOfBus(true, 0, type);
	this->setChannelLayoutOfBus(false, 0, type);
}

bool SeqSourceProcessor::addSeq(const SourceList::SeqBlock& block) {
	return this->srcs.add(block);
}

void SeqSourceProcessor::removeSeq(int index) {
	this->srcs.remove(index);
}

int SeqSourceProcessor::getSeqNum() const {
	return this->srcs.size();
}

const SourceList::SeqBlock SeqSourceProcessor::getSeq(int index) const {
	juce::GenericScopedLock locker(this->srcs.getLock());
	return this->srcs.get(index);
}

void SeqSourceProcessor::closeAllNote() {
	this->noteCloseFlag = true;
}

void SeqSourceProcessor::prepareToPlay(
	double /*sampleRate*/, int /*maximumExpectedSamplesPerBlock*/) {}

void SeqSourceProcessor::processBlock(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	/** Check Buffer Is Empty */
	if (buffer.getNumSamples() <= 0) { return; }

	/** Clear Audio Channel */
	auto dspBlock = juce::dsp::AudioBlock<float>(buffer).getSubsetChannelBlock(
		0, buffer.getNumChannels());
	dspBlock.fill(0);

	/** Clear MIDI Buffer */
	midiMessages.clear();

	/** Close Note */
	if (this->noteCloseFlag) {
		this->noteCloseFlag = false;

		for (auto& i : this->activeNoteSet) {
			midiMessages.addEvent(
				juce::MidiMessage::noteOff(std::get<0>(i), std::get<1>(i)), 0);
		}
	}

	/** Get Play Head */
	auto playHead = dynamic_cast<PlayPosition*>(this->getPlayHead());
	if (!playHead) { return; }

	/** Get Current Position */
	juce::Optional<juce::AudioPlayHead::PositionInfo> position = playHead->getPosition();
	if (!position) { return; }

	/** Check Play State */
	if (!position->getIsPlaying()) { return; }

	/** Get Time */
	double startTime = position->getTimeInSeconds().orFallback(-1);
	double sampleRate = this->getSampleRate();
	double duration = buffer.getNumSamples() / sampleRate;
	double endTime = startTime + duration;

	/** Copy Source Data */
	{
		juce::ScopedTryReadLock managerLocker(CloneableSourceManager::getInstance()->getLock());
		if (managerLocker.isLocked()) {
			juce::GenericScopedLock srcLocker(this->srcs.getLock());

			/** Find Hot Block */
			auto index = this->srcs.match(startTime, endTime);

			for (int i = std::get<0>(index);
				i <= std::get<1>(index) && i < this->srcs.size() && i >= 0; i++) {
				/** Get Block */
				auto block = this->srcs.getUnchecked(i);

				if (CloneableSource::SafePointer<> ptr = std::get<3>(block)) {
					/** Caculate Time */
					double dataStartTime = std::get<0>(block) + std::max(std::get<2>(block), 0.);
					double dataEndTime =
						std::min(std::get<1>(block), std::get<0>(block) + std::get<2>(block) + ptr->getSourceLength());

					if (dataEndTime > dataStartTime) {
						double hotStartTime = std::max(startTime, dataStartTime);
						double hotEndTime = std::min(endTime, dataEndTime);

						if (hotEndTime > hotStartTime) {
							if (auto p = dynamic_cast<CloneableAudioSource*>(ptr.getSource())) {
								/** Copy Audio Data */
								p->readData(buffer,
									hotStartTime - startTime,
									hotStartTime - (std::get<0>(block) + std::get<2>(block)),
									hotEndTime - hotStartTime);
							}
							else if (auto p = dynamic_cast<CloneableSynthSource*>(ptr.getSource())) {
								/** Copy Audio Data */
								p->readData(buffer,
									hotStartTime - startTime,
									hotStartTime - (std::get<0>(block) + std::get<2>(block)),
									hotEndTime - hotStartTime);
							}
							else if (auto p = dynamic_cast<CloneableMIDISource*>(ptr.getSource())) {
								/** Copy MIDI Message */
								double dataTime = std::get<0>(block) + std::get<2>(block);
								p->readData(midiMessages,
									dataTime - startTime,
									hotStartTime - dataTime,
									hotEndTime - dataTime);
							}
						}
					}
				}
			}
		}
	}

	/** Set Note State */
	for (auto i : midiMessages) {
		auto mes = i.getMessage();
		if (mes.isNoteOn(false)) {
			this->activeNoteSet.insert({ mes.getChannel(), mes.getNoteNumber() });
		}
		else if (mes.isNoteOff(true)) {
			this->activeNoteSet.erase({ mes.getChannel(), mes.getNoteNumber() });
		}
	}
}

double SeqSourceProcessor::getTailLengthSeconds() const {
	juce::GenericScopedLock locker(this->srcs.getLock());

	int size = this->srcs.size();
	return (size > 0) ? std::get<1>(this->srcs.getUnchecked(size - 1)) : 0;
}

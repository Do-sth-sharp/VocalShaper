#include "SeqSourceProcessor.h"
#include "PlayPosition.h"
#include "CloneableSourceManager.h"
#include "CloneableAudioSource.h"
#include "CloneableMIDISource.h"

SeqSourceProcessor::SeqSourceProcessor(const juce::AudioChannelSet& type)
	: audioChannels(type) {
	/** Set Channel Layout */
	this->setChannelLayoutOfBus(true, 0, type);
	this->setChannelLayoutOfBus(false, 0, type);
}

void SeqSourceProcessor::prepareToPlay(
	double /*sampleRate*/, int /*maximumExpectedSamplesPerBlock*/) {}

void SeqSourceProcessor::processBlock(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	/** Check Buffer Is Empty */
	if (buffer.getNumSamples() <= 0) { return; }

	/** Get Play Head */
	auto playHead = dynamic_cast<PlayPosition*>(this->getPlayHead());
	if (!playHead) { return; }

	/** Get Current Position */
	juce::Optional<juce::AudioPlayHead::PositionInfo> position = playHead->getPosition();
	if (!position) { return; }

	/** Check Play State */
	if (!position->getIsPlaying()) { return; }

	/** Get Time */
	double time = position->getTimeInSeconds().orFallback(-1);

	/** Find Source */
	SourceList::SeqBlock block;
	{
		juce::GenericScopedLock locker(this->srcs.getLock());

		int index = this->srcs.match(time);
		if (index >= 0) {
			block = this->srcs.getUnchecked(index);
		}
	}

	/** Copy Source Data */
	{
		juce::GenericScopedLock locker(CloneableSourceManager::getInstance()->getLock());
		if (CloneableSource::SafePointer<> ptr = std::get<3>(block)) {
			/** Caculate Time */
			double sampleRate = this->getSampleRate();
			double duration = buffer.getNumSamples() / sampleRate;
			double endTime = time + duration;

			double dataStartTime = std::get<0>(block) + std::max(std::get<2>(block), 0.);
			double dataEndTime = 
				std::min(std::get<1>(block), std::get<0>(block) + std::get<2>(block) + ptr->getSourceLength());

			if (dataEndTime > dataStartTime) {
				double hotStartTime = std::max(time, dataStartTime);
				double hotEndTime = std::min(endTime, dataEndTime);

				if (hotEndTime > hotStartTime) {
					if (auto p = dynamic_cast<CloneableAudioSource*>(ptr.getSource())) {
						p->readData(buffer,
							hotStartTime - time,
							hotStartTime - (std::get<0>(block) + std::get<2>(block)),
							hotEndTime - hotStartTime);
					}
					else if (auto p = dynamic_cast<CloneableMIDISource*>(ptr.getSource())) {
						/** TODO Copy MIDI Message */
					}
				}
			}
		}
	}
}

double SeqSourceProcessor::getTailLengthSeconds() const {
	juce::GenericScopedLock locker(this->srcs.getLock());

	int size = this->srcs.size();
	return (size > 0) ? std::get<1>(this->srcs.getUnchecked(size - 1)) : 0;
}

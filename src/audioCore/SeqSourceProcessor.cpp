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
			if (auto p = dynamic_cast<CloneableAudioSource*>(ptr.getSource())) {
				/** TODO Copy Audio Data */
			}
			else if (auto p = dynamic_cast<CloneableMIDISource*>(ptr.getSource())) {
				/** TODO Copy MIDI Message */
			}
		}
	}
}

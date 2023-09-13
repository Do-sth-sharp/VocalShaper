#include "SourceRecordProcessor.h"

#include "../source/CloneableAudioSource.h"
#include "../source/CloneableMIDISource.h"
#include "../source/CloneableSynthSource.h"

SourceRecordProcessor::SourceRecordProcessor() {}

SourceRecordProcessor::~SourceRecordProcessor() {
	while (this->getTaskNum() > 0) {
		this->removeTask(0);
	}
}

void SourceRecordProcessor::addTask(
	CloneableSource::SafePointer<> source, double offset) {
	juce::ScopedWriteLock locker(this->taskLock);

	/** Check Source */
	if (!source) { return; }

	/** Source Already Exists */
	this->tasks.removeIf([&source](RecorderTask& t)
		{ return std::get<0>(t) == source; });

	/** Prepare Task */
	source->prepareToRecordInternal(this->getTotalNumInputChannels(),
		this->getSampleRate(), this->getBlockSize());
	
	/** Add Task */
	return this->tasks.add({ source, offset });
}

void SourceRecordProcessor::removeTask(int index) {
	juce::ScopedWriteLock locker(this->taskLock);
	auto [src, offset] = this->tasks.removeAndReturn(index);
	if (src) {
		src->recordingFinishedInternal();
	}
}

int SourceRecordProcessor::getTaskNum() const {
	juce::ScopedReadLock locker(this->taskLock);
	return this->tasks.size();
}

std::tuple<CloneableSource::SafePointer<>, double>
	SourceRecordProcessor::getTask(int index) const {
	juce::ScopedReadLock locker(this->taskLock);
	auto& [source, offset] = this->tasks.getReference(index);
	return { source, offset };
}

void SourceRecordProcessor::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	this->setRateAndBufferSizeDetails(sampleRate, maximumExpectedSamplesPerBlock);

	/** Update Source Sample Rate And Buffer Size */
	juce::ScopedReadLock locker(this->taskLock);
	for (auto& [source, offset] : this->tasks) {
		if (source) {
			source->prepareToRecordInternal(this->getTotalNumInputChannels(),
				this->getSampleRate(), this->getBlockSize(), true);
		}
	}
}

void SourceRecordProcessor::processBlock(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	/** Lock Task List */
	juce::ScopedTryReadLock locker(this->taskLock);
	if (!locker.isLocked()) { return; }

	/** Check Play State */
	auto playHead = this->getPlayHead();
	if (!playHead) { return; }
	auto playPosition = playHead->getPosition();
	if (!playPosition->getIsPlaying() || !playPosition->getIsRecording()) { return; }

	/** Check Each Task */
	for (auto& [source, offset] : this->tasks) {
		/** Get Task Info */
		if (offset - buffer.getNumSamples() / this->getSampleRate() >
			playPosition->getTimeInSeconds().orFallback(0)) { continue; }
		double startPos =
			playPosition->getTimeInSeconds().orFallback(0) - offset;

		/** Copy Data */
		if (auto src = dynamic_cast<CloneableAudioSource*>(source.getSource())) {
			/** Audio Source */
			src->writeData(buffer, startPos);
		}
		else if (auto src = dynamic_cast<CloneableMIDISource*>(source.getSource())) {
			/** MIDI Source */
			src->writeData(midiMessages, startPos);
		}
		else if (auto src = dynamic_cast<CloneableSynthSource*>(source.getSource())) {
			/** Synth Source */
			src->writeData(midiMessages, startPos);
		}
	}
}

double SourceRecordProcessor::getTailLengthSeconds() const {
	if (auto playHead = this->getPlayHead()) {
		return playHead->getPosition()->getTimeInSeconds().orFallback(0);
	}
	return 0;
}

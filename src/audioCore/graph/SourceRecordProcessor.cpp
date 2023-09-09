#include "SourceRecordProcessor.h"

#include "../source/CloneableAudioSource.h"
#include "../source/CloneableMIDISource.h"
#include "../source/CloneableSynthSource.h"

SourceRecordProcessor::SourceRecordProcessor() {}

void SourceRecordProcessor::addTask(
	CloneableSource::SafePointer<> source, double offset) {
	juce::ScopedWriteLock locker(this->taskLock);

	/** Check Source */
	if (!source) { return; }

	/** Source Already Exists */
	this->tasks.removeIf([&source](RecorderTask& t)
		{ return std::get<1>(t) == source; });

	/** Lock Task */
	auto taskLocker =
		std::make_shared<juce::ScopedWriteLock>(source->getRecorderLock());

	/** Prepare Task */
	if (auto src = dynamic_cast<CloneableAudioSource*>(source.getSource())) {
		/** Audio Source */
		src->prepareToRecord(this->getTotalNumInputChannels(),
			this->getSampleRate(), this->getBlockSize());
	}
	else if (auto src = dynamic_cast<CloneableMIDISource*>(source.getSource())) {
		/** MIDI Source */
		src->prepareToRecord();
	}
	else if (auto src = dynamic_cast<CloneableSynthSource*>(source.getSource())) {
		/** Synth Source */
		src->prepareToRecord();
	}

	/** Add Task */
	return this->tasks.add(
		{ taskLocker, source, offset });
}

void SourceRecordProcessor::removeTask(int index) {
	juce::ScopedWriteLock locker(this->taskLock);
	this->tasks.remove(index);
}

int SourceRecordProcessor::getTaskNum() const {
	juce::ScopedReadLock locker(this->taskLock);
	return this->tasks.size();
}

std::tuple<CloneableSource::SafePointer<>, double>
	SourceRecordProcessor::getTask(int index) const {
	juce::ScopedReadLock locker(this->taskLock);
	auto& [l, source, offset] = this->tasks.getReference(index);
	return { source, offset };
}

void SourceRecordProcessor::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	this->setRateAndBufferSizeDetails(sampleRate, maximumExpectedSamplesPerBlock);

	/** TODO Update Source Sample Rate And Buffer Size */
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
	for (auto& [l, source, offset] : this->tasks) {
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
			/** TODO Synth Source */
		}
	}
}

double SourceRecordProcessor::getTailLengthSeconds() const {
	if (auto playHead = this->getPlayHead()) {
		return playHead->getPosition()->getTimeInSeconds().orFallback(0);
	}
	return 0;
}

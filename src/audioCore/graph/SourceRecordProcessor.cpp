#include "SourceRecordProcessor.h"

#include "../source/CloneableAudioSource.h"
#include "../source/CloneableMIDISource.h"
#include "../source/CloneableSynthSource.h"

SourceRecordProcessor::SourceRecordProcessor() {
	/** TODO */
}

void SourceRecordProcessor::addTask(
	CloneableSource::SafePointer<> source,
	SourceRecordProcessor::ChannelConnectionList channels, double offset) {
	juce::ScopedWriteLock locker(this->taskLock);

	/** Check Source */
	if (!source) { return; }

	/** Source Already Exists */
	this->tasks.removeIf([&source](RecorderTask& t)
		{ return std::get<1>(t) == source; });

	/** Add Task */
	return this->tasks.add(
		{ std::make_shared<juce::ScopedWriteLock>(source->getRecorderLock()),
		source, channels, offset });
}

void SourceRecordProcessor::removeTask(int index) {
	juce::ScopedWriteLock locker(this->taskLock);
	this->tasks.remove(index);
}

int SourceRecordProcessor::getTaskNum() const {
	juce::ScopedReadLock locker(this->taskLock);
	return this->tasks.size();
}

std::tuple<CloneableSource::SafePointer<>, SourceRecordProcessor::ChannelConnectionList, double>
	SourceRecordProcessor::getTask(int index) const {
	juce::ScopedReadLock locker(this->taskLock);
	auto& [l, source, channels, offset] = this->tasks.getReference(index);
	return { source, channels, offset };
}

void SourceRecordProcessor::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	/** TODO */
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
	for (auto& task : this->tasks) {
		/** Get Task Info */
		auto& [l, source, channels, offset] = task;
		if (offset > playPosition->getTimeInSeconds()) { continue; }

		/** Copy Data */
		if (auto src = dynamic_cast<CloneableAudioSource*>(source.getSource())) {
			/** TODO Audio Source */
		}
		else if (auto src = dynamic_cast<CloneableMIDISource*>(source.getSource())) {
			/** TODO MIDI Source */
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

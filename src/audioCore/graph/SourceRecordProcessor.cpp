#include "SourceRecordProcessor.h"

SourceRecordProcessor::SourceRecordProcessor() {
	/** TODO */
}

void SourceRecordProcessor::addTask(
	CloneableSource::SafePointer<> source,
	SourceRecordProcessor::ChannelConnectionList channels) {
	juce::ScopedWriteLock locker(this->taskLock);

	/** Check Source */
	if (!source) { return; }

	/** Source Already Exists */
	this->tasks.removeIf([&source](RecorderTask& t)
		{ return std::get<1>(t) == source; });

	/** Add Task */
	return this->tasks.add(
		{ std::make_shared<juce::ScopedWriteLock>(source->getRecorderLock()),
		source, channels });
}

void SourceRecordProcessor::removeTask(int index) {
	juce::ScopedWriteLock locker(this->taskLock);
	this->tasks.remove(index);
}

int SourceRecordProcessor::getTaskNum() const {
	juce::ScopedReadLock locker(this->taskLock);
	return this->tasks.size();
}

std::tuple<CloneableSource::SafePointer<>, SourceRecordProcessor::ChannelConnectionList>
	SourceRecordProcessor::getTask(int index) const {
	juce::ScopedReadLock locker(this->taskLock);
	auto& [l, source, channels] = this->tasks.getReference(index);
	return { source, channels };
}

void SourceRecordProcessor::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	/** TODO */
}

void SourceRecordProcessor::processBlock(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	/** TODO */
}

double SourceRecordProcessor::getTailLengthSeconds() const {
	if (auto playHead = this->getPlayHead()) {
		return playHead->getPosition()->getTimeInSeconds().orFallback(0);
	}
	return 0;
}

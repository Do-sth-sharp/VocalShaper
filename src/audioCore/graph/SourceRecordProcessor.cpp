#include "SourceRecordProcessor.h"

#include "../source/CloneableAudioSource.h"
#include "../source/CloneableMIDISource.h"
#include "../source/CloneableSynthSource.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

SourceRecordProcessor::SourceRecordProcessor() {}

SourceRecordProcessor::~SourceRecordProcessor() {
	while (this->getTaskNum() > 0) {
		this->removeTask(0);
	}
}

void SourceRecordProcessor::addTask(
	CloneableSource::SafePointer<> source, int srcIndex, double offset) {
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
	return this->tasks.add({ source, srcIndex, offset });
}

void SourceRecordProcessor::removeTask(int index) {
	juce::ScopedWriteLock locker(this->taskLock);
	auto [src, srcIndex, offset] = this->tasks.removeAndReturn(index);
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
	auto& [source, srcIndex, offset] = this->tasks.getReference(index);
	return { source, offset };
}

void SourceRecordProcessor::clearGraph() {
	juce::ScopedWriteLock locker(this->taskLock);
	this->tasks.clear();
}

void SourceRecordProcessor::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	this->setRateAndBufferSizeDetails(sampleRate, maximumExpectedSamplesPerBlock);

	/** Update Source Sample Rate And Buffer Size */
	juce::ScopedReadLock locker(this->taskLock);
	for (auto& [source, srcIndex, offset] : this->tasks) {
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
	for (auto& [source, srcIndex, offset] : this->tasks) {
		/** Get Task Info */
		int offsetPos = std::floor(offset * this->getSampleRate());
		if (static_cast<long long>(offsetPos - buffer.getNumSamples()) >
			playPosition->getTimeInSamples().orFallback(0)) { continue; }
		int startPos =
			playPosition->getTimeInSamples().orFallback(0) - offsetPos;

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

bool SourceRecordProcessor::parse(const google::protobuf::Message* data) {
	/** TODO */
	return true;
}

std::unique_ptr<google::protobuf::Message> SourceRecordProcessor::serialize() const {
	auto mes = std::make_unique<vsp4::Recorder>();

	auto list = mes->mutable_sources();
	juce::ScopedReadLock locker(this->taskLock);
	for (auto& [source, srcIndex, offset] : this->tasks) {
		auto smes = std::make_unique<vsp4::SourceRecorderInstance>();
		smes->set_index(srcIndex);
		smes->set_offset(offset);
		list->AddAllocated(smes.release());
	}

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

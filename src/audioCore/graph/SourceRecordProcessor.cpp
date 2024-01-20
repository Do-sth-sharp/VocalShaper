#include "SourceRecordProcessor.h"

#include "../source/CloneableAudioSource.h"
#include "../source/CloneableMIDISource.h"
#include "../source/CloneableSourceManager.h"
#include "../uiCallback/UICallback.h"
#include "../misc/AudioLock.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

SourceRecordProcessor::SourceRecordProcessor() {}

SourceRecordProcessor::~SourceRecordProcessor() {
	while (this->getTaskNum() > 0) {
		this->removeTask(0);
	}
}

void SourceRecordProcessor::insertTask(
	const SourceRecordProcessor::RecorderTask& task, int index) {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	auto& [source, offset, compensate] = task;

	/** Check Source */
	if (!source) { return; }

	/** Source Already Exists */
	this->tasks.removeIf([&source](RecorderTask& t)
		{ return std::get<0>(t) == source; });

	/** Prepare Task */
	source->prepareToRecordInternal(this->getTotalNumInputChannels(),
		this->getSampleRate(), this->getBlockSize());
	
	/** Add Task */
	return this->tasks.insert(index, task);
}

void SourceRecordProcessor::removeTask(int index) {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	auto [src, offset, compensate] = this->tasks.removeAndReturn(index);
	if (src) {
		src->recordingFinishedInternal();
	}
}

int SourceRecordProcessor::getTaskNum() const {
	return this->tasks.size();
}

const SourceRecordProcessor::RecorderTask
	SourceRecordProcessor::getTask(int index) const {
	return this->tasks.getReference(index);
}

void SourceRecordProcessor::clearGraph() {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	this->tasks.clear();
}

void SourceRecordProcessor::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	this->setRateAndBufferSizeDetails(sampleRate, maximumExpectedSamplesPerBlock);

	/** Update Source Sample Rate And Buffer Size */
	for (auto& [source, offset, compensate] : this->tasks) {
		if (source) {
			source->prepareToRecordInternal(this->getTotalNumInputChannels(),
				this->getSampleRate(), this->getBlockSize(), true);
		}
	}
}

void SourceRecordProcessor::processBlock(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	/** Check Play State */
	auto playHead = this->getPlayHead();
	if (!playHead) { return; }
	auto playPosition = playHead->getPosition();
	if (!playPosition->getIsPlaying() || !playPosition->getIsRecording()) { return; }

	/** Check Each Task */
	for (auto& [source, offset, compensate] : this->tasks) {
		/** Get Task Info */
		int offsetPos = std::floor(offset * this->getSampleRate());
		if (static_cast<long long>(offsetPos - buffer.getNumSamples()) >
			playPosition->getTimeInSamples().orFallback(0)) { continue; }
		int startPos =
			playPosition->getTimeInSamples().orFallback(0) - offsetPos;
		startPos -= (compensate * this->getBlockSize());

		/** Copy Data */
		if (auto src = dynamic_cast<CloneableAudioSource*>(source.getSource())) {
			/** Audio Source */
			src->writeData(buffer, startPos);
		}
		else if (auto src = dynamic_cast<CloneableMIDISource*>(source.getSource())) {
			/** MIDI Source */
			src->writeData(midiMessages, startPos);
		}
	}

	/** Callback */
	if (this->tasks.size() > 0) {
		this->limitedCall.call([] {
			UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
			}, 500 / (1000 / (this->getSampleRate() / buffer.getNumChannels())), 500);
	}
}

double SourceRecordProcessor::getTailLengthSeconds() const {
	if (auto playHead = this->getPlayHead()) {
		return playHead->getPosition()->getTimeInSeconds().orFallback(0);
	}
	return 0;
}

bool SourceRecordProcessor::parse(const google::protobuf::Message* data) {
	auto mes = dynamic_cast<const vsp4::Recorder*>(data);
	if (!mes) { return false; }

	this->clearGraph();

	auto& list = mes->sources();
	for (auto& i : list) {
		if (auto ptrSrc = CloneableSourceManager::getInstance()->getSource(i.index())) {
			this->insertTask({ ptrSrc, i.offset(), i.compensate()});
		}
	}

	return true;
}

std::unique_ptr<google::protobuf::Message> SourceRecordProcessor::serialize() const {
	auto mes = std::make_unique<vsp4::Recorder>();

	auto list = mes->mutable_sources();
	for (auto& [source, offset, compensate] : this->tasks) {
		auto smes = std::make_unique<vsp4::SourceRecorderInstance>();
		smes->set_index(CloneableSourceManager::getInstance()->getSourceIndex(source));
		smes->set_offset(offset);
		smes->set_compensate(compensate);
		list->AddAllocated(smes.release());
	}

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

#include "SourceRecordProcessor.h"
#include "MainGraph.h"
#include "../uiCallback/UICallback.h"
#include "../misc/AudioLock.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

SourceRecordProcessor::SourceRecordProcessor(MainGraph* parent)
	: parent(parent) {}

SourceRecordProcessor::~SourceRecordProcessor() {}

void SourceRecordProcessor::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	this->setRateAndBufferSizeDetails(sampleRate, maximumExpectedSamplesPerBlock);
}

void SourceRecordProcessor::processBlock(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	/** Check Play State */
	auto playHead = this->getPlayHead();
	if (!playHead) { return; }
	auto playPosition = playHead->getPosition();
	if (!playPosition->getIsPlaying() || !playPosition->getIsRecording()) { return; }
	int timeInSamples = playPosition->getTimeInSamples().orFallback(0);

	/** Check Each Task */
	std::set<int> trackIndexList;
	int trackNum = this->parent->getSourceNum();
	for (int i = 0; i < trackNum; i++) {
		auto track = this->parent->getSourceProcessor(i);
		if (track->getRecording()) {
			/** Copy Data */
			track->writeAudioData(buffer, timeInSamples);
			track->writeMIDIData(midiMessages, timeInSamples);

			/** Add Track Index to List */
			trackIndexList.insert(i);
		}
	}

	/** Callback */
	if (trackIndexList.size() > 0 && buffer.getNumSamples() > 0) {
		this->limitedCall.call([trackIndexList] {
			UICallbackAPI<const std::set<int>&>::invoke(
				UICallbackType::SourceRecord, trackIndexList);
			}, 500 / (1000 / (this->getSampleRate() / buffer.getNumSamples())), 500);
	}
}

double SourceRecordProcessor::getTailLengthSeconds() const {
	if (auto playHead = this->getPlayHead()) {
		return playHead->getPosition()->getTimeInSeconds().orFallback(0);
	}
	return 0;
}

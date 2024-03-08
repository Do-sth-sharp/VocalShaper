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
	int count = 0;
	int trackNum = this->parent->getSourceNum();
	for (int i = 0; i < trackNum; i++) {
		auto track = this->parent->getSourceProcessor(i);
		if (track->getRecording()) {
			/** Copy Data */
			track->writeAudioData(buffer, timeInSamples);
			track->writeMIDIData(midiMessages, timeInSamples);

			/** Increase Count */
			count++;
		}
	}

	/** Callback */
	if (count > 0) {
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

#include "SynthRenderThread.h"

#include "CloneableSynthSource.h"
#include "PlayPosition.h"
#include "Utils.h"

SynthRenderThread::SynthRenderThread(CloneableSynthSource* parent)
	: Thread("Synth Render Thread"), parent(parent) {}

SynthRenderThread::~SynthRenderThread() {
	this->stopThread(3000);
}

void SynthRenderThread::run() {
	/** Lock Buffer */
	juce::GenericScopedLock audioLocker(parent->audioLock);

	/** Prepare Synthesizer */
	if (!parent->synthesizer) { return; }
	double sampleRate = parent->getSourceSampleRate();
	int bufferSize = parent->getBufferSize();
	int channelNum = parent->audioBuffer.getNumChannels();
	parent->synthesizer->prepareToPlay(sampleRate, bufferSize);

	/** Get MIDI Data */
	juce::MidiFile midiData;
	{
		juce::ScopedReadLock locker(parent->lock);
		midiData = parent->buffer;
	}

	/** Set Play Head */
	auto playHead = std::make_unique<MovablePlayHead>();
	playHead->setSampleRate(sampleRate);
	midiData.findAllTempoEvents(playHead->getTempoSequence());
	midiData.findAllTimeSigEvents(playHead->getTempoSequence());
	parent->synthesizer->setPlayHead(playHead.get());

	/** TODO ARA Hand Shake */

	/** Render Audio Data */
	double audioLength = parent->getLength();
	playHead->transportPlay(true);
	int clipNum = (audioLength * sampleRate) / bufferSize;
	for (int i = 0; !this->threadShouldExit() && (i <= clipNum); i++) {
		/** Clip Time */
		auto playPosition = playHead->getPosition();
		double startTime = playPosition->getTimeInSeconds().orFallback(0);
		double endTime = startTime + bufferSize / sampleRate;
		int64_t startPos = playPosition->getTimeInSamples().orFallback(0);

		/** Get MIDI Data */
		juce::MidiMessageSequence midiMessages;
		for (int j = 0; j < midiData.getNumTracks(); j++) {
			auto track = midiData.getTrack(j);
			if (track) {
				midiMessages.addSequence(midiMessages, 0, startTime, endTime);
			}
		}
		juce::MidiBuffer midiBuffer;
		for (auto j : midiMessages) {
			auto& message = j->message;
			double time = message.getTimeStamp();
			midiBuffer.addEvent(message, time * sampleRate);
		}

		/** Call Process Block */
		juce::AudioBuffer<float> audioBuffer(channelNum, bufferSize);
		parent->synthesizer->processBlock(audioBuffer, midiBuffer);
		
		/** Copy Audio Data */
		for (int j = 0; j < channelNum; j++) {
			parent->audioBuffer.copyFrom(
				j, startPos, audioBuffer.getReadPointer(j),
				(i == (clipNum - 1)) ? ((uint64_t)(audioLength * sampleRate) % clipNum) : bufferSize);
		}

		/** Set Play Head */
		playHead->next(bufferSize);
	}

	/** Reset Play Head */
	parent->synthesizer->setPlayHead(nullptr);
}

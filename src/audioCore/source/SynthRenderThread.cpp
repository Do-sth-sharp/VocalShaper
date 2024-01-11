#include "SynthRenderThread.h"

#include <DMDA.h>

#include "CloneableSynthSource.h"
#include "../misc/PlayPosition.h"
#include "../misc/AudioLock.h"
#include "../Utils.h"

SynthRenderThread::SynthRenderThread(CloneableSynthSource* parent)
	: Thread("Synth Render Thread"), parent(parent) {}

SynthRenderThread::~SynthRenderThread() {
	this->stopThread(3000);
}

void SynthRenderThread::run() {
	/** Lock Buffer */
	juce::ScopedWriteLock audioLocker(audioLock::getAudioLock());

	/** Prepare Synth Info */
	if (!parent->synthesizer) { return; }
	double sampleRate = parent->getSourceSampleRate();
	int bufferSize = parent->getBufferSize();
	int channelNum = parent->audioBuffer.getNumChannels();

	/** Get MIDI Data */
	juce::MidiFile midiData = parent->buffer;

	/** Set Play Head */
	auto playHead = std::make_unique<MovablePlayHead>();
	playHead->setSampleRate(sampleRate);
	midiData.findAllTempoEvents(playHead->getTempoSequence());
	midiData.findAllTimeSigEvents(playHead->getTempoSequence());
	parent->synthesizer->setPlayHead(playHead.get());

	/** DMDA Update Context Data */
	DMDA::PluginHandler contextDataHandler(
		[&midiData](DMDA::Context* context) { 
			if (auto ptrContext = dynamic_cast<DMDA::MidiFileContext*>(context)) {
				juce::ScopedWriteLock locker(ptrContext->getLock());
				ptrContext->setData(&midiData);
			}
		});
	parent->synthesizer->getExtensions(contextDataHandler);

	/** Render Audio Data */
	double audioLength = parent->getLength();
	playHead->transportPlay(true);

	uint64_t totalSamples = audioLength * sampleRate;
	int clipNum = totalSamples / bufferSize;
	for (int i = 0; !this->threadShouldExit() && (totalSamples > 0) && (i <= clipNum); i++) {
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
				midiMessages.addSequence(*track, 0, startTime, endTime);
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
				(i == clipNum) ? (totalSamples % clipNum) : bufferSize);
		}

		/** Set Play Head */
		playHead->next(bufferSize);
	}

	/** DMDA Release Context Data */
	DMDA::PluginHandler contextDataReleaseHandler(
		[](DMDA::Context* context) {
			if (auto ptrContext = dynamic_cast<DMDA::MidiFileContext*>(context)) {
				juce::ScopedWriteLock locker(ptrContext->getLock());
				ptrContext->setData(nullptr);
			}
		});
	parent->synthesizer->getExtensions(contextDataReleaseHandler);

	/** Release Source */
	auto releaseFunc = [src = CloneableSynthSource::SafePointer(parent)] {
		if (src && src->synthesizer) {
			src->synthesizer->releaseResources();
		}
	};
	juce::MessageManager::callAsync(releaseFunc);

	/** Reset Play Head */
	parent->synthesizer->setPlayHead(nullptr);
}

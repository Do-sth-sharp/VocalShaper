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
	int clipSize = (audioLength * sampleRate) / bufferSize;
	for (int i = 0; !this->threadShouldExit() && (i <= clipSize); i++) {
		/** TODO Get MIDI Data */
		/** TODO Call Process Block */
		/** TODO Set Play Head */
	}

	/** Reset Play Head */
	parent->synthesizer->setPlayHead(nullptr);
}

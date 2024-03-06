#include "SynthThread.h"
#include "../graph/SeqSourceProcessor.h"
#include "../misc/AudioLock.h"

SynthThread::SynthThread(SeqSourceProcessor* parent)
	: Thread("Synth Thread"), parent(parent) {}

SynthThread::~SynthThread() {
	this->stopThread(5000);
}

void SynthThread::synthNow() {
	/** Check Thread Running */
	if (this->isThreadRunning()) { return; }

	/** Prepare MIDI */
	this->parent->prepareMIDIData();
	double length = this->parent->getMIDILength();
	this->parent->prepareAudioData(length);

	/** Start Thread */
	this->startThread();
}

void SynthThread::run() {
	/** TODO */
}

#include "SynthThread.h"
#include "../graph/SeqSourceProcessor.h"
#include "../misc/AudioLock.h"
#include "../misc/PlayPosition.h"
#include "../Utils.h"

SynthThread::SynthThread(SeqSourceProcessor* parent)
	: Thread("Synth Thread"), parent(parent) {}

SynthThread::~SynthThread() {
	this->stopThread(5000);
}

void SynthThread::synthNow() {
	/** Check Thread Running */
	if (this->isThreadRunning()) { return; }

	/** Check Synth Plugin */
	if (!this->parent->getInstrOffline()) { return; }
	auto plugin = dynamic_cast<PluginDecorator*>(
		this->parent->instr->getProcessor());
	if (!plugin) { return; }
	bool midiMode = plugin->producesMidi();

	/** Prepare MIDI */
	if (midiMode) {
		this->parent->prepareMIDIData();
	}
	double length = midiMode 
		? this->parent->getAudioLength()
		: this->parent->getMIDILength();
	if (!midiMode) {
		this->parent->prepareAudioData(length);
	}

	/** Start Thread */
	this->startThread();
}

void SynthThread::run() {
	/** Lock */
	juce::ScopedWriteLock sourceLocker(audioLock::getSourceLock());
	juce::ScopedReadLock pluginLocker(audioLock::getPluginLock());

	/** Check Synth Plugin */
	if (!this->parent->getInstrOffline()) { return; }
	auto plugin = dynamic_cast<PluginDecorator*>(
		this->parent->instr->getProcessor());
	if (!plugin) { return; }
	bool midiMode = plugin->producesMidi();

	/** Get Data Ptr */
	auto ptrMidiData = this->parent->midiData.get();
	auto ptrAudioData = this->parent->audioData.get();

	/** Prepare Synth Info */
	double sampleRate = plugin->getSampleRate();
	int bufferSize = plugin->getBlockSize();
	int channelNum = std::max(plugin->getTotalNumInputChannels(),
		plugin->getTotalNumOutputChannels());
	int ioChannels = std::min(channelNum,
		(ptrAudioData ? ptrAudioData->getNumChannels() : 0));

	/** Set Play Head */
	auto playHead = std::make_unique<MovablePlayHead>();
	playHead->setSampleRate(sampleRate);
	{
		juce::ScopedReadLock pluginLocker(audioLock::getPositionLock());
		playHead->getTempoSequence() = PlayPosition::getInstance()->getTempoSequence();
	}
	auto oldPlayHead = plugin->getPlayHead();
	plugin->setPlayHead(playHead.get());

	/** Set Flag */
	if (midiMode) {
		this->parent->midiChanged();
	}
	else {
		this->parent->audioChanged();
	}

	/** DMDA Update Context Data */
	plugin->setDMDAData(ptrMidiData);

	/** Render Data */
	double sourceLength = midiMode
		? this->parent->getAudioLength()
		: this->parent->getMIDILength();
	uint64_t totalSamples = sourceLength * sampleRate;
	playHead->transportPlay(true);

	double sourceSampleRate = this->parent->audioSampleRate;
	uint64_t sourceTotalSamples = sourceLength * sourceSampleRate;
	double sourceResampleRatio = sourceSampleRate / sampleRate;
	double sourceOutResampleRatio = 1 / sourceResampleRatio;

	if (ptrMidiData && midiMode) {
		ptrMidiData->addTrack(juce::MidiMessageSequence{});
	}
	if (ptrAudioData && (!midiMode)) {
		if (ptrAudioData->getNumSamples() < sourceTotalSamples) {
			ptrAudioData->setSize(ptrAudioData->getNumChannels(),
				sourceTotalSamples, true, true, true);
		}
	}

	juce::AudioSampleBuffer writeBuffer, writeBufferTemp;
	int clipNum = totalSamples / bufferSize;
	for (int i = 0; !this->threadShouldExit() && (totalSamples > 0) && (i <= clipNum); i++) {
		/** Clip Time */
		auto playPosition = playHead->getPosition();
		double startTime = playPosition->getTimeInSeconds().orFallback(0);
		double endTime = startTime + bufferSize / sampleRate;
		int64_t startPos = playPosition->getTimeInSamples().orFallback(0);

		/** Input Buffer */
		juce::MidiBuffer midiBuffer;
		juce::AudioBuffer<float> audioBuffer(channelNum, bufferSize);

		/** Get MIDI Data */
		if (ptrMidiData && (!midiMode)) {
			juce::MidiMessageSequence midiMessages;
			for (int j = 0; j < ptrMidiData->getNumTracks(); j++) {
				auto track = ptrMidiData->getTrack(j);
				if (track) {
					midiMessages.addSequence(*track, 0, startTime, endTime);
				}
			}
			for (auto j : midiMessages) {
				auto& message = j->message;
				double time = message.getTimeStamp();
				midiBuffer.addEvent(message, time * sampleRate);
			}
		}

		/** Get Audio Data */
		if (ptrAudioData && midiMode) {
			juce::MemoryAudioSource memSource(*ptrAudioData, false, false);
			juce::ResamplingAudioSource resampleSource(&memSource, false, ioChannels);
			resampleSource.setResamplingRatio(sourceResampleRatio);
			memSource.setNextReadPosition(startPos * sourceResampleRatio);
			resampleSource.prepareToPlay(bufferSize, sampleRate);

			int startSample = startPos;
			if (startSample < totalSamples) {
				int clipSize = bufferSize;
				if (startSample + clipSize >= totalSamples) {
					clipSize = totalSamples - startSample;
				}
				resampleSource.getNextAudioBlock(juce::AudioSourceChannelInfo{
						&audioBuffer, 0, clipSize });
			}
		}

		/** Call Process Block */
		plugin->processBlock(audioBuffer, midiBuffer);

		/** Copy MIDI Data */
		if (ptrMidiData && midiMode) {
			if (auto track = const_cast<juce::MidiMessageSequence*>(
				ptrMidiData->getTrack(ptrMidiData->getNumTracks() - 1))) {
				for (const auto& m : midiBuffer) {
					double timeStamp = startTime + m.samplePosition / sampleRate;
					if (timeStamp >= 0) {
						auto mes = m.getMessage();
						mes.setTimeStamp(timeStamp);
						track->addEvent(mes);
					}
				}
			}
		}

		/** Copy Audio Data */
		if (ptrAudioData && (!midiMode)) {
			utils::bufferOutputResampledFixed(*ptrAudioData, audioBuffer,
				writeBuffer, writeBufferTemp, sourceOutResampleRatio, ioChannels, sourceSampleRate,
				0, startPos, audioBuffer.getNumSamples());
		}

		/** Set Play Head */
		playHead->next(bufferSize);
	}

	/** DMDA Release Context Data */
	plugin->setDMDAData(nullptr);

	/** Reset Play Head */
	plugin->setPlayHead(oldPlayHead);
}

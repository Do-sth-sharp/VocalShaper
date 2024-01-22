#include "SynthRenderThread.h"

#include <DMDA.h>

#include "../misc/PlayPosition.h"
#include "../misc/AudioLock.h"
#include "../misc/VMath.h"
#include "../uiCallback/UICallback.h"
#include "../Utils.h"

SynthRenderThread::SynthRenderThread(CloneableSource* src)
	: Thread("Synth Render Thread"), src(src) {}

SynthRenderThread::~SynthRenderThread() {
	this->stopThread(3000);
}

void SynthRenderThread::setDstSource(CloneableSource::SafePointer<> dst) {
	/** Lock Buffer */
	juce::ScopedWriteLock audioLocker(audioLock::getAudioLock());

	/** Set Source */
	this->dst = dst;

	/** Callback */
	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});
}

CloneableSource::SafePointer<> SynthRenderThread::getDstSource() const {
	return this->dst;
}

void SynthRenderThread::run() {
	/** Callback */
	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});

	/** Lock Buffer */
	juce::ScopedWriteLock audioLocker(audioLock::getAudioLock());

	/** Check Synth Source */
	if (!this->src->synthesizer) { return; }
	if (!this->dst) { return; }

	/** Mark As Dst */
	this->dst->synthingByOther = true;

	/** Get Input Data */
	auto ptrMidiData = this->src->getMidiContentPtr();
	auto ptrAudioData = this->src->getAudioContentPtr();
	auto ptrMidiOut = this->dst->getMidiContentPtr();
	auto ptrAudioOut = this->dst->getAudioContentPtr();

	/** Prepare Synth Info */
	double sampleRate = this->src->synthesizer->getSampleRate();
	int bufferSize = this->src->synthesizer->getBlockSize();
	int channelNum = std::max(this->src->synthesizer->getTotalNumInputChannels(),
		this->src->synthesizer->getTotalNumOutputChannels());
	int inputChannels = std::min(channelNum,
		(ptrAudioData ? ptrAudioData->getNumChannels() : 0));
	int outputChannels = std::min(channelNum, 
		(ptrAudioOut ? ptrAudioOut->getNumChannels() : 0));

	/** Set Play Head */
	auto playHead = std::make_unique<MovablePlayHead>();
	playHead->setSampleRate(sampleRate);
	if (ptrMidiData) {
		ptrMidiData->findAllTempoEvents(playHead->getTempoSequence());
		ptrMidiData->findAllTimeSigEvents(playHead->getTempoSequence());
	}
	this->src->synthesizer->setPlayHead(playHead.get());

	/** DMDA Update Context Data */
	DMDA::PluginHandler contextDataHandler(
		[ptrMidiData](DMDA::Context* context) {
			if (auto ptrContext = dynamic_cast<DMDA::MidiFileContext*>(context)) {
				juce::ScopedWriteLock locker(ptrContext->getLock());
				if (ptrMidiData) {
					ptrContext->setData(ptrMidiData);
				}
			}
		});
	this->src->synthesizer->getExtensions(contextDataHandler);

	/** Render Audio Data */
	double audioLength = this->src->getLength();
	uint64_t totalSamples = audioLength * sampleRate;
	playHead->transportPlay(true);

	double dstSampleRate = this->dst->getSourceSampleRate();
	uint64_t dstTotalSamples = audioLength * dstSampleRate;
	double dstResampleRatio = sampleRate / dstSampleRate;

	double srcSampleRate = this->src->getSourceSampleRate();
	uint64_t srcTotalSamples = audioLength * srcSampleRate;
	double srcResampleRatio = srcSampleRate / sampleRate;

	if (ptrMidiOut) {
		ptrMidiOut->addTrack(juce::MidiMessageSequence{});
	}
	if (ptrAudioOut) {
		if (ptrAudioOut->getNumSamples() < dstTotalSamples) {
			ptrAudioOut->setSize(ptrAudioOut->getNumChannels(),
				dstTotalSamples, true, true, true);
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
		if (ptrMidiData) {
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
		if (ptrAudioData) {
			juce::MemoryAudioSource memSource(*ptrAudioData, false, false);
			juce::ResamplingAudioSource resampleSource(&memSource, false, inputChannels);
			resampleSource.setResamplingRatio(srcResampleRatio);
			memSource.setNextReadPosition(startPos * srcResampleRatio);
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
		this->src->synthesizer->processBlock(audioBuffer, midiBuffer);
		
		/** Copy MIDI Data */
		if (ptrMidiOut) {
			if (auto track = const_cast<juce::MidiMessageSequence*>(
				ptrMidiOut->getTrack(ptrMidiOut->getNumTracks() - 1))) {
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
		if (ptrAudioOut) {
			utils::bufferOutputResampledFixed(*ptrAudioOut, audioBuffer,
				writeBuffer, writeBufferTemp, dstResampleRatio, outputChannels, dstSampleRate,
				0, startPos, audioBuffer.getNumSamples());
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
	this->src->synthesizer->getExtensions(contextDataReleaseHandler);

	/** Release Source */
	auto releaseFunc = [src = CloneableSource::SafePointer(src)] {
		if (src && src->synthesizer) {
			src->synthesizer->releaseResources();
		}
	};
	juce::MessageManager::callAsync(releaseFunc);

	/** Reset Play Head */
	this->src->synthesizer->setPlayHead(nullptr);

	/** Clear Dst Mark */
	this->dst->synthingByOther = false;

	/** Callback */
	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});
}

#include "CloneableSynthSource.h"

#include <DMDA.h>
#include "Utils.h"

CloneableSynthSource::CloneableSynthSource()
    : CloneableSource() {
    this->synthThread = std::make_unique<SynthRenderThread>(this);
}

int CloneableSynthSource::getTrackNum() const {
    return this->buffer.getNumTracks();
}

double CloneableSynthSource::getSourceSampleRate() const {
    return this->sourceSampleRate;
}

void CloneableSynthSource::readData(juce::AudioBuffer<float>& buffer, double bufferDeviation,
    double dataDeviation, double length) const {
    if (buffer.getNumSamples() <= 0 || length <= 0) { return; }

    juce::GenericScopedTryLock locker(this->audioLock);
    if (locker.isLocked()) {
        if (this->source && this->memorySource) {
            this->memorySource->setNextReadPosition(std::floor(dataDeviation * this->sourceSampleRate));
            int startSample = (int)std::floor(bufferDeviation * this->getSampleRate());
            this->source->getNextAudioBlock(juce::AudioSourceChannelInfo{
                &buffer, startSample,
                    std::min(buffer.getNumSamples() - startSample, (int)std::ceil(length * this->getSampleRate()))});/**< Ceil then min with buffer size to ensure audio data fill the last point in buffer */
        }
    }
}

int CloneableSynthSource::getChannelNum() const {
    return this->audioBuffer.getNumChannels();
}

void CloneableSynthSource::setSynthesizer(
    std::unique_ptr<juce::AudioPluginInstance> synthesizer) {
    /** Stop Render */
    this->synthThread->stopThread(3000);

    this->synthesizer = std::move(synthesizer);

    /** DMDA Hand Shake */
    DMDA::PluginHandler handShakeHandler(
        [](DMDA::Context* context) { context->handShake(); });
    this->synthesizer->getExtensions(handShakeHandler);
}

const juce::String CloneableSynthSource::getSynthesizerName() const {
    if (this->synthesizer) {
        return this->synthesizer->getName();
    }
    return juce::String{};
}

void CloneableSynthSource::stopSynth() {
    this->synthThread->stopThread(3000);
}

void CloneableSynthSource::synth() {
    /** Stop Render */
    this->synthThread->stopThread(3000);

    /** Lock Buffer */
    juce::ScopedReadLock locker(this->lock);
    juce::GenericScopedLock audioLocker(this->audioLock);

    /** Prepare Audio Buffer */
    this->sourceSampleRate = this->getSampleRate();
    this->sampleRateChanged();
    this->audioBuffer.setSize(
        this->audioChannels, this->getLength() * this->sourceSampleRate,
        false, true, true);

    /** Start Render */
    this->synthThread->startThread();
}

bool CloneableSynthSource::clone(const CloneableSource* src) {
    /** Check Source Type */
    auto ptrSrc = dynamic_cast<const CloneableSynthSource*>(src);
    if (!ptrSrc) { return false; }

    /** Copy MIDI Data */
    {
        juce::ScopedWriteLock locker0(this->lock);
        juce::ScopedReadLock locker1(ptrSrc->lock);
        this->buffer = ptrSrc->buffer;
    }

    return true;
}

bool CloneableSynthSource::load(const juce::File& file) {
    /** Create Input Stream */
    juce::FileInputStream stream(file);
    if (stream.failedToOpen()) { return false; }

    /** Read MIDI File */
    juce::MidiFile midiFile;
    if (!midiFile.readFrom(stream, true)) { return false; }

    /** Copy Data */
    {
        juce::ScopedWriteLock locker(this->lock);
        this->buffer = midiFile;
        this->buffer.convertTimestampTicksToSeconds();
    }

    return true;
}

bool CloneableSynthSource::save(const juce::File& file) const {
    /** Create Output Stream */
    juce::FileOutputStream stream(file);
    if (stream.failedToOpen()) { return false; }
    stream.setPosition(0);
    stream.truncate();

    /** Copy Data */
    juce::ScopedWriteLock locker(this->lock);
    juce::MidiFile midiFile{ this->buffer };
    CloneableSynthSource::convertSecondsToTicks(midiFile);

    /** Write MIDI File */
    return midiFile.writeTo(stream);
}

double CloneableSynthSource::getLength() const {
    juce::ScopedReadLock locker(this->lock);

    /** Get Time In Seconds */
    return this->buffer.getLastTimestamp() + this->tailTime;
}

void CloneableSynthSource::sampleRateChanged() {
    juce::GenericScopedLock locker(this->audioLock);
    if (this->source) {
        this->source->setResamplingRatio(this->sourceSampleRate / this->getSampleRate());
        this->source->prepareToPlay(this->getBufferSize(), this->getSampleRate());
    }
}

void CloneableSynthSource::convertSecondsToTicks(juce::MidiFile& file) {
    /** Get Tempo Events */
    juce::MidiMessageSequence tempoSeq;
    file.findAllTempoEvents(tempoSeq);
    file.findAllTimeSigEvents(tempoSeq);

    auto timeFormat = file.getTimeFormat();
    if (timeFormat != 0)
    {
        for (int i = 0; i < file.getNumTracks(); i++) {
            if (auto track = file.getTrack(i)) {
                for (int j = track->getNumEvents(); --j >= 0;)
                {
                    auto& m = track->getEventPointer(j)->message;
                    m.setTimeStamp(utils::convertSecondsToTicks(m.getTimeStamp(), tempoSeq, timeFormat));
                }
            }
        }
    }
}

#include "CloneableMIDISource.h"

#include "Utils.h"

void CloneableMIDISource::readData(
    juce::MidiBuffer& buffer, double baseTime,
    double startTime, double endTime) const {
    juce::ScopedReadLock locker(this->lock);

    /** Get MIDI Data */
    juce::MidiMessageSequence total;
    for (int i = 0; i < this->buffer.getNumTracks(); i++) {
        auto track = this->buffer.getTrack(i);
        if (track) {
            total.addSequence(*track, 0, startTime, endTime);
        }
    }

    /** Copy Data */
    for (auto i : total) {
        auto& message = i->message;
        double time = message.getTimeStamp();
        buffer.addEvent(message, std::floor((time + baseTime) * this->getSampleRate()));
    }
}

int CloneableMIDISource::getTrackNum() const {
    return this->buffer.getNumTracks();
}

bool CloneableMIDISource::clone(const CloneableSource* src) {
	/** Check Source Type */
	auto ptrSrc = dynamic_cast<const CloneableMIDISource*>(src);
	if (!ptrSrc) { return false; }

	/** Copy MIDI Data */
    {
        juce::ScopedWriteLock locker0(this->lock);
        juce::ScopedReadLock locker1(ptrSrc->lock);
        this->buffer = ptrSrc->buffer;
    }

	return true;
}

bool CloneableMIDISource::load(const juce::File& file) {
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

bool CloneableMIDISource::save(const juce::File& file) const {
    /** Create Output Stream */
	juce::FileOutputStream stream(file);
	if (stream.failedToOpen()) { return false; }
    stream.setPosition(0);
    stream.truncate();

    /** Copy Data */
    juce::ScopedWriteLock locker(this->lock);
    juce::MidiFile midiFile{ this->buffer };
    CloneableMIDISource::convertSecondsToTicks(midiFile);

	/** Write MIDI File */
	return midiFile.writeTo(stream);
}

double CloneableMIDISource::getLength() const {
    juce::ScopedReadLock locker(this->lock);

	/** Get Time In Seconds */
    return this->buffer.getLastTimestamp();
}

void CloneableMIDISource::convertSecondsToTicks(juce::MidiFile& file) {
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

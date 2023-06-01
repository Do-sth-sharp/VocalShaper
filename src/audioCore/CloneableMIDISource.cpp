#include "CloneableMIDISource.h"

void CloneableMIDISource::readData(
    juce::MidiBuffer& buffer, double baseTime,
    double startTime, double endTime) const {
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

bool CloneableMIDISource::clone(const CloneableSource* src) {
	/** Check Source Type */
	auto ptrSrc = dynamic_cast<const CloneableMIDISource*>(src);
	if (!ptrSrc) { return false; }

	/** Copy MIDI Data */
	this->buffer = ptrSrc->buffer;

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
	this->buffer = midiFile;
    this->buffer.convertTimestampTicksToSeconds();

	return true;
}

bool CloneableMIDISource::save(const juce::File& file) const {
    /** Create Output Stream */
	juce::FileOutputStream stream(file);
	if (stream.failedToOpen()) { return false; }

    /** Copy Data */
    juce::MidiFile midiFile{ this->buffer };
    CloneableMIDISource::convertSecondsToTicks(midiFile);

	/** Write MIDI File */
	return midiFile.writeTo(stream);
}

double CloneableMIDISource::getLength() const {
	/** Get Time In Seconds */
    return this->buffer.getLastTimestamp();
}

double CloneableMIDISource::convertSecondsToTicks(double time,
    const juce::MidiMessageSequence& tempoEvents,
    int timeFormat) {
    if (timeFormat < 0)
        return time * (-(timeFormat >> 8) * (timeFormat & 0xff));

    double lastTime = 0, correctedTime = 0;
    auto tickLen = 1.0 / (timeFormat & 0x7fff);
    auto secsPerTick = 0.5 * tickLen;
    auto numEvents = tempoEvents.getNumEvents();

    for (int i = 0; i < numEvents; ++i)
    {
        auto& m = tempoEvents.getEventPointer(i)->message;
        auto eventTime = m.getTimeStamp();

        if (eventTime >= time)
            break;

        correctedTime += (eventTime - lastTime) / secsPerTick;
        lastTime = eventTime;

        if (m.isTempoMetaEvent())
            secsPerTick = tickLen * m.getTempoSecondsPerQuarterNote();

        while (i + 1 < numEvents)
        {
            auto& m2 = tempoEvents.getEventPointer(i + 1)->message;

            if (!juce::approximatelyEqual(m2.getTimeStamp(), eventTime))
                break;

            if (m2.isTempoMetaEvent())
                secsPerTick = tickLen * m2.getTempoSecondsPerQuarterNote();

            ++i;
        }
    }

    return correctedTime + (time - lastTime) / secsPerTick;
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
            auto track = file.getTrack(i);
            if (track) {
                for (int j = track->getNumEvents(); --j >= 0;)
                {
                    auto& m = track->getEventPointer(j)->message;
                    m.setTimeStamp(CloneableMIDISource::convertSecondsToTicks(m.getTimeStamp(), tempoSeq, timeFormat));
                }
            }
        }
    }
}

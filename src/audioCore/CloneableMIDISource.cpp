#include "CloneableMIDISource.h"

void CloneableMIDISource::readData(
    juce::MidiBuffer& buffer, double baseTime,
    double startTime, double endTime) const {
    /** TODO Get MIDI Data */
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

	return true;
}

bool CloneableMIDISource::save(const juce::File& file) const {
	/** Create Output Stream */
	juce::FileOutputStream stream(file);
	if (stream.failedToOpen()) { return false; }

	/** Write MIDI File */
	return this->buffer.writeTo(stream);
}

double CloneableMIDISource::getLength() const {
	/** Get Tempo Events */
	juce::MidiMessageSequence tempoSeq;
	this->buffer.findAllTempoEvents(tempoSeq);
	this->buffer.findAllTimeSigEvents(tempoSeq);

	/** Get Time In Seconds */
    return CloneableMIDISource::convertTicksToSeconds(
        this->buffer.getLastTimestamp(), tempoSeq, this->buffer.getTimeFormat());
}

/** From juce_MidiFile.cpp */
double CloneableMIDISource::convertTicksToSeconds(double time,
    const juce::MidiMessageSequence& tempoEvents,
    int timeFormat) 
{
    if (timeFormat < 0)
        return time / (-(timeFormat >> 8) * (timeFormat & 0xff));

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

        correctedTime += (eventTime - lastTime) * secsPerTick;
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

    return correctedTime + (time - lastTime) * secsPerTick;
}

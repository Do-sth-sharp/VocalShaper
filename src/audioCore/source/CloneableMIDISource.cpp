#include "CloneableMIDISource.h"

#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

void CloneableMIDISource::readData(
	juce::MidiBuffer& buffer, double baseTime,
	double startTime, double endTime) const {
	if (this->checkRecording()) { return; }

	juce::ScopedTryReadLock locker(this->lock);

	if (locker.isLocked()) {
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
}

int CloneableMIDISource::getTrackNum() const {
	return this->buffer.getNumTracks();
}

bool CloneableMIDISource::parse(const google::protobuf::Message* data) {
	/** TODO */
	return false;
}

std::unique_ptr<google::protobuf::Message> CloneableMIDISource::serialize() const {
	auto mes = std::make_unique<vsp4::Source>();

	mes->set_type(vsp4::Source_Type_MIDI);
	mes->set_name(this->getName().toStdString());
	juce::String path = this->getPath();
	mes->set_path(
		juce::String{ path.isNotEmpty() ? path
		: utils::getSourceDefaultPathForMIDI(
			this->getId(), this->getName()) }.toStdString());

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

std::unique_ptr<CloneableSource> CloneableMIDISource::clone() const {
	/** Lock */
	juce::ScopedTryReadLock locker(this->lock);
	if (locker.isLocked()) {
		/** Create New Source */
		auto dst = std::unique_ptr<CloneableMIDISource>();

		/** Copy MIDI Data */
		dst->buffer = this->buffer;

		/** Move Result */
		return std::move(std::unique_ptr<CloneableSource>(dst.release()));
	}
	return nullptr;
}

bool CloneableMIDISource::load(const juce::File& file) {
	/** Check Not Recording */
	if (this->checkRecording()) { return false; }

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
	auto stream = std::make_unique<juce::FileOutputStream>(file);
	if (stream->failedToOpen()) { return false; }
	stream->setPosition(0);
	stream->truncate();

	/** Copy Data */
	juce::ScopedWriteLock locker(this->lock);
	juce::MidiFile midiFile{ this->buffer };
	utils::convertSecondsToTicks(midiFile);

	/** Write MIDI File */
	if (!midiFile.writeTo(*(stream.get()))) {
		stream = nullptr;
		file.deleteFile();
		return false;
	}

	return true;
}

double CloneableMIDISource::getLength() const {
	juce::ScopedReadLock locker(this->lock);

	/** Get Time In Seconds */
	return this->buffer.getLastTimestamp();
}

void CloneableMIDISource::prepareToRecord(
	int /*inputChannels*/, double /*sampleRate*/,
	int /*blockSize*/, bool updateOnly) {
	if (!updateOnly) {
		/** Lock */
		juce::ScopedWriteLock locker(this->lock);

		/** Add New Track */
		this->buffer.addTrack(juce::MidiMessageSequence{});

		/** Set Flag */
		this->changed();
	}
}

void CloneableMIDISource::recordingFinished() {
	/** Lock */
	juce::ScopedWriteLock locker(this->lock);

	/** Match Notes */
	for (int i = 0; i < this->buffer.getNumTracks(); i++) {
		if (auto track =
			const_cast<juce::MidiMessageSequence*>(this->buffer.getTrack(i))) {
			track->updateMatchedPairs();
		}
	}

	/** Set Flag */
	this->changed();
}

void CloneableMIDISource::writeData(
	const juce::MidiBuffer& buffer, int offset) {
	/** Lock */
	juce::ScopedTryWriteLock locker(this->lock);
	if (locker.isLocked()) {
		/** Write To The Last Track */
		if (auto track = const_cast<juce::MidiMessageSequence*>(
			this->buffer.getTrack(this->buffer.getNumTracks() - 1))) {
			for (const auto& m : buffer) {
				double timeAdjustment = (m.samplePosition + offset) / this->getSampleRate();
				if (timeAdjustment >= 0) {
					track->addEvent(m.getMessage(), timeAdjustment);
				}
			}

			/** Set Flag */
			this->changed();
		}
	}
}

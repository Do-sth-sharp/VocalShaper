#include "CloneableMIDISource.h"

#include "../uiCallback/UICallback.h"
#include "../misc/AudioLock.h"
#include "../AudioConfig.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

void CloneableMIDISource::readData(
	juce::MidiBuffer& buffer, int baseTime,
	int startTime, int endTime) const {
	/** Get MIDI Data */
	juce::MidiMessageSequence total;
	for (int i = 0; i < this->buffer.getNumTracks(); i++) {
		if (auto track = this->buffer.getTrack(i)) {
			total.addSequence(*track, 0, startTime / this->getSampleRate(), endTime / this->getSampleRate());
		}
	}

	/** Copy Data */
	for (auto i : total) {
		auto& message = i->message;
		double time = message.getTimeStamp();
		buffer.addEvent(message, std::floor((time + baseTime / this->getSampleRate()) * this->getSampleRate()));
	}
}

int CloneableMIDISource::getTrackNum() const {
	return this->buffer.getNumTracks();
}

int CloneableMIDISource::getEventNum() const {
	juce::ScopedTryReadLock locker(audioLock::getSourceLock());

	int total = 0;
	for (int i = 0; i < this->buffer.getNumTracks(); i++) {
		if (auto track = this->buffer.getTrack(i)) {
			total += track->getNumEvents();
		}
	}
	return total;
}

bool CloneableMIDISource::parse(const google::protobuf::Message* data) {
	auto src = dynamic_cast<const vsp4::Source*>(data);
	if (!src) { return false; }

	this->setName(src->name());
	this->setPath(src->path());
	return true;
}

std::unique_ptr<google::protobuf::Message> CloneableMIDISource::serialize() const {
	auto mes = std::make_unique<vsp4::Source>();

	mes->set_type(vsp4::Source_Type_MIDI);
	mes->set_id(this->getId());
	mes->set_name(this->getName().toStdString());
	juce::String path = this->getPath();
	mes->set_path(
		juce::String{ path.isNotEmpty() ? path
		: utils::getSourceDefaultPathForMIDI(
			this->getId(), this->getName()) }.toStdString());

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

std::unique_ptr<CloneableSource> CloneableMIDISource::createThisType() const {
	return std::unique_ptr<CloneableSource>{ new CloneableMIDISource };
}

bool CloneableMIDISource::clone(CloneableSource* dst) const {
	/** Lock */
	juce::ScopedTryReadLock locker(audioLock::getSourceLock());
	if (locker.isLocked()) {
		if (auto dsts = dynamic_cast<CloneableMIDISource*>(dst)) {
			/** Copy MIDI Data */
			dsts->buffer = this->buffer;

			/** Result */
			return true;
		}
	}
	return false;
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
		juce::ScopedWriteLock locker(audioLock::getSourceLock());
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
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
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
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	/** Get Time In Seconds */
	return this->buffer.getLastTimestamp() + AudioConfig::getMidiTail();
}

void CloneableMIDISource::prepareToRecord(
	int /*inputChannels*/, double /*sampleRate*/,
	int /*blockSize*/, bool updateOnly) {
	if (!updateOnly) {
		/** Lock */
		juce::ScopedWriteLock locker(audioLock::getSourceLock());

		/** Add New Track */
		this->buffer.addTrack(juce::MidiMessageSequence{});

		/** Set Flag */
		this->changed();

		/** Callback */
		juce::MessageManager::callAsync([] {
			UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
			});
	}
}

void CloneableMIDISource::recordingFinished() {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Match Notes */
	for (int i = 0; i < this->buffer.getNumTracks(); i++) {
		if (auto track =
			const_cast<juce::MidiMessageSequence*>(this->buffer.getTrack(i))) {
			track->updateMatchedPairs();
		}
	}

	/** Set Flag */
	this->changed();

	/** Callback */
	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});
}

void CloneableMIDISource::writeData(
	const juce::MidiBuffer& buffer, int offset) {
	/** Write To The Last Track */
	if (auto track = const_cast<juce::MidiMessageSequence*>(
		this->buffer.getTrack(this->buffer.getNumTracks() - 1))) {
		for (const auto& m : buffer) {
			double timeStamp = (m.samplePosition + offset) / this->getSampleRate();
			if (timeStamp >= 0) {
				auto mes = m.getMessage();
				mes.setTimeStamp(timeStamp);
				track->addEvent(mes);
			}
		}

		/** Set Flag */
		this->changed();
	}
}

juce::MidiFile* CloneableMIDISource::getMidiContentPtr() {
	return &(this->buffer);
}

#include "SourceMIDITemp.h"
#include "../Utils.h"

#define MIDI_LYRICS_TYPE 0x05
#define MIDI_LYRICS_TEMP_INIT { -1.0, "" }

void SourceMIDITemp::setData(const juce::MidiFile& data) {
	/** Get Time Format */
	this->timeFormat = data.getTimeFormat();
	
	/** Clear Lists */
	this->eventList.clear();

	this->noteList.clear();
	this->pitchWheelList.clear();
	this->afterTouchList.clear();
	this->channelPressureList.clear();
	this->controllerList.clear();
	this->miscList.clear();

	/** For Each Track */
	for (int i = 0; i < data.getNumTracks(); i++) {
		this->addTrack(*(data.getTrack(i)));
	}
}

void SourceMIDITemp::addTrack(const juce::MidiMessageSequence& track) {
	/** Add Track to List */
	this->eventList.add({});

	this->noteList.add({});
	this->pitchWheelList.add({});
	this->afterTouchList.add({});
	this->channelPressureList.add({});
	this->controllerList.add({});
	this->miscList.add({});

	/** Add Events */
	this->addEvents(this->eventList.size() - 1, track);
}

void SourceMIDITemp::removeEvents(int track, double startTime, double timeLength) {
	/** Check Index */
	if (track < 0 || track >= this->eventList.size()) {
		return;
	}

	/** Get Tracks */
	auto& eventList = this->eventList.getReference(track);

	/** Remove Events */
	double endTime = startTime + timeLength;
	for (int i = eventList.size() - 1; i >= 0; i--) {
		auto ptr = eventList.getUnchecked(i);
		if (ptr->timeSec >= startTime && ptr->timeSec <= endTime) {
			eventList.remove(i);
			continue;
		}
		/** Remove All Note Off Marker */
		if (auto ptrNoteOff = dynamic_cast<NoteOffMarker*>(ptr)) {
			eventList.remove(i);
			continue;
		}
		/** Remove Note In Time Area */
		if (auto ptrNote = dynamic_cast<Note*>(ptr)) {
			if (ptrNote->endSec >= startTime && ptrNote->timeSec <= endTime) {
				eventList.remove(i);
				continue;
			}
		}
	}

	/** Rebuild Note Off Markers */
	{
		/** Note Temp */
		using NoteOnItem = std::tuple<double, int>;
		auto noteOnCompFunc = [](NoteOnItem& a, NoteOnItem& b)->bool {
			return std::get<0>(a) > std::get<0>(b);
			};
		std::priority_queue<NoteOnItem, std::vector<NoteOnItem>, decltype(noteOnCompFunc)> noteOnObjectTemp;

		/** For Each Note */
		for (int i = 0; i < eventList.size(); i++) {
			if (auto ptr = eventList.getUnchecked(i)) {
				/** Note On Item */
				if (auto ptrNote = dynamic_cast<Note*>(ptr)) {
					noteOnObjectTemp.push({ ptrNote->endSec, i });
				}

				/** Insert Note Off Marker */
				if (i < eventList.size() - 1) {
					if (auto ptrNext = eventList.getUnchecked(i + 1)) {
						if (!noteOnObjectTemp.empty()) {
							auto& [firstNoteEndTime, firstNoteIndex] = noteOnObjectTemp.top();
							if (ptr->timeSec <= firstNoteEndTime && ptrNext->timeSec > firstNoteEndTime) {
								if (auto ptrNote = dynamic_cast<Note*>(eventList.getUnchecked(firstNoteIndex))) {
									auto noteOff = std::make_unique<NoteOffMarker>();
									noteOff->channel = ptrNote->channel;
									noteOff->timeSec = firstNoteEndTime;

									noteOff->eventOnIndex = firstNoteIndex;
									ptrNote->eventOffIndex = i + 1;

									noteOff->eventIndex = ptrNote->eventOffIndex;
									noteOff->eventInListIndex = -1;

									eventList.insert(ptrNote->eventOffIndex, std::move(noteOff));
								}
								noteOnObjectTemp.pop();
							}
						}
					}
				}
			}
		}

		/** Remained Notes */
		while (!noteOnObjectTemp.empty()) {
			auto& [firstNoteEndTime, firstNoteIndex] = noteOnObjectTemp.top();

			if (auto ptrNote = dynamic_cast<Note*>(eventList.getUnchecked(firstNoteIndex))) {
				auto noteOff = std::make_unique<NoteOffMarker>();
				noteOff->channel = ptrNote->channel;
				noteOff->timeSec = firstNoteEndTime;

				noteOff->eventOnIndex = firstNoteIndex;
				ptrNote->eventOffIndex = eventList.size();

				noteOff->eventIndex = ptrNote->eventOffIndex;
				noteOff->eventInListIndex = -1;

				eventList.add(std::move(noteOff));
			}

			noteOnObjectTemp.pop();
		}
	}
	
	/** Update Indexs */
	this->updateIndexs(track);
}

void SourceMIDITemp::addEvents(int track, const juce::MidiMessageSequence& list) {
	/** Check Index */
	if (track < 0 || track >= this->eventList.size()) {
		return;
	}

	/** Ensure Note Matched */
	juce::MidiMessageSequence listTemp{ list };
	listTemp.updateMatchedPairs(utils::regardVel0NoteAsNoteOff());

	/** Track Event Temp */
	LyricsItem lastLyrics = MIDI_LYRICS_TEMP_INIT;
	NoteOnTemp noteOnObjectTemp;
	int indexTemp = 0;

	/** Add Events */
	this->addMIDIMessages(
		track, listTemp, noteOnObjectTemp, indexTemp, lastLyrics);
}

const juce::MidiFile SourceMIDITemp::makeMIDIFile() const {
	juce::MidiFile file;
	utils::setMIDITimeFormat(file, this->timeFormat);
	
	for (int i = 0; i < this->eventList.size(); i++) {
		auto track = this->makeMIDITrack(i);
		file.addTrack(track);
	}

	return file;
}

const juce::MidiMessageSequence SourceMIDITemp::makeMIDITrack(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->eventList.size()) { return juce::MidiMessageSequence{}; }

	/** Temp */
	juce::MidiMessageSequence track;

	/** Get Events */
	int indexTemp = 0;
	this->findMIDIMessages(index, 0, DBL_MAX, track, indexTemp);
	
	/** Match Note On Off */
	track.updateMatchedPairs();

	return track;
}

int SourceMIDITemp::getTrackNum() const {
	return this->eventList.size();
}

double SourceMIDITemp::getLength() const {
	double result = 0;

	for (auto& track : this->eventList) {
		if (auto ptr = track.getLast()) {
			result = std::max(result, ptr->timeSec);
		}
	}

	return result;
}

bool SourceMIDITemp::isTrackEmpty(int track) const {
	if (track < 0 || track >= this->eventList.size()) {
		return true;
	}

	auto& trackRef = this->eventList.getReference(track);
	return trackRef.isEmpty();
}

int SourceMIDITemp::getNoteNum(int track) const {
	if (track < 0 || track >= this->noteList.size()) {
		return 0;
	}
	return this->noteList.getReference(track).size();
}

int SourceMIDITemp::getPitchWheelNum(int track) const {
	if (track < 0 || track >= this->pitchWheelList.size()) {
		return 0;
	}
	return this->pitchWheelList.getReference(track).size();
}

int SourceMIDITemp::getAfterTouchNum(int track) const {
	if (track < 0 || track >= this->afterTouchList.size()) {
		return 0;
	}
	return this->afterTouchList.getReference(track).size();
}

int SourceMIDITemp::getChannelPressureNum(int track) const {
	if (track < 0 || track >= this->channelPressureList.size()) {
		return 0;
	}
	return this->channelPressureList.getReference(track).size();
}

const std::set<uint8_t> SourceMIDITemp::getControllerNumbers(int track) const {
	if (track < 0 || track >= this->controllerList.size()) {
		return {};
	}

	std::set<uint8_t> result;
	auto& map = this->controllerList.getReference(track);
	for (auto& i : map) {
		result.insert(i.first);
	}

	return result;
}

int SourceMIDITemp::getControllerNum(int track, uint8_t number) const {
	if (track < 0 || track >= this->controllerList.size()) {
		return 0;
	}

	auto& map = this->controllerList.getReference(track);
	auto it = map.find(number);
	if (it == map.end()) {
		return 0;
	}

	return it->second.size();
}

int SourceMIDITemp::getMiscNum(int track) const {
	if (track < 0 || track >= this->miscList.size()) {
		return 0;
	}
	return this->miscList.getReference(track).size();
}

const SourceMIDITemp::Note SourceMIDITemp::getNote(int track, int index) const {
	if (track < 0 || track >= this->noteList.size()) {
		return {};
	}
	if (track < 0 || track >= this->eventList.size()) {
		return {};
	}

	auto& trackRef = this->noteList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	int structIndex = trackRef[index];
	auto& eventTrackRef = this->eventList.getReference(track);
	if (structIndex < 0 || structIndex >= eventTrackRef.size()) {
		return {};
	}

	return *(dynamic_cast<SourceMIDITemp::Note*>(
		eventTrackRef.getUnchecked(structIndex)));
}

const SourceMIDITemp::IntParam SourceMIDITemp::getPitchWheel(int track, int index) const {
	if (track < 0 || track >= this->pitchWheelList.size()) {
		return {};
	}
	if (track < 0 || track >= this->eventList.size()) {
		return {};
	}

	auto& trackRef = this->pitchWheelList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	int structIndex = trackRef[index];
	auto& eventTrackRef = this->eventList.getReference(track);
	if (structIndex < 0 || structIndex >= eventTrackRef.size()) {
		return {};
	}

	return *(dynamic_cast<SourceMIDITemp::IntParam*>(
		eventTrackRef.getUnchecked(structIndex)));
}

const SourceMIDITemp::AfterTouch SourceMIDITemp::getAfterTouch(int track, int index) const {
	if (track < 0 || track >= this->afterTouchList.size()) {
		return {};
	}
	if (track < 0 || track >= this->eventList.size()) {
		return {};
	}

	auto& trackRef = this->afterTouchList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	int structIndex = trackRef[index];
	auto& eventTrackRef = this->eventList.getReference(track);
	if (structIndex < 0 || structIndex >= eventTrackRef.size()) {
		return {};
	}

	return *(dynamic_cast<SourceMIDITemp::AfterTouch*>(
		eventTrackRef.getUnchecked(structIndex)));
}

const SourceMIDITemp::IntParam SourceMIDITemp::getChannelPressure(int track, int index) const {
	if (track < 0 || track >= this->channelPressureList.size()) {
		return {};
	}
	if (track < 0 || track >= this->eventList.size()) {
		return {};
	}

	auto& trackRef = this->channelPressureList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	int structIndex = trackRef[index];
	auto& eventTrackRef = this->eventList.getReference(track);
	if (structIndex < 0 || structIndex >= eventTrackRef.size()) {
		return {};
	}

	return *(dynamic_cast<SourceMIDITemp::IntParam*>(
		eventTrackRef.getUnchecked(structIndex)));
}

const SourceMIDITemp::Controller SourceMIDITemp::getController(int track, uint8_t number, int index) const {
	if (track < 0 || track >= this->controllerList.size()) {
		return {};
	}
	if (track < 0 || track >= this->eventList.size()) {
		return {};
	}

	auto& trackRef = this->controllerList.getReference(track);
	auto it = trackRef.find(number);
	if (it == trackRef.end()) {
		return {};
	}

	if (track < 0 || track >= it->second.size()) {
		return {};
	}

	int structIndex = it->second[index];
	auto& eventTrackRef = this->eventList.getReference(track);
	if (structIndex < 0 || structIndex >= eventTrackRef.size()) {
		return {};
	}

	return *(dynamic_cast<SourceMIDITemp::Controller*>(
		eventTrackRef.getUnchecked(structIndex)));
}

const SourceMIDITemp::Misc SourceMIDITemp::getMisc(int track, int index) const {
	if (track < 0 || track >= this->miscList.size()) {
		return {};
	}
	if (track < 0 || track >= this->eventList.size()) {
		return {};
	}

	auto& trackRef = this->miscList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	int structIndex = trackRef[index];
	auto& eventTrackRef = this->eventList.getReference(track);
	if (structIndex < 0 || structIndex >= eventTrackRef.size()) {
		return {};
	}

	return *(dynamic_cast<SourceMIDITemp::Misc*>(
		eventTrackRef.getUnchecked(structIndex)));
}

uint16_t SourceMIDITemp::makeNoteNumberWithChannel(uint8_t channel, uint8_t number) {
	return ((uint16_t)channel << 8) ^ (uint16_t)number;
}

void SourceMIDITemp::findMIDIMessages(
	int track, double startSec, double endSec,
	juce::MidiMessageSequence& list, int& indexTemp) const {
	/** Check Track */
	if (track < 0 || track >= this->eventList.size()) { return; }
	auto& trackSeq = this->eventList.getReference(track);

	/** Check Start Index */
	if (indexTemp < 0 || indexTemp >= trackSeq.size()) {
		indexTemp = SourceMIDITemp::binarySearchStart(trackSeq, 0, trackSeq.size() - 1, startSec);
	}
	else {
		auto current = trackSeq.getUnchecked(indexTemp);
		if (current->timeSec >= startSec) {
			if (auto last = trackSeq[indexTemp - 1]) {
				if (last->timeSec >= startSec) {
					indexTemp = SourceMIDITemp::binarySearchStart(trackSeq, 0, indexTemp, startSec);
				}
			}
		}
		else {
			indexTemp = SourceMIDITemp::binarySearchStart(trackSeq, indexTemp, trackSeq.size() - 1, startSec);
		}
	}
	if (indexTemp < 0) { return; }

	/** Events */
	for (; indexTemp < trackSeq.size(); indexTemp++) {
		auto eventPtr = trackSeq.getUnchecked(indexTemp);

		/** End */
		if (eventPtr->timeSec >= endSec) { break; }

		/** Note On */
		if (auto note = dynamic_cast<Note*>(eventPtr)) {
			/** Lyrics */
			if (note->lyrics.isNotEmpty()) {
				auto lyricsEvent = juce::MidiMessage::textMetaEvent(
					MIDI_LYRICS_TYPE, note->lyrics);
				lyricsEvent.setTimeStamp(note->timeSec);

				list.addEvent(lyricsEvent);
			}

			/** Event */
			auto onEvent = juce::MidiMessage::noteOn(
				note->channel, note->pitch, note->vel);
			onEvent.setTimeStamp(note->timeSec);

			list.addEvent(onEvent);
		}
		/** Note Off */
		else if (auto noteOff = dynamic_cast<NoteOffMarker*>(eventPtr)) {
			if (auto note = dynamic_cast<Note*>(trackSeq[noteOff->eventOnIndex])) {
				/** Event */
				auto offEvent = juce::MidiMessage::noteOff(
					note->channel, note->pitch, note->vel);
				offEvent.setTimeStamp(noteOff->timeSec);

				list.addEvent(offEvent);
			}
		}
		/** Pitch Wheel */
		else if (auto pitch = dynamic_cast<IntParam*>(eventPtr)) {
			/** Event */
			auto event = juce::MidiMessage::pitchWheel(
				pitch->channel, pitch->value);
			event.setTimeStamp(pitch->timeSec);

			list.addEvent(event);
		}
		/** After Touch */
		else if (auto afterTouch = dynamic_cast<AfterTouch*>(eventPtr)) {
			/** Event */
			auto event = juce::MidiMessage::aftertouchChange(
				afterTouch->channel, afterTouch->notePitch, afterTouch->value);
			event.setTimeStamp(afterTouch->timeSec);

			list.addEvent(event);
		}
		/** Channel Pressure */
		else if (auto channelPressure = dynamic_cast<IntParam*>(eventPtr)) {
			/** Event */
			auto event = juce::MidiMessage::channelPressureChange(
				channelPressure->channel, channelPressure->value);
			event.setTimeStamp(channelPressure->timeSec);

			list.addEvent(event);
		}
		/** Controller */
		else if (auto controller = dynamic_cast<Controller*>(eventPtr)) {
			/** Event */
			auto event = juce::MidiMessage::controllerEvent(
				controller->channel, controller->number, controller->value);
			event.setTimeStamp(controller->timeSec);

			list.addEvent(event);
		}
		/** Misc */
		else if (auto misc = dynamic_cast<Misc*>(eventPtr)) {
			/** Event */
			auto event = misc->message;
			event.setTimeStamp(misc->timeSec);

			list.addEvent(event);
		}
	}
}

void SourceMIDITemp::addMIDIMessages(
	int track, const juce::MidiMessageSequence& list,
	NoteOnTemp& noteOnTemp, int& indexTemp, LyricsItem& lyricsTemp) {
	/** Check Track */
	if (track < 0 || track >= this->eventList.size()) { return; }
	auto& trackSeq = this->eventList.getReference(track);

	/** Add Message */
	SourceMIDITemp::addMIDIMessagesInternal(
		trackSeq, list, noteOnTemp, indexTemp, lyricsTemp);

	/** Update Index */
	this->updateIndexs(track);
}

int SourceMIDITemp::addNote(int track, double startTime, double endTime, uint8_t channel,
	uint8_t pitch, uint8_t vel, const juce::String& lyrics) {
	/** Limit Track Index */
	if (track < 0 || track >= this->eventList.size()) { return -1; }
	
	/** Limit Time */
	if (endTime <= startTime) { return -1; }

	/** Get Insert Index */
	auto& list = this->eventList.getReference(track);
	int startIndex = SourceMIDITemp::linearSearchInsert(list, 0, startTime);

	/** Create Note Start Event */
	auto note = std::make_unique<Note>();
	note->channel = channel;
	note->timeSec = startTime;
	note->endSec = endTime;
	note->pitch = pitch;
	note->vel = vel;
	note->lyrics = lyrics;

	/** Insert Into Event List */
	note->eventIndex = startIndex;
	list.insert(startIndex, std::move(note));

	/** Update Event Index In Event List */
	for (int i = 0; i < list.size(); i++) {
		if (i != startIndex) {
			auto ptr = list.getUnchecked(i);
			if (ptr->eventIndex >= startIndex) {
				ptr->eventIndex++;
			}

			if (auto pNote = dynamic_cast<Note*>(ptr)) {
				if (pNote->eventOffIndex >= startIndex) {
					pNote->eventOffIndex++;
				}
			}

			else if (auto pNoteOff = dynamic_cast<NoteOffMarker*>(ptr)) {
				if (pNoteOff->eventOnIndex >= startIndex) {
					pNoteOff->eventOnIndex++;
				}
			}
		}
	}

	/** Get Note Off Insert Index */
	int endIndex = SourceMIDITemp::linearSearchInsert(
		list, startIndex, endTime);

	/** Create Note End Event */
	auto noteOff = std::make_unique<NoteOffMarker>();
	noteOff->channel = channel;
	noteOff->timeSec = endTime;
	noteOff->eventOnIndex = startIndex;

	/** Insert Note End Into Event List */
	noteOff->eventIndex = endIndex;
	list.insert(endIndex, std::move(noteOff));

	/** Update Note On */
	if (auto pNote = dynamic_cast<Note*>(list[startIndex])) {
		pNote->eventOffIndex = endIndex;
	}

	/** Update Event Index In Event List */
	for (int i = 0; i < list.size(); i++) {
		if (i != endIndex) {
			auto ptr = list.getUnchecked(i);
			if (ptr->eventIndex >= endIndex) {
				ptr->eventIndex++;
			}

			if (auto pNote = dynamic_cast<Note*>(ptr)) {
				if (pNote->eventIndex != startIndex &&
					pNote->eventOffIndex >= endIndex) {
					pNote->eventOffIndex++;
				}
			}

			else if (auto pNoteOff = dynamic_cast<NoteOffMarker*>(ptr)) {
				if (pNoteOff->eventOnIndex >= endIndex) {
					pNoteOff->eventOnIndex++;
				}
			}
		}
	}

	/** Rebuild Index Temp */
	this->updateIndexs(track);

	/** Return Note Index */
	if (auto pNote = dynamic_cast<Note*>(list[startIndex])) {
		return pNote->eventInListIndex;
	}
	return -1;
}

int SourceMIDITemp::setNoteTime(int track, int index,
	double startTime, double endTime) {
	/** Limit Track Index */
	if (track < 0 || track >= this->eventList.size()) { return -1; }

	/** Limit Time */
	if (endTime <= startTime) { return -1; }

	/** Get Note Data */
	auto& list = this->eventList.getReference(track);
	if (index < 0 || index >= list.size()) { return -1; }
	auto pNote = dynamic_cast<Note*>(list[index]);
	if (!pNote) { return -1; }

	uint8_t channel = pNote->channel;
	uint8_t pitch = pNote->pitch;
	uint8_t vel = pNote->vel;
	juce::String lyrics = pNote->lyrics;

	/** Change Note */
	if (this->removeNote(track, index)) {
		return this->addNote(
			track, startTime, endTime,
			channel, pitch, vel, lyrics);
	}
	return -1;
}

bool SourceMIDITemp::setNoteChannel(int track, int index, uint8_t channel) {
	/** Limit Track Index */
	if (track < 0 || track >= this->eventList.size()) { return false; }

	/** Get Note Pointer */
	auto& list = this->eventList.getReference(track);
	if (index < 0 || index >= list.size()) { return false; }
	auto pNote = dynamic_cast<Note*>(list[index]);
	if (!pNote) { return false; }

	/** Get Note Off */
	int offIndex = pNote->eventOffIndex;
	if (offIndex < 0 || offIndex >= list.size()) { return false; }
	auto pNoteOff = dynamic_cast<NoteOffMarker*>(list[offIndex]);
	if (!pNoteOff) { return false; }

	/** Set Channel */
	pNote->channel = channel;
	pNoteOff->channel = channel;

	return true;
}

bool SourceMIDITemp::setNotePitch(int track, int index, uint8_t pitch) {
	/** Limit Track Index */
	if (track < 0 || track >= this->eventList.size()) { return false; }

	/** Get Note Pointer */
	auto& list = this->eventList.getReference(track);
	if (index < 0 || index >= list.size()) { return false; }
	auto pNote = dynamic_cast<Note*>(list[index]);
	if (!pNote) { return false; }

	/** Set Pitch */
	pNote->pitch = pitch;

	return true;
}

bool SourceMIDITemp::setNoteVelocity(int track, int index, uint8_t vel) {
	/** Limit Track Index */
	if (track < 0 || track >= this->eventList.size()) { return false; }

	/** Get Note Pointer */
	auto& list = this->eventList.getReference(track);
	if (index < 0 || index >= list.size()) { return false; }
	auto pNote = dynamic_cast<Note*>(list[index]);
	if (!pNote) { return false; }

	/** Set Velocity */
	pNote->vel = vel;

	return true;
}

bool SourceMIDITemp::setNoteLyrics(int track, int index, const juce::String& lyrics) {
	/** Limit Track Index */
	if (track < 0 || track >= this->eventList.size()) { return false; }

	/** Get Note Pointer */
	auto& list = this->eventList.getReference(track);
	if (index < 0 || index >= list.size()) { return false; }
	auto pNote = dynamic_cast<Note*>(list[index]);
	if (!pNote) { return false; }

	/** Set Lyrics */
	pNote->lyrics = lyrics;

	return true;
}

bool SourceMIDITemp::removeNote(int track, int index) {
	/** Check Track Index */
	if (track < 0 || track >= this->eventList.size()) { return false; }

	/** Get End Index */
	auto& list = this->eventList.getReference(track);
	if (index < 0 || index >= list.size()) { return false; }
	int offIndex = -1;
	if (auto pNote = dynamic_cast<Note*>(list[index])) {
		offIndex = pNote->eventOffIndex;
	}
	if (offIndex < 0 || offIndex >= list.size()) { return false; }
	if (offIndex <= index) { return false; }

	/** Remove Note Off */
	list.remove(offIndex);

	/** Update Index */
	for (int i = 0; i < list.size(); i++) {
		if (i != offIndex) {
			auto ptr = list.getUnchecked(i);
			if (ptr->eventIndex > offIndex) {
				ptr->eventIndex--;
			}

			if (auto pNote = dynamic_cast<Note*>(ptr)) {
				if (pNote->eventIndex != index &&
					pNote->eventOffIndex > offIndex) {
					pNote->eventOffIndex--;
				}
			}

			else if (auto pNoteOff = dynamic_cast<NoteOffMarker*>(ptr)) {
				if (pNoteOff->eventOnIndex > offIndex) {
					pNoteOff->eventOnIndex--;
				}
			}
		}
	}

	/** Remove Note */
	list.remove(index);

	/** Update Index */
	for (int i = 0; i < list.size(); i++) {
		if (i != index) {
			auto ptr = list.getUnchecked(i);
			if (ptr->eventIndex > index) {
				ptr->eventIndex--;
			}

			if (auto pNote = dynamic_cast<Note*>(ptr)) {
				if (pNote->eventOffIndex > index) {
					pNote->eventOffIndex--;
				}
			}

			else if (auto pNoteOff = dynamic_cast<NoteOffMarker*>(ptr)) {
				if (pNoteOff->eventOnIndex > index) {
					pNoteOff->eventOnIndex--;
				}
			}
		}
	}

	/** Rebuild Index Temp */
	this->updateIndexs(track);

	return true;
}

int SourceMIDITemp::binarySearchStart(
	const juce::OwnedArray<MIDIStruct>& eventsList, int low, int high, double time) {

	while (low <= high) {
		int mid = low + (high - low) / 2;

		auto current = eventsList[mid];
		auto next = eventsList[mid + 1];
		if (!current) { return -1; }

		if (mid == low) {
			if (time <= current->timeSec) {
				return mid;
			}
		}
		if (mid == high) {
			if (time > current->timeSec) {
				if (next && time <= next->timeSec) {
					return mid;
				}
				return -1;
			}
		}

		if (time > current->timeSec && next && time <= next->timeSec) {
			return mid + 1;
		}
		else if (time <= current->timeSec) {
			high = mid - 1;
			continue;
		}
		else {
			low = mid + 1;
			continue;
		}
	}

	return -1;
}

int SourceMIDITemp::linearSearchInsert(
	const juce::OwnedArray<MIDIStruct>& eventsList, int indexStart, double time) {
	/** First */
	if (indexStart == 0 && eventsList.size() > 0
		&& time < eventsList.getFirst()->timeSec) {
		return 0;
	}

	/** Search For Each Events */
	for (int i = indexStart; i < eventsList.size() - 1; i++) {
		auto current = eventsList[i];
		auto next = eventsList[i + 1];
		if (time >= current->timeSec && time < next->timeSec) {
			return i + 1;
		}
	}

	/** Last */
	return eventsList.size();
}

int SourceMIDITemp::getIndexTempInsertIndex(int index,
	juce::OwnedArray<MIDIStruct>& eventsList,
	const juce::MidiMessage& message) {
	for (int i = index - 1; i >= 0; i--) {
		auto current = eventsList[i];

		/** Get Notes */
		if (message.isNoteOn(!utils::regardVel0NoteAsNoteOff())) {
			if (auto note = dynamic_cast<Note*>(current)) {
				return note->eventInListIndex + 1;
			}
		}
		/** Get Lyrics */
		else if (message.isMetaEvent() && message.getMetaEventType() == MIDI_LYRICS_TYPE) {
			return -1;
		}
		/** Note Off Marker */
		else if (message.isNoteOff(utils::regardVel0NoteAsNoteOff())) {
			return -1;
		}
		/** Pitch Wheel */
		else if (message.isPitchWheel()) {
			if (auto pitchWheel = dynamic_cast<IntParam*>(current)) {
				return pitchWheel->eventInListIndex + 1;
			}
		}
		/** After Touch */
		else if (message.isAftertouch()) {
			if (auto afterTouch = dynamic_cast<AfterTouch*>(current)) {
				return afterTouch->eventInListIndex + 1;
			}
		}
		/** Channel Pressure */
		else if (message.isChannelPressure()) {
			if (auto channelPressure = dynamic_cast<IntParam*>(current)) {
				return channelPressure->eventInListIndex + 1;
			}
		}
		/** MIDI CC */
		else if (message.isController()) {
			if (auto controller = dynamic_cast<Controller*>(current)) {
				return controller->eventInListIndex + 1;
			}
		}
		/** Other exclude Lyrics */
		else {
			if (auto misc = dynamic_cast<Misc*>(current)) {
				return misc->eventInListIndex + 1;
			}
		}
	}

	/** First */
	return 0;
}

void SourceMIDITemp::addMIDIMessagesInternal(
	juce::OwnedArray<MIDIStruct>& eventsList,
	const juce::MidiMessageSequence& list,
	NoteOnTemp& noteOnTemp, int& indexTemp, LyricsItem& lyricsTemp) {
	for (auto event : list) {
		SourceMIDITemp::addMIDIMessageInternal(eventsList,
			event->message, noteOnTemp, indexTemp, lyricsTemp);
	}
}

void SourceMIDITemp::addMIDIMessageInternal(
	juce::OwnedArray<MIDIStruct>& eventsList,
	const juce::MidiMessage& message,
	NoteOnTemp& noteOnTemp, int& indexTemp, LyricsItem& lyricsTemp) {
	/** Select Insert Index And Update Index Temp */
	if (indexTemp <= 0 || indexTemp > eventsList.size()) {
		indexTemp = SourceMIDITemp::linearSearchInsert(eventsList, 0, message.getTimeStamp());
	}
	else {
		auto last = eventsList[indexTemp - 1];
		if (message.getTimeStamp() < last->timeSec) {
			indexTemp = SourceMIDITemp::linearSearchInsert(eventsList, 0, message.getTimeStamp());
		}
		else {
			if (auto next = eventsList[indexTemp]) {
				if (message.getTimeStamp() > next->timeSec) {
					indexTemp = SourceMIDITemp::linearSearchInsert(eventsList, indexTemp, message.getTimeStamp());
				}
			}
		}
	}

	/** Get Notes */
	if (message.isNoteOn(!utils::regardVel0NoteAsNoteOff())) {
		auto note = std::make_unique<Note>();
		note->channel = (uint8_t)message.getChannel();
		note->timeSec = message.getTimeStamp();
		note->endSec = note->timeSec;
		note->pitch = (uint8_t)message.getNoteNumber();
		note->vel = message.getVelocity();

		if (juce::approximatelyEqual(std::get<0>(lyricsTemp), note->timeSec)) {
			note->lyrics = std::get<1>(lyricsTemp);
			lyricsTemp = MIDI_LYRICS_TEMP_INIT;
		}

		note->eventOffIndex = -1;
		noteOnTemp[SourceMIDITemp::makeNoteNumberWithChannel(note->channel, note->pitch)] = indexTemp;

		note->eventIndex = indexTemp;

		eventsList.insert(indexTemp, std::move(note));
	}
	/** Get Lyrics */
	else if (message.isMetaEvent() && message.getMetaEventType() == MIDI_LYRICS_TYPE) {
		lyricsTemp = { message.getTimeStamp(), message.getTextFromTextMetaEvent() };

		/** Pass Though */
		return;
	}
	/** Note Off Marker */
	else if (message.isNoteOff(utils::regardVel0NoteAsNoteOff())) {
		auto noteOff = std::make_unique<NoteOffMarker>();
		noteOff->channel = (uint8_t)message.getChannel();
		noteOff->timeSec = message.getTimeStamp();

		noteOff->eventOnIndex = -1;
		{
			auto tempIt = noteOnTemp.find(SourceMIDITemp::makeNoteNumberWithChannel(
				(uint8_t)message.getChannel(), (uint8_t)message.getNoteNumber()));
			if (tempIt != noteOnTemp.end()) {
				int noteIndex = tempIt->second;
				if (noteIndex >= 0 && noteIndex < indexTemp) {
					if (auto note = dynamic_cast<Note*>(eventsList[noteIndex])) {
						note->endSec = noteOff->timeSec;
						note->eventOffIndex = indexTemp;

						noteOff->eventOnIndex = noteIndex;
					}
				}

				noteOnTemp.erase(tempIt);
			}
		}

		noteOff->eventIndex = indexTemp;

		eventsList.insert(indexTemp, std::move(noteOff));
	}
	/** Pitch Wheel */
	else if (message.isPitchWheel()) {
		auto param = std::make_unique<IntParam>();
		param->channel = (uint8_t)message.getChannel();
		param->timeSec = message.getTimeStamp();
		param->value = message.getPitchWheelValue();

		param->eventIndex = indexTemp;

		eventsList.insert(indexTemp, std::move(param));
	}
	/** After Touch */
	else if (message.isAftertouch()) {
		auto param = std::make_unique<AfterTouch>();
		param->channel = (uint8_t)message.getChannel();
		param->timeSec = message.getTimeStamp();
		param->notePitch = (uint8_t)message.getNoteNumber();
		param->value = (uint8_t)message.getAfterTouchValue();

		param->eventIndex = indexTemp;

		eventsList.insert(indexTemp, std::move(param));
	}
	/** Channel Pressure */
	else if (message.isChannelPressure()) {
		auto param = std::make_unique<IntParam>();
		param->channel = (uint8_t)message.getChannel();
		param->timeSec = message.getTimeStamp();
		param->value = message.getChannelPressureValue();

		param->eventIndex = indexTemp;

		eventsList.insert(indexTemp, std::move(param));
	}
	/** MIDI CC */
	else if (message.isController()) {
		auto controller = std::make_unique<Controller>();
		controller->channel = (uint8_t)message.getChannel();
		controller->timeSec = message.getTimeStamp();
		controller->number = (uint8_t)message.getControllerNumber();
		controller->value = (uint8_t)message.getControllerValue();

		controller->eventIndex = indexTemp;

		eventsList.insert(indexTemp, std::move(controller));
	}
	/** Other exclude Lyrics */
	else {
		auto misc = std::make_unique<Misc>();
		misc->channel = (message.isSysEx() || message.isMetaEvent())
			? 0 : (uint8_t)message.getChannel();
		misc->timeSec = message.getTimeStamp();
		misc->message = message;

		misc->eventIndex = indexTemp;

		eventsList.insert(indexTemp, std::move(misc));
	}

	/** Increase Events List Index */
	for (int i = indexTemp + 1; i < eventsList.size(); i++) {
		auto current = eventsList[i];
		current->eventIndex++;
	}

	/** Increase Index Temp */
	indexTemp++;
}

void SourceMIDITemp::updateIndexs(int track) {
	/** Check Index */
	if (track < 0 || track >= this->eventList.size()) {
		return;
	}

	/** Get Tracks */
	auto& eventList = this->eventList.getReference(track);

	auto& noteList = this->noteList.getReference(track);
	auto& pitchWheelList = this->pitchWheelList.getReference(track);
	auto& afterTouchList = this->afterTouchList.getReference(track);
	auto& channelPressureList = this->channelPressureList.getReference(track);
	auto& controllerList = this->controllerList.getReference(track);
	auto& miscList = this->miscList.getReference(track);

	/** Clear Index List */
	noteList.clear();
	pitchWheelList.clear();
	afterTouchList.clear();
	channelPressureList.clear();
	controllerList.clear();
	miscList.clear();

	/** Rebuild Indexs */
	for (int i = 0; i < eventList.size(); i++) {
		auto ptr = eventList.getUnchecked(i);
		ptr->eventIndex = i;

		/** Note On */
		if (auto note = dynamic_cast<Note*>(ptr)) {
			note->eventInListIndex = noteList.size();
			noteList.add(i);
		}
		/** Pitch Wheel */
		else if (auto pitch = dynamic_cast<IntParam*>(ptr)) {
			pitch->eventInListIndex = pitchWheelList.size();
			pitchWheelList.add(i);
		}
		/** After Touch */
		else if (auto afterTouch = dynamic_cast<AfterTouch*>(ptr)) {
			afterTouch->eventInListIndex = afterTouchList.size();
			afterTouchList.add(i);
		}
		/** Channel Pressure */
		else if (auto channelPressure = dynamic_cast<IntParam*>(ptr)) {
			channelPressure->eventInListIndex = channelPressureList.size();
			channelPressureList.add(i);
		}
		/** Controller */
		else if (auto controller = dynamic_cast<Controller*>(ptr)) {
			auto& controllers = controllerList[controller->number];

			controller->eventInListIndex = controllers.size();
			controllers.add(i);
		}
		/** Misc */
		else if (auto misc = dynamic_cast<Misc*>(ptr)) {
			misc->eventInListIndex = miscList.size();
			miscList.add(i);
		}
	}
}

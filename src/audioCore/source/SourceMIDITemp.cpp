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
	/** Ensure Note Matched */
	juce::MidiMessageSequence trackTemp{ track };
	trackTemp.updateMatchedPairs(utils::regardVel0NoteAsNoteOff());

	/** Track Event Temp */
	LyricsItem lastLyrics = MIDI_LYRICS_TEMP_INIT;
	NoteOnTemp noteOnObjectTemp;

	juce::OwnedArray<MIDIStruct> events;

	juce::Array<int> noteTrack;
	juce::Array<int> pitchWheel, channelPressure;
	juce::Array<int> afterTouch;
	std::unordered_map<uint8_t, juce::Array<int>> controllers;
	juce::Array<int> miscs;

	int indexTemp = 0;

	/** Add Events */
	this->addMIDIMessages(
		events, noteTrack, pitchWheel, channelPressure, afterTouch, controllers, miscs,
		trackTemp, noteOnObjectTemp, indexTemp, lastLyrics);

	/** Add Track to List */
	this->eventList.add(std::move(events));

	this->noteList.add(noteTrack);
	this->pitchWheelList.add(pitchWheel);
	this->afterTouchList.add(afterTouch);
	this->channelPressureList.add(channelPressure);
	this->controllerList.add(controllers);
	this->miscList.add(miscs);
}

void SourceMIDITemp::removeEvents(int track, double startTime, double timeLength) {
	/** TODO */
}

void SourceMIDITemp::addEvents(int track, const juce::MidiMessageSequence& list) {
	/** TODO */
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

	/** Index Lists */
	auto& noteSeq = this->noteList.getReference(track);
	auto& pitchWheelSeq = this->pitchWheelList.getReference(track);
	auto& channelPressureSeq = this->channelPressureList.getReference(track);
	auto& afterTouchSeq = this->afterTouchList.getReference(track);
	auto& controllerSeq = this->controllerList.getReference(track);
	auto& miscSeq = this->miscList.getReference(track);

	SourceMIDITemp::addMIDIMessages(
		trackSeq, noteSeq, pitchWheelSeq, channelPressureSeq, afterTouchSeq, controllerSeq, miscSeq,
		list, noteOnTemp, indexTemp, lyricsTemp);
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

void SourceMIDITemp::addMIDIMessages(
	juce::OwnedArray<MIDIStruct>& eventsList,
	juce::Array<int>& noteTrackIndexList,
	juce::Array<int>& pitchWheelIndexList,
	juce::Array<int>& channelPressureIndexList,
	juce::Array<int>& afterTouchIndexList,
	std::unordered_map<uint8_t, juce::Array<int>>& controllersIndexList,
	juce::Array<int>& miscsIndexList,
	const juce::MidiMessageSequence& list,
	NoteOnTemp& noteOnTemp, int& indexTemp, LyricsItem& lyricsTemp) {
	for (auto event : list) {
		SourceMIDITemp::addMIDIMessage(eventsList, noteTrackIndexList, pitchWheelIndexList,
			channelPressureIndexList, afterTouchIndexList, controllersIndexList, miscsIndexList,
			event->message, noteOnTemp, indexTemp, lyricsTemp);
	}
}

void SourceMIDITemp::addMIDIMessage(
	juce::OwnedArray<MIDIStruct>& eventsList,
	juce::Array<int>& noteTrackIndexList,
	juce::Array<int>& pitchWheelIndexList,
	juce::Array<int>& channelPressureIndexList,
	juce::Array<int>& afterTouchIndexList,
	std::unordered_map<uint8_t, juce::Array<int>>& controllersIndexList,
	juce::Array<int>& miscsIndexList,
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

	/** Get Index Temp Index */
	int listIndex = SourceMIDITemp::getIndexTempInsertIndex(
		indexTemp, eventsList, message);

	/** Index List Ptr */
	juce::Array<int>* indexListPtr = nullptr;

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
		note->eventInListIndex = listIndex;

		eventsList.insert(indexTemp, std::move(note));
		noteTrackIndexList.insert(listIndex, indexTemp);

		indexListPtr = &noteTrackIndexList;
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
		noteOff->eventInListIndex = listIndex;

		eventsList.insert(indexTemp, std::move(noteOff));
	}
	/** Pitch Wheel */
	else if (message.isPitchWheel()) {
		auto param = std::make_unique<IntParam>();
		param->channel = (uint8_t)message.getChannel();
		param->timeSec = message.getTimeStamp();
		param->value = message.getPitchWheelValue();

		param->eventIndex = indexTemp;
		param->eventInListIndex = listIndex;

		eventsList.insert(indexTemp, std::move(param));
		pitchWheelIndexList.insert(listIndex, indexTemp);

		indexListPtr = &pitchWheelIndexList;
	}
	/** After Touch */
	else if (message.isAftertouch()) {
		auto param = std::make_unique<AfterTouch>();
		param->channel = (uint8_t)message.getChannel();
		param->timeSec = message.getTimeStamp();
		param->notePitch = (uint8_t)message.getNoteNumber();
		param->value = (uint8_t)message.getAfterTouchValue();

		param->eventIndex = indexTemp;
		param->eventInListIndex = listIndex;

		eventsList.insert(indexTemp, std::move(param));
		afterTouchIndexList.insert(listIndex, indexTemp);

		indexListPtr = &afterTouchIndexList;
	}
	/** Channel Pressure */
	else if (message.isChannelPressure()) {
		auto param = std::make_unique<IntParam>();
		param->channel = (uint8_t)message.getChannel();
		param->timeSec = message.getTimeStamp();
		param->value = message.getChannelPressureValue();

		param->eventIndex = indexTemp;
		param->eventInListIndex = listIndex;

		eventsList.insert(indexTemp, std::move(param));
		channelPressureIndexList.insert(listIndex, indexTemp);

		indexListPtr = &channelPressureIndexList;
	}
	/** MIDI CC */
	else if (message.isController()) {
		auto controller = std::make_unique<Controller>();
		controller->channel = (uint8_t)message.getChannel();
		controller->timeSec = message.getTimeStamp();
		controller->number = (uint8_t)message.getControllerNumber();
		controller->value = (uint8_t)message.getControllerValue();

		auto& controllerList = controllersIndexList[controller->number];

		controller->eventIndex = indexTemp;
		controller->eventInListIndex = listIndex;

		eventsList.insert(indexTemp, std::move(controller));
		controllerList.insert(listIndex, indexTemp);

		indexListPtr = &controllerList;
	}
	/** Other exclude Lyrics */
	else {
		auto misc = std::make_unique<Misc>();
		misc->channel = (message.isSysEx() || message.isMetaEvent())
			? 0 : (uint8_t)message.getChannel();
		misc->timeSec = message.getTimeStamp();
		misc->message = message;

		misc->eventIndex = indexTemp;
		misc->eventInListIndex = listIndex;

		eventsList.insert(indexTemp, std::move(misc));
		miscsIndexList.insert(listIndex, indexTemp);

		indexListPtr = &miscsIndexList;
	}

	/** Increase Events List Index */
	for (int i = indexTemp + 1; i < eventsList.size(); i++) {
		auto current = eventsList[i];
		current->eventIndex++;
	}

	if (indexListPtr) {
		/** Increase Index List Index */
		auto newItem = eventsList[indexTemp];
		for (int i = indexTemp + 1; i < eventsList.size(); i++) {
			auto current = eventsList[i];
			if (typeid(*newItem) == typeid(*current)) {
				current->eventInListIndex++;
			}
		}

		/** Increase Index List Data */
		for (int i = listIndex + 1; i < indexListPtr->size(); i++) {
			(indexListPtr->getReference(i))++;
		}
	}

	/** Increase Index Temp */
	indexTemp++;
}

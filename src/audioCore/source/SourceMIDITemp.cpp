#include "SourceMIDITemp.h"

void SourceMIDITemp::setData(const juce::MidiFile& data) {
	this->sourceData = data;

	this->update();
}

void SourceMIDITemp::addTrack(const juce::MidiMessageSequence& track) {
	this->sourceData.addTrack(track);

	this->update();
}

void SourceMIDITemp::update() {
	/** Clear Lists */
	this->noteList.clear();
	this->sustainPedalList.clear();
	this->sostenutoPedalList.clear();
	this->softPedalList.clear();
	this->pitchWheelList.clear();
	this->afterTouchList.clear();
	this->channelPressureList.clear();
	this->controllerList.clear();
	this->miscList.clear();

	/** For Each Track */
	for (int i = 0; i < this->sourceData.getNumTracks(); i++) {
		juce::MidiMessageSequence track{ *(this->sourceData.getTrack(i)) };
		track.updateMatchedPairs();
		double endTime = track.getEndTime();

		/** Track Event Temp */
		juce::Array<Note> noteTrack;
		juce::String lyricsTemp;
		double lyricsTimeTemp = 0;

		juce::Array<Pedal> sustain, sostenuto, soft;
		juce::Array<IntParam> pitchWheel, channelPressure;
		juce::Array<AfterTouch> afterTouch;
		std::unordered_map<uint8_t, juce::Array<Controller>> controllers;
		juce::Array<Misc> miscs;

		/** For Each Event */
		for (int i = 0; i < track.getNumEvents(); i++) {
			auto event = track.getEventPointer(i);

			/** Get Notes */
			if (event->message.isNoteOn(true)) {
				Note note{};
				note.channel = (uint8_t)event->message.getChannel();
				note.startSec = event->message.getTimeStamp();
				note.endSec = event->noteOffObject ? event->noteOffObject->message.getTimeStamp() : endTime;
				note.pitch = (uint8_t)event->message.getNoteNumber();
				note.vel = event->message.getVelocity();
				if (note.startSec == lyricsTimeTemp) {
					note.lyrics = lyricsTemp;
				}
				note.noteOnEvent = i;

				noteTrack.add(note);
				continue;
			}
			/** Get Lyrics */
			if (event->message.isMetaEvent() && event->message.getMetaEventType() == 0x05) {
				lyricsTemp = event->message.getTextFromTextMetaEvent();
				lyricsTimeTemp = event->message.getTimeStamp();
				continue;
			}
			/** Sustain Pedal */
			if (event->message.isSustainPedalOn() || event->message.isSustainPedalOff()) {
				Pedal pedal{};
				pedal.channel = (uint8_t)event->message.getChannel();
				pedal.timeSec = event->message.getTimeStamp();
				pedal.value = event->message.isSustainPedalOn();
				pedal.event = i;

				sustain.add(pedal);
				continue;
			}
			/** Sostenuto Pedal */
			if (event->message.isSostenutoPedalOn() || event->message.isSostenutoPedalOff()) {
				Pedal pedal{};
				pedal.channel = (uint8_t)event->message.getChannel();
				pedal.timeSec = event->message.getTimeStamp();
				pedal.value = event->message.isSostenutoPedalOn();
				pedal.event = i;

				sostenuto.add(pedal);
				continue;
			}
			/** Soft Pedal */
			if (event->message.isSoftPedalOn() || event->message.isSoftPedalOff()) {
				Pedal pedal{};
				pedal.channel = (uint8_t)event->message.getChannel();
				pedal.timeSec = event->message.getTimeStamp();
				pedal.value = event->message.isSoftPedalOn();
				pedal.event = i;

				soft.add(pedal);
				continue;
			}
			/** Pitch Wheel */
			if (event->message.isPitchWheel()) {
				IntParam param{};
				param.channel = (uint8_t)event->message.getChannel();
				param.timeSec = event->message.getTimeStamp();
				param.value = event->message.getPitchWheelValue();
				param.event = i;

				pitchWheel.add(param);
				continue;
			}
			/** After Touch */
			if (event->message.isAftertouch()) {
				AfterTouch param{};
				param.channel = (uint8_t)event->message.getChannel();
				param.timeSec = event->message.getTimeStamp();
				param.notePitch = (uint8_t)event->message.getNoteNumber();
				param.value = (uint8_t)event->message.getAfterTouchValue();
				param.event = i;

				afterTouch.add(param);
				continue;
			}
			/** Channel Pressure */
			if (event->message.isChannelPressure()) {
				IntParam param{};
				param.channel = (uint8_t)event->message.getChannel();
				param.timeSec = event->message.getTimeStamp();
				param.value = event->message.getChannelPressureValue();
				param.event = i;

				channelPressure.add(param);
				continue;
			}
			/** MIDI CC */
			if (event->message.isController()) {
				Controller controller{};
				controller.channel = (uint8_t)event->message.getChannel();
				controller.timeSec = event->message.getTimeStamp();
				controller.number = (uint8_t)event->message.getControllerNumber();
				controller.value = (uint8_t)event->message.getControllerValue();
				controller.event = i;

				controllers[controller.number].add(controller);
				continue;
			}
			/** Other exclude Lyrics */
			{
				Misc misc{};
				misc.channel = (uint8_t)event->message.getChannel();
				misc.timeSec = event->message.getTimeStamp();
				misc.message = event->message;
				misc.event = i;

				miscs.add(misc);
				continue;
			}
		}

		/** Add Track to List */
		this->noteList.add(noteTrack);
		this->sustainPedalList.add(sustain);
		this->sostenutoPedalList.add(sostenuto);
		this->softPedalList.add(soft);
		this->pitchWheelList.add(pitchWheel);
		this->afterTouchList.add(afterTouch);
		this->channelPressureList.add(channelPressure);
		this->controllerList.add(controllers);
		this->miscList.add(miscs);
	}
}

juce::MidiFile* SourceMIDITemp::getSourceData() {
	return &(this->sourceData);
}

int SourceMIDITemp::getTrackNum() const {
	return this->sourceData.getNumTracks();
}

int SourceMIDITemp::getNoteNum(int track) const {
	if (track < 0 || track >= this->noteList.size()) {
		return 0;
	}
	return this->noteList.getReference(track).size();
}

int SourceMIDITemp::getSustainPedalNum(int track) const {
	if (track < 0 || track >= this->sustainPedalList.size()) {
		return 0;
	}
	return this->sustainPedalList.getReference(track).size();
}

int SourceMIDITemp::getSostenutoPedalNum(int track) const {
	if (track < 0 || track >= this->sostenutoPedalList.size()) {
		return 0;
	}
	return this->sostenutoPedalList.getReference(track).size();
}

int SourceMIDITemp::getSoftPedalNum(int track) const {
	if (track < 0 || track >= this->softPedalList.size()) {
		return 0;
	}
	return this->softPedalList.getReference(track).size();
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

	auto& trackRef = this->noteList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	return trackRef.getUnchecked(index);
}

const SourceMIDITemp::Pedal SourceMIDITemp::getSustainPedal(int track, int index) const {
	if (track < 0 || track >= this->sustainPedalList.size()) {
		return {};
	}

	auto& trackRef = this->sustainPedalList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	return trackRef.getUnchecked(index);
}

const SourceMIDITemp::Pedal SourceMIDITemp::getSostenutoPedal(int track, int index) const {
	if (track < 0 || track >= this->sostenutoPedalList.size()) {
		return {};
	}

	auto& trackRef = this->sostenutoPedalList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	return trackRef.getUnchecked(index);
}

const SourceMIDITemp::Pedal SourceMIDITemp::getSoftPedal(int track, int index) const {
	if (track < 0 || track >= this->softPedalList.size()) {
		return {};
	}

	auto& trackRef = this->softPedalList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	return trackRef.getUnchecked(index);
}

const SourceMIDITemp::IntParam SourceMIDITemp::getPitchWheel(int track, int index) const {
	if (track < 0 || track >= this->pitchWheelList.size()) {
		return {};
	}

	auto& trackRef = this->pitchWheelList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	return trackRef.getUnchecked(index);
}

const SourceMIDITemp::AfterTouch SourceMIDITemp::getAfterTouch(int track, int index) const {
	if (track < 0 || track >= this->afterTouchList.size()) {
		return {};
	}

	auto& trackRef = this->afterTouchList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	return trackRef.getUnchecked(index);
}

const SourceMIDITemp::IntParam SourceMIDITemp::getChannelPressure(int track, int index) const {
	if (track < 0 || track >= this->channelPressureList.size()) {
		return {};
	}

	auto& trackRef = this->channelPressureList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	return trackRef.getUnchecked(index);
}

const SourceMIDITemp::Controller SourceMIDITemp::getController(int track, uint8_t number, int index) const {
	if (track < 0 || track >= this->controllerList.size()) {
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

	return it->second.getUnchecked(index);
}

const SourceMIDITemp::Misc SourceMIDITemp::getMisc(int track, int index) const {
	if (track < 0 || track >= this->miscList.size()) {
		return {};
	}

	auto& trackRef = this->miscList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	return trackRef.getUnchecked(index);
}

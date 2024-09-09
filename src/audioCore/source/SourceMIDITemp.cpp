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
		auto track = this->sourceData.getTrack(i);
		double endTime = track->getEndTime();

		/** Track Event Temp */
		juce::Array<Note> noteTrack;
		juce::String lyricsTemp;
		double lyricsTimeTemp = 0;

		juce::Array<Pedal> sustain, sostenuto, soft;
		juce::Array<IntParam> pitchWheel, afterTouch, channelPressure;
		std::unordered_map<uint8_t, juce::Array<Controller>> controllers;
		juce::Array<Misc> miscs;

		/** For Each Event */
		for (int i = 0; i < track->getNumEvents(); i++) {
			auto event = track->getEventPointer(i);

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
				IntParam param{};
				param.channel = (uint8_t)event->message.getChannel();
				param.timeSec = event->message.getTimeStamp();
				param.value = event->message.getAfterTouchValue();
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

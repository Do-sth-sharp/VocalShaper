#include "TempoTemp.h"

void TempoTemp::update(
	juce::MidiMessageSequence& tempoMessages) {
	juce::MidiMessageSequence tMes, bMes;
	for (auto& i : tempoMessages) {
		auto& mes = i->message;
		if (mes.isTempoMetaEvent()) {
			tMes.addEvent(mes);
		}
		else if (mes.isTimeSignatureMetaEvent()) {
			bMes.addEvent(mes);
		}
	}

	this->updateTempo(tMes);
	this->updateBeat(bMes);
}

void TempoTemp::updateTempo(
	juce::MidiMessageSequence& tempoMessages) {
	/** Clear Temp */
	this->tempoTemp.clear();
	this->lastTempoIndex = -1;

	/** Last Time Temp */
	double teTemp = 0.;

	/** Get Each Tempo Event */
	int mesNum = tempoMessages.getNumEvents();
	for (int i = 0; i < mesNum; i++) {
		auto currentEvent = tempoMessages.getEventPointer(i);

		/** Value Temp */
		double xs = 0, xe = 0, ts = 0, te = 0, T = 0;

		/** Not The Last Event */
		if (i < mesNum - 1) {
			auto nextEvent = tempoMessages.getEventPointer(i + 1);

			xs = currentEvent->message.getTimeStamp();
			xe = nextEvent->message.getTimeStamp();
			T = currentEvent->message.getTempoSecondsPerQuarterNote();
			ts = teTemp;
			te = teTemp = TempoTemp::tFuncBeat(xe, xs, T, ts);
		}
		/** The Last Event */
		else {
			xs = currentEvent->message.getTimeStamp();
			xe = DBL_MAX;
			T = currentEvent->message.getTempoSecondsPerQuarterNote();
			ts = teTemp;
			te = teTemp = DBL_MAX;
		}

		/** Add To List */
		this->tempoTemp.add({ xs, xe, ts, te, T });
	}
}

void TempoTemp::updateBeat(
	juce::MidiMessageSequence& beatMessages) {
	/** Clear Temp */
	this->beatTemp.clear();
	this->lastBeatIndex = -1;

	/** TODO */
}

double TempoTemp::tFuncBeat(
	double x, double xs, double T, double ts) {
	/** t(x) = ((x - xs) / T) * 60 + ts */
	return ((x - xs) / T) * 60. + ts;
}

double TempoTemp::xFuncBeat(
	double t, double ts, double T, double xs) {
	/** x(t) = ((t - ts) / 60) * T + xs */
	return ((t - ts) / 60.) * T + xs;
}

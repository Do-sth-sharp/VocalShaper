#include "PlayPosition.h"

#include <chrono>
#include "../uiCallback/UICallback.h"
#include "../misc/AudioLock.h"

juce::Optional<juce::AudioPlayHead::PositionInfo> MovablePlayHead::getPosition() const {
	/** Get System Clock Nanoseconds */
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::nanoseconds ns
		= std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
	this->position.setHostTimeNs((uint64_t)ns.count());

	return juce::makeOptional(this->position);
}

bool MovablePlayHead::canControlTransport() {
	//return juce::MessageManager::existsAndIsCurrentThread();
	return true;
}

void MovablePlayHead::transportPlay(bool shouldStartPlaying) {
	juce::ScopedWriteLock locker(audioLock::getPositionLock());
	this->position.setIsPlaying(shouldStartPlaying);
	if (!shouldStartPlaying) {
		this->position.setIsRecording(false);
		this->overflowFlag = false;
	}

	UICallbackAPI<bool>::invoke(
		UICallbackType::PlayStateChanged, shouldStartPlaying);
}

void MovablePlayHead::transportRecord(bool shouldStartRecording) {
	juce::ScopedWriteLock locker(audioLock::getPositionLock());
	this->position.setIsRecording(shouldStartRecording);
	if (shouldStartRecording) {
		this->position.setIsPlaying(true);
	}

	UICallbackAPI<bool>::invoke(
		UICallbackType::RecordStateChanged, shouldStartRecording);
}

void MovablePlayHead::transportRewind() {
	juce::ScopedWriteLock locker(audioLock::getPositionLock());
	this->position.setTimeInSamples(0);
	this->position.setTimeInSeconds(0);
	this->position.setBarCount(0);
	this->position.setPpqPositionOfLastBarStart(0);
	this->position.setPpqPosition(0);
	this->overflowFlag = false;
}

void MovablePlayHead::setTimeFormat(short ticksPerQuarter) {
	juce::ScopedWriteLock locker(audioLock::getPositionLock());
	this->timeFormat = ticksPerQuarter;
}

void MovablePlayHead::setSampleRate(double sampleRate) {
	juce::ScopedWriteLock locker(audioLock::getPositionLock());
	this->sampleRate = sampleRate;

	this->position.setTimeInSamples(
		(int64_t)std::floor(this->position.getTimeInSeconds().orFallback(0) * this->sampleRate));
}

void MovablePlayHead::setLooping(bool looping) {
	juce::ScopedWriteLock locker(audioLock::getPositionLock());
	this->position.setIsLooping(looping);
}

void MovablePlayHead::setLoopPointsInSeconds(const std::tuple<double, double>& points) {
	juce::ScopedWriteLock locker(audioLock::getPositionLock());

	std::tie(this->loopStartSec, this->loopEndSec) = points;
	double startQuarter = this->toQuarter(this->loopStartSec);
	double endQuarter = this->toQuarter(this->loopEndSec);

	this->position.setLoopPoints(
		juce::AudioPlayHead::LoopPoints{ startQuarter, endQuarter });
}

void MovablePlayHead::setPositionInSeconds(double time) {
	juce::ScopedWriteLock locker(audioLock::getPositionLock());
	this->position.setTimeInSeconds(time);

	this->updatePositionByTimeInSecond();
}

void MovablePlayHead::setPositionInQuarter(double time) {
	juce::ScopedWriteLock locker(audioLock::getPositionLock());
	this->setPositionInSeconds(this->toSecondQ(time));
}

void MovablePlayHead::setPositionInSamples(int64_t sampleNum) {
	juce::ScopedWriteLock locker(audioLock::getPositionLock());
	this->position.setTimeInSamples(sampleNum);

	this->updatePositionByTimeInSample();
}

void MovablePlayHead::next(int blockSize) {
	if (this->position.getIsPlaying()) {
		uint64_t time = this->position.getTimeInSamples().orFallback(0);
		time += blockSize;
		this->setPositionInSamples(time);
	}
}

double MovablePlayHead::toSecond(double timeTick) const {
	return this->toSecond(timeTick, this->timeFormat);
}

double MovablePlayHead::toTick(double timeSecond) const {
	return this->toTick(timeSecond, this->timeFormat);
}

double MovablePlayHead::toSecond(double timeTick, short timeFormat) const {
	int tempIndex = this->tempoTemp.selectByTick(timeTick, timeFormat);
	return this->tempoTemp.tickToSec(timeTick, tempIndex, timeFormat);
}

double MovablePlayHead::toTick(double timeSecond, short timeFormat) const {
	int tempIndex = this->tempoTemp.selectBySec(timeSecond);
	return this->tempoTemp.secToTick(timeSecond, tempIndex, timeFormat);
}

double MovablePlayHead::toQuarter(double timeSecond) const {
	int tempIndex = this->tempoTemp.selectBySec(timeSecond);
	return this->tempoTemp.secToQuarter(timeSecond, tempIndex);
}

double MovablePlayHead::toSecondQ(double timeQuarter) const {
	int tempIndex = this->tempoTemp.selectByQuarter(timeQuarter);
	return this->tempoTemp.quarterToSec(timeQuarter, tempIndex);
}

std::tuple<int, double> MovablePlayHead::toBar(double timeSecond) const {
	int tempIndex = this->tempoTemp.selectBySec(timeSecond);
	double timeQuarter = this->tempoTemp.secToQuarter(timeSecond, tempIndex);
	double timeBar = this->tempoTemp.quarterToBar(timeQuarter, tempIndex);
	double quarterPerBar = this->tempoTemp.getQuarterPerBar(tempIndex);

	int barCount = std::floor(timeBar);
	double quarterInBar = (timeBar - barCount) * quarterPerBar;

	return { barCount, timeQuarter - quarterInBar };
}

std::tuple<int, double> MovablePlayHead::toBarQ(double timeQuarter) const {
	int tempIndex = this->tempoTemp.selectByQuarter(timeQuarter);
	double timeBar = this->tempoTemp.quarterToBar(timeQuarter, tempIndex);
	double quarterPerBar = this->tempoTemp.getQuarterPerBar(tempIndex);

	int barCount = std::floor(timeBar);
	double quarterInBar = (timeBar - barCount) * quarterPerBar;

	return { barCount, timeQuarter - quarterInBar };
}

juce::MidiMessageSequence& MovablePlayHead::getTempoSequence() {
	return this->tempos;
}

void MovablePlayHead::updateTempoTemp() {
	juce::ScopedWriteLock locker(audioLock::getPositionLock());
	this->tempoTemp.update(this->tempos);

	UICallbackAPI<void>::invoke(UICallbackType::TempoChanged);
}

int MovablePlayHead::getTempoTempIndexBySec(double timeSec) const {
	return this->tempoTemp.selectBySec(timeSec);
}

int MovablePlayHead::getTempoTempIndexByQuarter(double timeQuarter) const {
	return this->tempoTemp.selectByQuarter(timeQuarter);
}

const MovablePlayHead::TempoDataMini MovablePlayHead::getTempoTempData(int tempIndex) const {
	return this->tempoTemp.getTempoDataMini(tempIndex);
}

const juce::Array<MovablePlayHead::TempoLabelData> MovablePlayHead::getTempoDataList() const {
	/** Result */
	juce::Array<MovablePlayHead::TempoLabelData> result;

	/** Temp */
	double tempoTemp = 120.0;
	int numeratorTemp = 4, denominatorTemp = 4;

	/** Get Each Label */
	for (auto i : this->tempos) {
		auto& mes = i->message;
		if (mes.isTempoMetaEvent()) {
			tempoTemp = 1.0 / mes.getTempoSecondsPerQuarterNote() * 60.0;
			result.add({ mes.getTimeStamp(), tempoTemp, numeratorTemp, denominatorTemp, true });
		}
		else if (mes.isTimeSignatureMetaEvent()) {
			mes.getTimeSignatureInfo(numeratorTemp, denominatorTemp);
			result.add({ mes.getTimeStamp(), tempoTemp, numeratorTemp, denominatorTemp, false });
		}
	}

	/** Return */
	return result;
}

bool MovablePlayHead::getLooping() const {
	return this->position.getIsLooping();
}

std::tuple<double, double> MovablePlayHead::getLoopingTimeSec() const {
	return { this->loopStartSec, this->loopEndSec };
}

double MovablePlayHead::getSampleRate() const {
	return this->sampleRate;
}

void MovablePlayHead::setOverflow() {
	this->overflowFlag = true;
}

bool MovablePlayHead::checkOverflow() const {
	return this->overflowFlag;
}

int MovablePlayHead::addTempoLabelTempo(double time, double tempo) {
	/** Get Index */
	int index = this->getTempoInsertIndex(time);

	/** Insert Label */
	this->tempos.addEvent(juce::MidiMessage::tempoMetaEvent(
		1 / (tempo / 60)).withTimeStamp(time));
	this->updateTempoTemp();

	/** Return Index */
	return index;
}

int MovablePlayHead::addTempoLabelBeat(double time, int numerator, int denominator) {
	/** Get Index */
	int index = this->getTempoInsertIndex(time);

	/** Insert Label */
	this->tempos.addEvent(juce::MidiMessage::timeSignatureMetaEvent(
		numerator, denominator).withTimeStamp(time));
	this->updateTempoTemp();

	/** Return Index */
	return index;
}

void MovablePlayHead::removeTempoLabel(int index) {
	this->tempos.deleteEvent(index, false);
}

int MovablePlayHead::getTempoLabelNum() const {
	return this->tempos.getNumEvents();
}

bool MovablePlayHead::isTempoLabelTempoEvent(int index) const {
	if (auto ptrEvent = this->tempos.getEventPointer(index)) {
		return ptrEvent->message.isTempoMetaEvent();
	}
	return false;
}

void MovablePlayHead::setTempoLabelTime(int index, double time) {
	if (auto ptrEvent = this->tempos.getEventPointer(index)) {
		/** Set Message Time */
		ptrEvent->message.setTimeStamp(time);
		this->tempos.sort();

		/** Update Temp */
		this->updateTempoTemp();
	}
}

void MovablePlayHead::setTempoLabelTempo(int index, double tempo) {
	if (auto ptrEvent = this->tempos.getEventPointer(index)) {
		if (ptrEvent->message.isTempoMetaEvent()) {
			/** Set Message Tempo */
			double time = ptrEvent->message.getTimeStamp();
			ptrEvent->message = juce::MidiMessage::tempoMetaEvent(
				1 / (tempo / 60)).withTimeStamp(time);

			/** Update Temp */
			this->updateTempoTemp();
		}
	}
}

void MovablePlayHead::setTempoLabelBeat(
	int index, int numerator, int denominator) {
	if (auto ptrEvent = this->tempos.getEventPointer(index)) {
		if (ptrEvent->message.isTempoMetaEvent()) {
			/** Set Message Beat */
			double time = ptrEvent->message.getTimeStamp();
			ptrEvent->message = juce::MidiMessage::timeSignatureMetaEvent(
				numerator, denominator).withTimeStamp(time);

			/** Update Temp */
			this->updateTempoTemp();
		}
	}
}

double MovablePlayHead::getTempoLabelTime(int index) const {
	if (auto ptrEvent = this->tempos.getEventPointer(index)) {
		return ptrEvent->message.getTimeStamp();
	}
	return 0;
}

double MovablePlayHead::getTempoLabelTempo(int index) const {
	if (auto ptrEvent = this->tempos.getEventPointer(index)) {
		return 1 / ptrEvent->message.getTempoSecondsPerQuarterNote() * 60;
	}
	return 120;
}

std::tuple<int, int> MovablePlayHead::getTempoLabelBeat(int index) const {
	if (auto ptrEvent = this->tempos.getEventPointer(index)) {
		int numeratorTemp = 4, denominatorTemp = 4;
		ptrEvent->message.getTimeSignatureInfo(numeratorTemp, denominatorTemp);
		return { numeratorTemp, denominatorTemp };
	}
	return { 4, 4 };
}

void MovablePlayHead::updatePositionByTimeInSecond() {
	double time = this->position.getTimeInSeconds().orFallback(0);
	this->position.setTimeInSamples((int64_t)std::floor(time * this->sampleRate));
	double timeQuarter = this->toQuarter(time);
	this->position.setPpqPosition(timeQuarter);

	auto [barCount, barPpq] = this->toBarQ(timeQuarter);
	this->position.setBarCount(barCount);
	this->position.setPpqPositionOfLastBarStart(barPpq);
}

void MovablePlayHead::updatePositionByTimeInSample() {
	uint64_t sample = this->position.getTimeInSamples().orFallback(0);
	double time = sample / this->sampleRate;
	this->position.setTimeInSeconds(time);
	double timeQuarter = this->toQuarter(time);
	this->position.setPpqPosition(timeQuarter);

	auto [barCount, barPpq] = this->toBarQ(timeQuarter);
	this->position.setBarCount(barCount);
	this->position.setPpqPositionOfLastBarStart(barPpq);
}

int MovablePlayHead::getTempoInsertIndex(double time) const {
	for (int i = this->tempos.getNumEvents() - 1; i >= 0; i--) {
		auto ptrEvent = this->tempos.getEventPointer(i);
		if (ptrEvent->message.getTimeStamp() >= time) {
			return i + 1;
		}
	}
	return 0;
}

PlayPosition* PlayPosition::getInstance() {
	return PlayPosition::instance ? PlayPosition::instance : (PlayPosition::instance = new PlayPosition());
}

void PlayPosition::releaseInstance() {
	if (PlayPosition::instance) {
		delete PlayPosition::instance;
		PlayPosition::instance = nullptr;
	}
}

PlayPosition* PlayPosition::instance = nullptr;

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

void MovablePlayHead::setTempoSequence(const juce::MidiMessageSequence& seq) {
	this->tempos.clear();
	this->insertTempoSequence(seq);
}

void MovablePlayHead::insertTempoSequence(const juce::MidiMessageSequence& seq) {
	for (auto i : seq) {
		this->insert(i->message);
	}
	this->updateTempoTemp();
	this->updateLabelTypeTemp();
}

const juce::MidiMessageSequence MovablePlayHead::getTempoSequence() const {
	juce::MidiMessageSequence result;
	for (auto& event : this->tempos) {
		result.addEvent(event, 0);
	}
	return result;
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
	for (auto &i : this->tempos) {
		if (i.isTempoMetaEvent()) {
			tempoTemp = 1.0 / i.getTempoSecondsPerQuarterNote() * 60.0;
			result.add({ i.getTimeStamp(), tempoTemp, numeratorTemp, denominatorTemp, true });
		}
		else if (i.isTimeSignatureMetaEvent()) {
			i.getTimeSignatureInfo(numeratorTemp, denominatorTemp);
			result.add({ i.getTimeStamp(), tempoTemp, numeratorTemp, denominatorTemp, false });
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

int MovablePlayHead::addTempoLabelTempo(
	double time, double tempo, int newIndex) {
	/** Insert Label */
	int index = this->insert(juce::MidiMessage::tempoMetaEvent(
		1000000.0 / (tempo / 60.0)).withTimeStamp(time), newIndex);
	this->updateTempoTemp();
	this->updateLabelTypeTemp();

	/** Return Index */
	return index;
}

int MovablePlayHead::addTempoLabelBeat(
	double time, int numerator, int denominator, int newIndex) {
	/** Insert Label */
	int index = this->insert(juce::MidiMessage::timeSignatureMetaEvent(
		numerator, denominator).withTimeStamp(time), newIndex);
	this->updateTempoTemp();
	this->updateLabelTypeTemp();

	/** Return Index */
	return index;
}

void MovablePlayHead::removeTempoLabel(int index) {
	this->remove(index);

	/** Update Temp */
	this->updateTempoTemp();
	this->updateLabelTypeTemp();
}

int MovablePlayHead::getTempoLabelNum() const {
	return this->tempos.size();
}

bool MovablePlayHead::isTempoLabelTempoEvent(int index) const {
	if (index >= 0 && index < this->tempos.size()) {
		auto& event = this->tempos.getReference(index);
		return event.isTempoMetaEvent();
	}
	return false;
}

int MovablePlayHead::setTempoLabelTime(int index, double time, int newIndex) {
	if (index >= 0 && index < this->tempos.size()) {
		/** Remove */
		auto event = this->remove(index);

		/** Set Message Time */
		time = std::max(time, 0.0);
		event.setTimeStamp(time);
		
		/** Add */
		int indexTemp = this->insert(event, newIndex);

		/** Update Temp */
		this->updateTempoTemp();
		this->updateLabelTypeTemp();

		/** Return */
		return indexTemp;
	}
	return index;
}

void MovablePlayHead::setTempoLabelTempo(int index, double tempo) {
	if (index >= 0 && index < this->tempos.size()) {
		auto& event = this->tempos.getReference(index);
		if (event.isTempoMetaEvent()) {
			/** Set Message Tempo */
			double time = event.getTimeStamp();
			event = juce::MidiMessage::tempoMetaEvent(
				1000000.0 / (tempo / 60.0)).withTimeStamp(time);

			/** Update Temp */
			this->updateTempoTemp();
			this->updateLabelTypeTemp();
		}
	}
}

void MovablePlayHead::setTempoLabelBeat(
	int index, int numerator, int denominator) {
	if (index >= 0 && index < this->tempos.size()) {
		auto& event = this->tempos.getReference(index);
		if (event.isTimeSignatureMetaEvent()) {
			/** Set Message Beat */
			double time = event.getTimeStamp();
			event = juce::MidiMessage::timeSignatureMetaEvent(
				numerator, denominator).withTimeStamp(time);

			/** Update Temp */
			this->updateTempoTemp();
			this->updateLabelTypeTemp();
		}
	}
}

double MovablePlayHead::getTempoLabelTime(int index) const {
	if (index >= 0 && index < this->tempos.size()) {
		auto& event = this->tempos.getReference(index);
		return event.getTimeStamp();
	}
	return 0;
}

double MovablePlayHead::getTempoLabelTempo(int index) const {
	if (index >= 0 && index < this->tempos.size()) {
		auto& event = this->tempos.getReference(index);

		return 1 / event.getTempoSecondsPerQuarterNote() * 60;
	}
	return 120;
}

std::tuple<int, int> MovablePlayHead::getTempoLabelBeat(int index) const {
	if (index >= 0 && index < this->tempos.size()) {
		auto& event = this->tempos.getReference(index);

		int numeratorTemp = 4, denominatorTemp = 4;
		event.getTimeSignatureInfo(numeratorTemp, denominatorTemp);
		return { numeratorTemp, denominatorTemp };
	}
	return { 4, 4 };
}

int MovablePlayHead::insert(const juce::MidiMessage& mes, int newIndex) {
	int index = newIndex;
	if (index < 0 || index > this->tempos.size()) {
		index = this->getTempoInsertIndex(mes.getTimeStamp());
	}
	else {
		do {
			double time = mes.getTimeStamp();
			if (index > 0) {
				auto& last = this->tempos.getReference(index - 1);
				if (!(last.getTimeStamp() <= time)) {
					index = this->getTempoInsertIndex(time);
					break;
				}
			}
			if (index < this->tempos.size() - 1) {
				auto& next = this->tempos.getReference(index + 1);
				double time = mes.getTimeStamp();
				if (!(next.getTimeStamp() >= time)) {
					index = this->getTempoInsertIndex(time);
					break;
				}
			}
		} while (false);
	}
	
	this->tempos.insert(index, mes);
	return index;
}

const juce::MidiMessage MovablePlayHead::remove(int index) {
	if (index >= 0 && index < this->tempos.size()) {
		auto event = this->tempos.getUnchecked(index);
		this->tempos.remove(index);
		return event;
	}
	return {};
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
	/** No Events */
	if (this->tempos.size() == 0) {
		return 0;
	}

	/** Last */
	{
		auto& last = this->tempos.getReference(this->tempos.size() - 1);
		if (last.getTimeStamp() <= time) {
			return this->tempos.size();
		}
	}

	/** First */
	{
		auto& first = this->tempos.getReference(0);
		if (first.getTimeStamp() > time) {
			return 0;
		}
	}

	/** Insert */
	for (int i = 0; i < this->tempos.size() - 1; i++) {
		auto& last = this->tempos.getReference(i);
		auto& next = this->tempos.getReference(i + 1);
		if (last.getTimeStamp() <= time &&
			next.getTimeStamp() > time) {
			return i + 1;
		}
	}

	/** Error */
	return -1;
}

void MovablePlayHead::updateLabelTypeTemp() {
	juce::ScopedWriteLock locker(audioLock::getPositionLock());

	this->tempoTypeIndexTemp.clear();
	this->beatTypeIndexTemp.clear();

	for (int i = 0; i < this->tempos.size(); i++) {
		auto& message = this->tempos.getReference(i);
		if (message.isTempoMetaEvent()) {
			this->tempoTypeIndexTemp.add(i);
			continue;
		}
		if (message.isTimeSignatureMetaEvent()) {
			this->beatTypeIndexTemp.add(i);
			continue;
		}
	}
}

int MovablePlayHead::getTempoTypeLabelNum() const {
	return this->tempoTypeIndexTemp.size();
}

int MovablePlayHead::getBeatTypeLabelNum() const {
	return this->beatTypeIndexTemp.size();
}

int MovablePlayHead::getTempoTypeLabelIndex(int typeIndex) const {
	if (typeIndex >= 0 && typeIndex < this->tempoTypeIndexTemp.size()) {
		return this->tempoTypeIndexTemp.getUnchecked(typeIndex);
	}
	return -1;
}

int MovablePlayHead::getBeatTypeLabelIndex(int typeIndex) const {
	if (typeIndex >= 0 && typeIndex < this->beatTypeIndexTemp.size()) {
		return this->beatTypeIndexTemp.getUnchecked(typeIndex);
	}
	return -1;
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

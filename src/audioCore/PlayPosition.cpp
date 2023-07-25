#include "PlayPosition.h"

#include <chrono>
#include "Utils.h"

juce::Optional<juce::AudioPlayHead::PositionInfo> MovablePlayHead::getPosition() const {
	juce::ScopedReadLock locker(this->lock);

	/** Get System Clock Nanoseconds */
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::nanoseconds ns
		= std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
	this->position.setHostTimeNs((uint64_t)ns.count());

	return juce::makeOptional(this->position);
}

const juce::ReadWriteLock& MovablePlayHead::getLock() const {
	return this->lock;
}

bool MovablePlayHead::canControlTransport() {
	//return juce::MessageManager::existsAndIsCurrentThread();
	return true;
}

void MovablePlayHead::transportPlay(bool shouldStartPlaying) {
	juce::ScopedWriteLock locker(this->lock);
	this->position.setIsPlaying(shouldStartPlaying);
	if (!shouldStartPlaying) {
		this->position.setIsRecording(false);
	}
}

void MovablePlayHead::transportRecord(bool shouldStartRecording) {
	juce::ScopedWriteLock locker(this->lock);
	this->position.setIsRecording(shouldStartRecording);
	if (shouldStartRecording) {
		this->position.setIsPlaying(true);
	}
}

void MovablePlayHead::transportRewind() {
	juce::ScopedWriteLock locker(this->lock);
	this->position.setTimeInSamples(0);
	this->position.setTimeInSeconds(0);
	this->position.setBarCount(0);
	this->position.setPpqPositionOfLastBarStart(0);
	this->position.setPpqPosition(0);
}

void MovablePlayHead::setTimeFormat(short ticksPerQuarter) {
	juce::ScopedWriteLock locker(this->lock);
	this->timeFormat = ticksPerQuarter;
}

void MovablePlayHead::setSampleRate(double sampleRate) {
	juce::ScopedWriteLock locker(this->lock);
	this->sampleRate = sampleRate;

	this->position.setTimeInSamples(
		(int64_t)std::floor(this->position.getTimeInSeconds().orFallback(0) * this->sampleRate));
}

void MovablePlayHead::setLooping(bool looping) {
	juce::ScopedWriteLock locker(this->lock);
	this->position.setIsLooping(looping);
}

void MovablePlayHead::setLoopPointsInSeconds(const std::tuple<double, double>& points) {
	juce::ScopedWriteLock locker(this->lock);

	auto [start, end] = points;
	start = this->toTick(start) / this->timeFormat;
	end = this->toTick(end) / this->timeFormat;

	this->position.setLoopPoints(
		juce::AudioPlayHead::LoopPoints{ start, end });
}

void MovablePlayHead::setLoopPointsInQuarter(const std::tuple<double, double>& points) {
	juce::ScopedWriteLock locker(this->lock);
	this->position.setLoopPoints(
		juce::AudioPlayHead::LoopPoints{ std::get<0>(points), std::get<1>(points) });
}

void MovablePlayHead::setPositionInSeconds(double time) {
	juce::ScopedWriteLock locker(this->lock);
	this->position.setTimeInSeconds(time);

	this->updatePositionByTimeInSecond();
}

void MovablePlayHead::setPositionInQuarter(double time) {
	juce::ScopedWriteLock locker(this->lock);
	this->setPositionInSeconds(this->toSecond(time * this->timeFormat));
}

void MovablePlayHead::setPositionInSamples(int64_t sampleNum) {
	juce::ScopedWriteLock locker(this->lock);
	this->position.setTimeInSamples(sampleNum);

	this->updatePositionByTimeInSample();
}

void MovablePlayHead::next(int blockSize) {
	juce::ScopedWriteLock locker(this->lock);
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

std::tuple<int, double> MovablePlayHead::toBar(double timeSecond) const {
	return this->toBar(timeSecond, this->timeFormat);
}

double MovablePlayHead::toSecond(double timeTick, short timeFormat) const {
	return utils::convertTicksToSecondsWithObjectiveTempoTime(
		timeTick, this->tempos, this->timeFormat);
}

double MovablePlayHead::toTick(double timeSecond, short timeFormat) const {
	return utils::convertSecondsToTicks(
		timeSecond, this->tempos, timeFormat);
}

std::tuple<int, double> MovablePlayHead::toBar(double timeSecond, short /*timeFormat*/) const {
	return utils::getBarBySecond(timeSecond, this->tempos);
}

juce::MidiMessageSequence& MovablePlayHead::getTempoSequence() {
	return this->tempos;
}

void MovablePlayHead::updatePositionByTimeInSecond() {
	double time = this->position.getTimeInSeconds().orFallback(0);
	this->position.setTimeInSamples((int64_t)std::floor(time * this->sampleRate));
	this->position.setPpqPosition(this->toTick(time) / this->timeFormat);

	auto barInfo = this->toBar(time);
	this->position.setBarCount(std::get<0>(barInfo));
	this->position.setPpqPositionOfLastBarStart(std::get<1>(barInfo));
}

void MovablePlayHead::updatePositionByTimeInSample() {
	uint64_t sample = this->position.getTimeInSamples().orFallback(0);
	double time = sample / this->sampleRate;
	this->position.setTimeInSeconds(time);
	this->position.setPpqPosition(this->toTick(time) / this->timeFormat);

	auto barInfo = this->toBar(time);
	this->position.setBarCount(std::get<0>(barInfo));
	this->position.setPpqPositionOfLastBarStart(std::get<1>(barInfo));
}

PlayPosition* PlayPosition::getInstance() {
	juce::GenericScopedLock locker{ PlayPosition::instanceGuard };
	return PlayPosition::instance ? PlayPosition::instance : (PlayPosition::instance = new PlayPosition());
}

void PlayPosition::releaseInstance() {
	juce::GenericScopedLock locker{ PlayPosition::instanceGuard };
	if (PlayPosition::instance) {
		delete PlayPosition::instance;
		PlayPosition::instance = nullptr;
	}
}

PlayPosition* PlayPosition::instance = nullptr;
juce::CriticalSection PlayPosition::instanceGuard;

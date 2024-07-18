#pragma once

#include <JuceHeader.h>
#include "TempoTemp.h"

class MovablePlayHead : public juce::AudioPlayHead {
public:
	MovablePlayHead() = default;
	~MovablePlayHead() override = default;

	juce::Optional<juce::AudioPlayHead::PositionInfo> getPosition() const override;

	bool canControlTransport() override;
	void transportPlay(bool shouldStartPlaying) override;
	void transportRecord(bool shouldStartRecording) override;
	void transportRewind() override;

	void setTimeFormat(short ticksPerQuarter);
	void setSampleRate(double sampleRate);
	void setLooping(bool looping);
	void setLoopPointsInSeconds(const std::tuple<double, double>& points);
	void setPositionInSeconds(double time);
	void setPositionInQuarter(double time);
	void setPositionInSamples(int64_t sampleNum);
	void next(int blockSize);

	double toSecond(double timeTick) const;
	double toTick(double timeSecond) const;
	double toSecond(double timeTick, short timeFormat) const;
	double toTick(double timeSecond, short timeFormat) const;
	double toQuarter(double timeSecond) const;
	double toSecondQ(double timeQuarter) const;
	/** BarCount, BarStartQuarter */
	std::tuple<int, double> toBar(double timeSecond) const;
	/** BarCount, BarStartQuarter */
	std::tuple<int, double> toBarQ(double timeQuarter) const;

	void setTempoSequence(const juce::MidiMessageSequence& seq);
	void insertTempoSequence(const juce::MidiMessageSequence& seq);
	const juce::MidiMessageSequence getTempoSequence() const;
	void updateTempoTemp();
	int getTempoTempIndexBySec(double timeSec) const;
	int getTempoTempIndexByQuarter(double timeQuarter) const;
	using TempoDataMini = TempoTemp::TempoDataMini;
	const TempoDataMini getTempoTempData(int tempIndex) const;
	/** timeInSec, tempo, numerator, denominator, isTempo */
	using TempoLabelData = std::tuple<double, double, int, int, bool>;
	const juce::Array<TempoLabelData> getTempoDataList() const;
	bool getLooping() const;
	std::tuple<double, double> getLoopingTimeSec() const;
	double getSampleRate() const;
	void setOverflow();
	bool checkOverflow() const;

	int addTempoLabelTempo(double time, double tempo, int newIndex = -1);
	int addTempoLabelBeat(double time, int numerator, int denominator, int newIndex = -1);
	void removeTempoLabel(int index);
	int getTempoLabelNum() const;
	bool isTempoLabelTempoEvent(int index) const;
	int setTempoLabelTime(int index, double time, int newIndex = -1);
	void setTempoLabelTempo(int index, double tempo);
	void setTempoLabelBeat(int index, int numerator, int denominator);
	double getTempoLabelTime(int index) const;
	double getTempoLabelTempo(int index) const;
	std::tuple<int, int> getTempoLabelBeat(int index) const;

	int getTempoInsertIndex(double time) const;

protected:
	mutable juce::AudioPlayHead::PositionInfo position;
	juce::Array<juce::MidiMessage> tempos;
	TempoTemp tempoTemp;
	std::atomic_short timeFormat = 480;
	std::atomic<double> sampleRate = 48000;
	std::atomic_bool overflowFlag = false;
	std::atomic<double> loopStartSec = 0, loopEndSec = 0;

	int insert(const juce::MidiMessage& mes, int newIndex = -1);
	const juce::MidiMessage remove(int index);

	void updatePositionByTimeInSecond();
	void updatePositionByTimeInSample();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MovablePlayHead)
};

class PlayPosition final : public MovablePlayHead,
	private juce::DeletedAtShutdown {
public:
	PlayPosition() = default;
	~PlayPosition() override = default;

public:
	static PlayPosition* getInstance();
	static void releaseInstance();

private:
	static PlayPosition* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayPosition)
};

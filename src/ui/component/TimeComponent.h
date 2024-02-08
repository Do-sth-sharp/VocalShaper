#pragma once

#include <JuceHeader.h>
#include "../misc/LevelMeterHub.h"

class TimeComponent final
	: public juce::Component,
	public LevelMeterHub::Target,
	public juce::SettableTooltipClient {
public:
	TimeComponent();

	void updateLevelMeter() override;
	void paint(juce::Graphics& g) override;

	void mouseUp(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;

	void switchTime();

private:
	double timeInSec = 0;
	uint64_t timeInMeasure = 0;
	double timeInBeat = 0;
	bool showSec = true;

	using LevelValue = std::tuple<float, float>;
	LevelValue level;

	bool isPlaying = false;
	bool isRecording = false;

	static uint8_t convertBits(uint8_t num);
	static void paintNum(
		juce::Graphics& g, const juce::Rectangle<int>& area,
		float lineThickness, float splitThickness, uint8_t num);
	static void paintColon(
		juce::Graphics& g, const juce::Rectangle<int>& area,
		float lineThickness, float splitThickness);
	static void paintDot(
		juce::Graphics& g, const juce::Rectangle<int>& area,
		float lineThickness, float splitThickness);
	static void paintLevelMeter(
		juce::Graphics& g, const juce::Rectangle<int>& area,
		const LevelValue& value, float splitThickness, bool logMeter);
	static void paintRecordStatus(
		juce::Graphics& g, const juce::Rectangle<int>& area,
		float lineThickness, bool recording);
	static void paintPlayStatus(
		juce::Graphics& g, const juce::Rectangle<int>& area,
		float lineThickness, bool playing);

	juce::PopupMenu createTimeMenu() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeComponent)
};

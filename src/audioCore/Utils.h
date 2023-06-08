#pragma once

#include <JuceHeader.h>

namespace utils {
	/**
	 * @brief	Get the channel index and size of the audio bus.
	 */
	std::tuple<int, int> getChannelIndexAndNumOfBus(
		const juce::AudioProcessor* processor, int busIndex, bool isInput);

	double convertSecondsToTicks(double time,
		const juce::MidiMessageSequence& tempoEvents,
		int timeFormat);

	double convertTicksToSeconds(double time,
		const juce::MidiMessageSequence& tempoEvents,
		int timeFormat);

	double convertSecondsToTicksWithObjectiveTempoTime(double time,
		const juce::MidiMessageSequence& tempoEvents,
		int timeFormat);

	double convertTicksToSecondsWithObjectiveTempoTime(double time,
		const juce::MidiMessageSequence& tempoEvents,
		int timeFormat);
}

#define UNUSED(var) (void)var

#pragma once

#include <JuceHeader.h>
#include "Mixer.h"
#include "Sequencer.h"

class MainGraph final : public juce::AudioProcessorGraph {
public:
	MainGraph();

	/**
	 * @brief	Set the input and output channel number of current audio device.
	 */
	void setAudioLayout(int inputChannelNum, int outputChannelNum);

	/**
	 * @brief	Set the MIDI Message Hook.
	 */
	void setMIDIMessageHook(const std::function<void(const juce::MidiMessage&)> hook);

	/**
	 * @brief	Get the Mixer Processor.
	 */
	Mixer* getMixer() const;
	/**
	 * @brief	Get the Sequencer Processor.
	 */
	Sequencer* getSequencer() const;

	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;

private:
	juce::AudioProcessorGraph::Node::Ptr sequencer;
	juce::AudioProcessorGraph::Node::Ptr mixer;
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode;

	std::function<void(const juce::MidiMessage&)> midiHook;
	juce::ReadWriteLock hookLock;

	void processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainGraph)
};

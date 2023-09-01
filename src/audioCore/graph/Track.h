﻿#pragma once

#include <JuceHeader.h>
#include "PluginDock.h"

class Track final : public juce::AudioProcessorGraph {
public:
	Track() = delete;
	Track(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());

	/**
	 * @brief	Add an audio input bus onto the track.
	 */
	bool addAdditionalAudioBus();
	/**
	 * @brief	Remove the last audio input bus from the track.
	 */
	bool removeAdditionalAudioBus();

	void setMute(bool mute);
	bool getMute() const;
	void setGain(float gain);
	float getGain() const;
	void setPan(float pan);
	float getPan() const;
	void setSlider(float slider);
	float getSlider() const;

	const juce::AudioChannelSet& getAudioChannelSet() const;

	PluginDock* getPluginDock() const;

	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	void setPlayHead(juce::AudioPlayHead* newPlayHead) override;

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode, midiOutputNode;
	juce::AudioChannelSet audioChannels;

	juce::AudioProcessorGraph::Node::Ptr pluginDockNode;

	juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::Panner<float>> gainAndPanner;
	juce::dsp::ProcessorChain<juce::dsp::Gain<float>> slider;
	std::atomic<bool> isMute = false;

	float panValue = 0.0;

private:
	bool canAddBus(bool isInput) const override;
	bool canRemoveBus(bool isInput) const override;

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
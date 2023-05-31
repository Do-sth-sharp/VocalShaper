﻿#pragma once

#include <JuceHeader.h>

#include "SourceList.h"

class SeqSourceProcessor final : public juce::AudioProcessor {
public:
	SeqSourceProcessor() = delete;
	SeqSourceProcessor(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());

public:
	const juce::String getName() const override { return "SeqSource"; };

	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	void releaseResources() override {};
	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

	bool acceptsMidi() const override { return true; };
	bool producesMidi() const override { return true; };

	juce::AudioProcessorEditor* createEditor() override { return nullptr; };
	bool hasEditor() const override { return false; };

	int getNumPrograms() override { return 0; };
	int getCurrentProgram() override { return 0; };
	void setCurrentProgram(int index) override {};
	const juce::String getProgramName(int index) override { return juce::String{}; };
	void changeProgramName(int index, const juce::String& newName) override {};

	void getStateInformation(juce::MemoryBlock& destData) override {};
	void setStateInformation(const void* data, int sizeInBytes) override {};

	double getTailLengthSeconds() const override;

private:
	juce::AudioChannelSet audioChannels;
	SourceList srcs;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqSourceProcessor)
};

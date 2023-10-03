#pragma once

#include <JuceHeader.h>
#include "PluginDock.h"
#include "../project/Serializable.h"

class Track final : public juce::AudioProcessorGraph,
	public Serializable {
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
	int getAdditionalAudioBusNum() const;

	void setMute(bool mute);
	bool getMute() const;
	void setGain(float gain);
	float getGain() const;
	void setPan(float pan);
	float getPan() const;
	void setSlider(float slider);
	float getSlider() const;

	void setTrackName(const juce::String& name);
	const juce::String getTrackName() const;
	void setTrackColor(const juce::Colour& color);
	const juce::Colour getTrackColor() const;

	const juce::AudioChannelSet& getAudioChannelSet() const;

	PluginDock* getPluginDock() const;

	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	void setPlayHead(juce::AudioPlayHead* newPlayHead) override;

public:
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<google::protobuf::Message> serialize() const override;

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode, midiOutputNode;
	juce::AudioChannelSet audioChannels;

	juce::AudioProcessorGraph::Node::Ptr pluginDockNode;

	juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::Panner<float>> gainAndPanner;
	juce::dsp::ProcessorChain<juce::dsp::Gain<float>> slider;
	std::atomic<bool> isMute = false;

	float panValue = 0.0;

	juce::String trackName;
	juce::Colour trackColor;

private:
	bool canAddBus(bool isInput) const override;
	bool canRemoveBus(bool isInput) const override;

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};

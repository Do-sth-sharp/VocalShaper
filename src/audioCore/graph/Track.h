#pragma once

#include <JuceHeader.h>
#include "PluginDock.h"
#include "../project/Serializable.h"

class Track final : public juce::AudioProcessorGraph,
	public Serializable {
public:
	Track() = delete;
	Track(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());

	void updateIndex(int index);

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

	void clearGraph();

	const juce::Array<float> getOutputLevels() const;

	class SafePointer {
	private:
		juce::WeakReference<Track> weakRef;

	public:
		SafePointer() = default;
		SafePointer(Track* source) : weakRef(source) {};
		SafePointer(const SafePointer& other) noexcept : weakRef(other.weakRef) {};

		SafePointer& operator= (const SafePointer& other) { weakRef = other.weakRef; return *this; };
		SafePointer& operator= (Track* newSource) { weakRef = newSource; return *this; };

		Track* getTrack() const noexcept { return dynamic_cast<Track*> (weakRef.get()); };
		operator Track* () const noexcept { return getTrack(); };
		Track* operator->() const noexcept { return getTrack(); };
		void deleteAndZero() { delete getTrack(); };

		bool operator== (Track* component) const noexcept { return weakRef == component; };
		bool operator!= (Track* component) const noexcept { return weakRef != component; };
	};

public:
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<google::protobuf::Message> serialize() const override;

private:
	int index = -1;

	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode, midiOutputNode;
	juce::AudioChannelSet audioChannels;

	juce::AudioProcessorGraph::Node::Ptr pluginDockNode;

	juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::Panner<float>> gainAndPanner;
	juce::dsp::ProcessorChain<juce::dsp::Gain<float>> slider;
	std::atomic<bool> isMute = false;

	std::atomic<float> panValue = 0.0;

	juce::String trackName;
	juce::Colour trackColor;

	juce::Array<float> outputLevels;

private:
	bool canAddBus(bool isInput) const override;
	bool canRemoveBus(bool isInput) const override;

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

	JUCE_DECLARE_WEAK_REFERENCEABLE(Track)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};

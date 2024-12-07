#pragma once

#include <JuceHeader.h>
#include "PluginDock.h"
#include "../project/Serializable.h"

class MixerTrack final : public juce::AudioProcessorGraph,
	public Serializable {
public:
	enum class TrackType {
		Track = 0, AuxTrack, MasterTrack
	};

	MixerTrack() = delete;
	MixerTrack(TrackType type,
		const juce::AudioChannelSet& bus = juce::AudioChannelSet::stereo());

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

	void clearGraph();

	class SafePointer {
	private:
		juce::WeakReference<MixerTrack> weakRef;

	public:
		SafePointer() = default;
		SafePointer(MixerTrack* source) : weakRef(source) {};
		SafePointer(const SafePointer& other) noexcept : weakRef(other.weakRef) {};

		SafePointer& operator= (const SafePointer& other) { weakRef = other.weakRef; return *this; };
		SafePointer& operator= (MixerTrack* newSource) { weakRef = newSource; return *this; };

		MixerTrack* getTrack() const noexcept { return dynamic_cast<MixerTrack*> (weakRef.get()); };
		operator MixerTrack* () const noexcept { return getTrack(); };
		MixerTrack* operator->() const noexcept { return getTrack(); };
		void deleteAndZero() { delete getTrack(); };

		bool operator== (MixerTrack* component) const noexcept { return weakRef == component; };
		bool operator!= (MixerTrack* component) const noexcept { return weakRef != component; };
	};

public:
	bool parse(
		const google::protobuf::Message* data,
		const ParseConfig& config) override;
	std::unique_ptr<google::protobuf::Message> serialize(
		const SerializeConfig& config) const override;

private:
	int index = -1;
	const TrackType type;

	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode, midiOutputNode;
	const juce::AudioChannelSet audioChannels;

	juce::AudioProcessorGraph::Node::Ptr pluginDockNode;

	juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::Panner<float>> gainAndPanner;
	juce::dsp::ProcessorChain<juce::dsp::Gain<float>> slider;

	std::atomic<float> panValue = 0.0;

private:
	bool canAddBus(bool isInput) const override;
	bool canRemoveBus(bool isInput) const override;

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

	JUCE_DECLARE_WEAK_REFERENCEABLE(MixerTrack)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerTrack)
};

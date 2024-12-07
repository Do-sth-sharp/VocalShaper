#pragma once

#include <JuceHeader.h>
#include "SeqSourceProcessor.h"
#include "MixerTrack.h"
#include "../project/Serializable.h"

class Track final : public juce::AudioProcessorGraph,
	public Serializable {
public:
	using TrackType = MixerTrack::TrackType;

	Track() = delete;
	Track(TrackType type,
		const juce::AudioChannelSet& bus = juce::AudioChannelSet::stereo());
	~Track();

	void updateIndex(int index);

	SeqSourceProcessor* getSequencer() const;
	MixerTrack* getMixer() const;

	const juce::AudioChannelSet& getAudioChannelSet() const;

	void setTrackName(const juce::String& name);
	const juce::String getTrackName() const;
	void setTrackColor(const juce::Colour& color);
	const juce::Colour getTrackColor() const;

	void setMute(bool mute);
	bool getMute() const;
	void setSolo(bool solo);
	bool getSolo() const;
	bool getEquivalentMute() const;

	/**
	 * @brief	Add an audio input bus onto the track.
	 */
	bool addAdditionalAudioBus();
	/**
	 * @brief	Remove the last audio input bus from the track.
	 */
	bool removeAdditionalAudioBus();
	int getAdditionalAudioBusNum() const;

	const juce::Array<float> getOutputLevels() const;

	void setPlayHead(juce::AudioPlayHead* newPlayHead) override;

	void clearGraph();

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

	juce::AudioProcessorGraph::Node::Ptr sequencerNode, mixerNode;

	juce::String trackName;
	juce::Colour trackColor;

	std::atomic_bool isMute = false;
	std::atomic_bool isSolo = false;

	juce::Array<float> outputLevels;

	bool canAddBus(bool isInput) const override;
	bool canRemoveBus(bool isInput) const override;

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

	JUCE_DECLARE_WEAK_REFERENCEABLE(Track)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};

#pragma once

#include <JuceHeader.h>

class Sequencer final : public juce::AudioProcessorGraph {
public:
	Sequencer();

	/**
	 * @brief	Insert a source track onto sequencer.
	 */
	void insertSource(std::unique_ptr<juce::AudioProcessor> processor, int index = -1);
	/**
	 * @brief	Remove a source track from sequencer.
	 */
	void removeSource(int index);
	/**
	 * @brief	Insert an instrument onto sequencer.
	 */
	void insertInstrument(std::unique_ptr<juce::AudioProcessor> processor, int index = -1);
	/**
	 * @brief	Remove an instrument from sequencer.
	 */
	void removeInstrument(int index);

	/**
	 * @brief	Get source track number of sequencer.
	 */
	int getSourceNum() const;
	/**
	 * @brief	Get processor pointer of the source track.
	 */
	juce::AudioProcessor* getSourceProcessor(int index) const;
	/**
	 * @brief	Get instrument number of sequencer.
	 */
	int getInstrumentNum() const;
	/**
	 * @brief	Get processor pointer of the instrument.
	 */
	juce::AudioProcessor* getInstrumentProcessor(int index) const;

	/**
	 * @brief	Enable MIDI input of the source track.
	 */
	void setMIDIInputConnection(int sourceIndex);
	/**
	 * @brief	Disable MIDI input of the source track.
	 */
	void removeMIDIInputConnection(int sourceIndex);
	/**
	 * @brief	Connect audio input channel of the source track with audio input node.
	 */
	void setAudioInputConnection(int sourceIndex, int srcChannel, int dstChannel);
	/**
	 * @brief	Disconnect audio input channel of the source track with audio input node.
	 */
	void removeAudioInputConnection(int sourceIndex, int srcChannel, int dstChannel);
	/**
	 * @brief	Enable MIDI connection between the source track and the instrument.
	 */
	void addMIDIInstrumentConnection(int sourceIndex, int instrIndex);
	/**
	 * @brief	Disable MIDI connection between the source track and the instrument.
	 */
	void removeMIDIInstrumentConnection(int sourceIndex, int instrIndex);
	/**
	 * @brief	Enable MIDI output of the source track.
	 */
	void setMIDISendConnection(int sourceIndex);
	/**
	 * @brief	Disable MIDI output of the source track.
	 */
	void removeMIDISendConnection(int sourceIndex);
	/**
	 * @brief	Connect audio output channel of the source track with audio output node.
	 */
	void setAudioSendConnection(int sourceIndex, int srcChannel, int dstChannel);
	/**
	 * @brief	Disconnect audio output channel of the source track with audio output node.
	 */
	void removeAudioSendConnection(int sourceIndex, int srcChannel, int dstChannel);
	/**
	 * @brief	Connect audio output channel of the instrument with audio output node.
	 */
	void setAudioOutputConnection(int instrIndex, int srcChannel, int dstChannel);
	/**
	 * @brief	Disconnect audio output channel of the instrument with audio output node.
	 */
	void removeAudioOutputConnection(int instrIndex, int srcChannel, int dstChannel);

	/**
	 * @brief	Check MIDI input of the source track is enabled.
	 */
	bool isMIDIInputConnected(int sourceIndex) const;
	/**
	 * @brief	Check audio input channel of the source track with audio input node connection.
	 */
	bool isAudioInputConnected(int sourceIndex, int srcChannel, int dstChannel) const;
	/**
	 * @brief	Check MIDI connection between the source track and the instrument is enabled.
	 */
	bool isMIDIInstrumentConnected(int sourceIndex, int instrIndex) const;
	/**
	 * @brief	Check MIDI output of the source track is enabled.
	 */
	bool isMIDISendConnected(int sourceIndex) const;
	/**
	 * @brief	Check audio output channel of the source track with audio output node connection.
	 */
	bool isAudioSendConnected(int sourceIndex, int srcChannel, int dstChannel) const;
	/**
	 * @brief	Check audio output channel of the instrument with audio output node connection.
	 */
	bool isAudioOutputConnected(int instrIndex, int srcChannel, int dstChannel) const;

	/**
	 * @brief	Set audio input channel layout.
	 */
	void setInputChannels(const juce::Array<juce::AudioChannelSet>& channels);

	/**
	 * @brief	Add an audio output bus.
	 */
	void addOutputBus(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	/**
	 * @brief	Remove the last audio output bus.
	 */
	void removeOutputBus();

private:
	juce::AudioProcessorGraph::Node::Ptr midiInputNode, midiOutputNode;
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;

	juce::Array<juce::AudioProcessorGraph::Node::Ptr> audioSourceNodeList;
	juce::Array<juce::AudioProcessorGraph::Node::Ptr> instrumentNodeList;

	juce::Array<juce::AudioProcessorGraph::Connection> midiInputConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioInputConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> midiInstrumentConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> midiSendConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioSendConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioOutputConnectionList;

	void setAudioLayout(const juce::AudioProcessorGraph::BusesLayout& busLayout);

	void removeIllegalAudioInputConnections();
	void removeIllegalAudioSendConnections();
	void removeIllegalAudioOutputConnections();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sequencer)
};

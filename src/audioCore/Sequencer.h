#pragma once

#include <Defs.h>

class Sequencer final : public juce::AudioProcessorGraph {
public:
	Sequencer();

	void insertSource(std::unique_ptr<juce::AudioProcessor> processor, int index = -1);
	void removeSource(int index);
	void insertInstrment(std::unique_ptr<juce::AudioProcessor> processor, int index = -1);
	void removeInstrment(int index);

	void setMIDIInputConnection(int sourceIndex);
	void removeMIDIInputConnection(int sourceIndex);
	void setAudioInputConnection(int sourceIndex, int busIndex);
	void removeAudioInputConnection(int sourceIndex);
	void addMIDIInstrumentConnection(int sourceIndex, int instrIndex);
	void removeMIDIInstrumentConnection(int sourceIndex, int instrIndex);
	void setMIDISendConnection(int sourceIndex);
	void removeMIDISendConnection(int sourceIndex);
	void setAudioSendConnection(int sourceIndex, int busIndex);
	void removeAudioSendConnection(int sourceIndex);
	void setAudioOutputConnection(int instrIndex, int busIndex);
	void removeAudioOutputConnection(int instrIndex);

	void setAudioLayout(const juce::AudioProcessorGraph::BusesLayout& busLayout);

	void removeIllegalAudioInputConnections();
	void removeIllegalAudioSendConnections();
	void removeIllegalAudioOutputConnections();

	void setInputChannels(const juce::Array<juce::AudioChannelSet>& channels);

	void addOutputBus();
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

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sequencer)
};

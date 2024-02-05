#pragma once

#include <JuceHeader.h>

#include "SourceList.h"
#include "../project/Serializable.h"

class SeqSourceProcessor final : public juce::AudioProcessor,
	public Serializable {
public:
	SeqSourceProcessor() = delete;
	SeqSourceProcessor(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());

	int addSeq(const SourceList::SeqBlock& block);
	void removeSeq(int index);
	int getSeqNum() const;
	const SourceList::SeqBlock getSeq(int index) const;

	void setTrackName(const juce::String& name);
	const juce::String getTrackName() const;
	void setTrackColor(const juce::Colour& color);
	const juce::Colour getTrackColor() const;

	const juce::AudioChannelSet& getAudioChannelSet() const;

	void closeAllNote();

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

	void clearGraph();

	class SafePointer {
	private:
		juce::WeakReference<SeqSourceProcessor> weakRef;

	public:
		SafePointer() = default;
		SafePointer(SeqSourceProcessor* source) : weakRef(source) {};
		SafePointer(const SafePointer& other) noexcept : weakRef(other.weakRef) {};

		SafePointer& operator= (const SafePointer& other) { weakRef = other.weakRef; return *this; };
		SafePointer& operator= (SeqSourceProcessor* newSource) { weakRef = newSource; return *this; };

		SeqSourceProcessor* getSourceSeq() const noexcept { return dynamic_cast<SeqSourceProcessor*> (weakRef.get()); };
		operator SeqSourceProcessor* () const noexcept { return getSourceSeq(); };
		SeqSourceProcessor* operator->() const noexcept { return getSourceSeq(); };
		void deleteAndZero() { delete getSourceSeq(); };

		bool operator== (SeqSourceProcessor* component) const noexcept { return weakRef == component; };
		bool operator!= (SeqSourceProcessor* component) const noexcept { return weakRef != component; };
	};

public:
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<google::protobuf::Message> serialize() const override;

private:
	juce::AudioChannelSet audioChannels;
	SourceList srcs;
	std::set<std::tuple<int, int>> activeNoteSet;
	std::atomic_bool noteCloseFlag = false;

	juce::String trackName;
	juce::Colour trackColor;

	JUCE_DECLARE_WEAK_REFERENCEABLE(SeqSourceProcessor)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqSourceProcessor)
};

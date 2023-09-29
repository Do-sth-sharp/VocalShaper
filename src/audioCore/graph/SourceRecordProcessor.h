#pragma once

#include <JuceHeader.h>

#include "../source/CloneableSource.h"
#include "../project/Serializable.h"

class SourceRecordProcessor final : public juce::AudioProcessor,
	public Serializable {
public:
	SourceRecordProcessor();
	~SourceRecordProcessor();

	void addTask(CloneableSource::SafePointer<> source, double offset);
	void removeTask(int index);
	int getTaskNum() const;
	std::tuple<CloneableSource::SafePointer<>, double> getTask(int index) const;

public:
	const juce::String getName() const override { return "Source Recorder"; };

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

public:
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<const google::protobuf::Message> serialize() const override;

private:
	using RecorderTask = std::tuple<CloneableSource::SafePointer<>, double>;
	juce::Array<RecorderTask> tasks;
	juce::ReadWriteLock taskLock;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceRecordProcessor)
};

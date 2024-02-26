#pragma once

#include <JuceHeader.h>

#include "../project/Serializable.h"
#include "../uiCallback/LimitedCall.h"

class SourceRecordProcessor final : public juce::AudioProcessor {
public:
	SourceRecordProcessor();
	~SourceRecordProcessor();

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

	class SafePointer {
	private:
		juce::WeakReference<SourceRecordProcessor> weakRef;

	public:
		SafePointer() = default;
		SafePointer(SourceRecordProcessor* source) : weakRef(source) {};
		SafePointer(const SafePointer& other) noexcept : weakRef(other.weakRef) {};

		SafePointer& operator= (const SafePointer& other) { weakRef = other.weakRef; return *this; };
		SafePointer& operator= (SourceRecordProcessor* newSource) { weakRef = newSource; return *this; };

		SourceRecordProcessor* getRecorder() const noexcept { return dynamic_cast<SourceRecordProcessor*> (weakRef.get()); };
		operator SourceRecordProcessor* () const noexcept { return getRecorder(); };
		SourceRecordProcessor* operator->() const noexcept { return getRecorder(); };
		void deleteAndZero() { delete getRecorder(); };

		bool operator== (SourceRecordProcessor* component) const noexcept { return weakRef == component; };
		bool operator!= (SourceRecordProcessor* component) const noexcept { return weakRef != component; };
	};

private:
	LimitedCall limitedCall;

	JUCE_DECLARE_WEAK_REFERENCEABLE(SourceRecordProcessor)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceRecordProcessor)
};

#pragma once

#include <JuceHeader.h>

class PluginDecorator final : public juce::AudioProcessor {
public:
	PluginDecorator() = delete;
	explicit PluginDecorator(std::unique_ptr<juce::AudioPluginInstance> plugin);

	bool canPluginAddBus(bool isInput) const;
	bool canPluginRemoveBus(bool isInput) const;

	void setMIDIChannel(int channel);
	int getMIDIChannel() const;

	const juce::Array<juce::AudioProcessorParameter*>& getPluginParamList() const;
	/*template<typename T>
	T getParamValue(int index) const;
	template<typename T>
	T getParamValue(const juce::String& name) const;
	template<typename T>
	void setParamValue(int index, T value);
	template<typename T>
	void setParamValue(const juce::String& name, T value);*/

public:
	const juce::String getName() const override;
	juce::StringArray getAlternateDisplayNames() const override;

	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	void releaseResources() override;
	void memoryWarningReceived() override;
	void processBlock(
		juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
	void processBlock(
		juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override;
	void processBlockBypassed(
		juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
	void processBlockBypassed(
		juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override;

	bool canAddBus(bool isInput) const override;
	bool canRemoveBus(bool isInput) const override;

	bool supportsDoublePrecisionProcessing() const override;
	double getTailLengthSeconds() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool supportsMPE() const override;
	bool isMidiEffect() const override;

	void reset() override;

	juce::AudioProcessorParameter* getBypassParameter() const override;
	void setNonRealtime(bool isNonRealtime) noexcept override;

	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	void refreshParameterList() override;

	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;
	void setCurrentProgramStateInformation(const void* data, int sizeInBytes) override;

	void processorLayoutsChanged() override;

	void addListener(juce::AudioProcessorListener* newListener) override;
	void removeListener(juce::AudioProcessorListener* listenerToRemove) override;

	void setPlayHead(juce::AudioPlayHead* newPlayHead) override;

	juce::int32 getAAXPluginIDForMainBusConfig(
		const juce::AudioChannelSet& mainInputLayout,
		const juce::AudioChannelSet& mainOutputLayout,
		bool idForAudioSuite) const override;

	juce::AudioProcessor::CurveData getResponseCurve(
		juce::AudioProcessor::CurveData::Type) const override;
	void updateTrackProperties(
		const juce::AudioProcessor::TrackProperties& properties) override;

private:
	std::unique_ptr<juce::AudioPluginInstance> plugin = nullptr;
	bool initFlag = false;
	std::atomic_int midiChannel = 1;

	void numChannelsChanged() override;
	void numBusesChanged() override;

	void updatePluginBuses();
	void syncBusesFromPlugin();
	void syncBusesNumFromPlugin();

	static void filterMIDIMessage(int channel, juce::MidiBuffer& midiMessages);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginDecorator)
};

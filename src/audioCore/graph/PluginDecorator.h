﻿#pragma once

#include <JuceHeader.h>
#include "../project/Serializable.h"
#include "../ara/ARAVirtualDocument.h"

class SeqSourceProcessor;

class PluginDecorator final : public juce::AudioProcessor,
	public Serializable {
public:
	PluginDecorator() = delete;
	PluginDecorator(SeqSourceProcessor* seq, bool isInstr = false,
		const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	explicit PluginDecorator(std::unique_ptr<juce::AudioPluginInstance> plugin,
		const juce::String& identifier, SeqSourceProcessor* seq,
		bool isInstr = false,
		const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	~PluginDecorator();

	using SetPluginCallback = std::function<void(void)>;
	void setPlugin(
		std::unique_ptr<juce::AudioPluginInstance> plugin,
		const juce::String& pluginIdentifier, SetPluginCallback callback, bool hasARA = false);
	void setARA(
		juce::ARAFactoryWrapper factory, const juce::String& pluginIdentifier,
		SetPluginCallback callback);
	void handleARALoadError(
		const juce::String& pluginIdentifier,
		SetPluginCallback callback);

	bool isARAValid() const;

	void createARADataID();
	const juce::String getARADataID() const;
	void loadARADataFrom(const juce::String& araDir, const juce::String& id);
	void saveARAData(const juce::String& araDir) const;

	const juce::AudioChannelSet& getAudioChannelSet() const;
	const juce::String getPluginIdentifier() const;

	bool canPluginAddBus(bool isInput) const;
	bool canPluginRemoveBus(bool isInput) const;

	void setMIDIChannel(int channel);
	int getMIDIChannel() const;

	const juce::Array<juce::AudioProcessorParameter*>& getPluginParamList() const;
	const juce::StringArray getParamNameList() const;
	const juce::String getParamName(int index) const;
	float getParamValue(int index) const;
	float getParamDefaultValue(int index) const;
	void setParamValue(int index, float value);

	void connectParamCC(int paramIndex, int CCIndex);
	int getCCParamConnection(int CCIndex) const;
	int getParamCCConnection(int paramIndex) const;
	void removeCCParamConnection(int CCIndex);
	//void setParamCCListenning(int paramIndex);

	void setMIDICCIntercept(bool midiCCShouldIntercept);
	bool getMIDICCIntercept() const;

	void setMIDIOutput(bool midiShouldOutput);
	bool getMIDIOutput() const;

	using MIDICCListener = std::function<void(int)>;
	void setMIDICCListener(const MIDICCListener& listener);
	void clearMIDICCListener();

	void invokeARADocumentRegionChange();
	void invokeARADocumentContextChange();
	void invokeARADocumentTrackInfoChange();

	class SafePointer {
	private:
		juce::WeakReference<PluginDecorator> weakRef;

	public:
		SafePointer() = default;
		SafePointer(PluginDecorator* source) : weakRef(source) {};
		SafePointer(const SafePointer& other) noexcept : weakRef(other.weakRef) {};

		SafePointer& operator= (const SafePointer& other) { weakRef = other.weakRef; return *this; };
		SafePointer& operator= (PluginDecorator* newSource) { weakRef = newSource; return *this; };

		PluginDecorator* getPlugin() const noexcept { return dynamic_cast<PluginDecorator*> (weakRef.get()); };
		operator PluginDecorator* () const noexcept { return getPlugin(); };
		PluginDecorator* operator->() const noexcept { return getPlugin(); };
		void deleteAndZero() { delete getPlugin(); };

		bool operator== (PluginDecorator* component) const noexcept { return weakRef == component; };
		bool operator!= (PluginDecorator* component) const noexcept { return weakRef != component; };
	};

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

	juce::AudioProcessor::CurveData getResponseCurve(
		juce::AudioProcessor::CurveData::Type) const override;
	void updateTrackProperties(
		const juce::AudioProcessor::TrackProperties& properties) override;

public:
	bool parse(
		const google::protobuf::Message* data,
		const ParseConfig& config) override;
	std::unique_ptr<google::protobuf::Message> serialize(
		const SerializeConfig& config) const override;

private:
	SeqSourceProcessor* const seq;

	std::unique_ptr<juce::AudioPluginInstance> plugin = nullptr;
	juce::String pluginIdentifier;
	std::unique_ptr<juce::AudioBuffer<float>> buffer = nullptr;
	std::unique_ptr<juce::AudioBuffer<double>> doubleBuffer = nullptr;
	std::atomic_int midiChannel = 1;
	std::array<std::atomic_int, 128> paramCCList;
	//std::atomic_int paramListenningCC = -1;
	std::atomic_bool midiShouldOutput = false;
	std::atomic_bool midiCCShouldIntercept = true;
	const juce::AudioChannelSet audioChannels;
	const bool isInstr = false;
	std::atomic_bool pluginPrepared = false;

	MIDICCListener ccListener;

	std::unique_ptr<juce::ARAHostDocumentController> araDocumentController = nullptr;
	juce::ARAHostModel::EditorRendererInterface araEditorRenderer;
	juce::ARAHostModel::PlaybackRendererInterface araPlaybackRenderer;
	std::unique_ptr<ARAVirtualDocument> araVirtualDocument = nullptr;
	std::unique_ptr<juce::ChangeBroadcaster> araRegionChangeBroadcaster = nullptr;
	std::unique_ptr<juce::ChangeBroadcaster> araContextChangeBroadcaster = nullptr;
	std::unique_ptr<juce::ChangeBroadcaster> araTrackInfoChangeBroadcaster = nullptr;

	juce::String araDataID;

	int pluginOnOffCount = 0;
	juce::SpinLock pluginOnOffMutex;

	static void filterMIDIMessage(int channel, juce::MidiBuffer& midiMessages);
	static void interceptMIDIMessage(bool shouldMIDIOutput, juce::MidiBuffer& midiMessages);
	static void interceptMIDICCMessage(bool shouldMIDICCIntercept, juce::MidiBuffer& midiMessages);

	void parseMIDICC(juce::MidiBuffer& midiMessages);
	
	void updateBuffer();

	void pluginOnOffInternal(
		bool shouldOn, double sampleRate, int blockSize,
		bool stateQuickSwitch = false);

	void updatePluginBuses();

	JUCE_DECLARE_WEAK_REFERENCEABLE(PluginDecorator)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginDecorator)
};

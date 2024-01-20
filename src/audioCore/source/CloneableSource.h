#pragma once

#include <JuceHeader.h>
#include "../project/Serializable.h"

class CloneableSourceManager;
class CloneableSource;
template<typename T>
concept IsCloneable = std::derived_from<T, CloneableSource>;

class SynthRenderThread;

class CloneableSource : public Serializable {
public:
	CloneableSource(const juce::String& name = juce::String{});
	CloneableSource(int id, const juce::String& name = juce::String{});
	virtual ~CloneableSource() = default;

	std::unique_ptr<CloneableSource> cloneThis() const;
	bool cloneAs(CloneableSource* dst) const;
	void initThis(double sampleRate = 48000, int channelNum = 2, int sampleNum = 0);
	bool loadFrom(const juce::File& file);
	bool saveAs(const juce::File& file) const;
	double getSourceLength() const;

	bool checkSaved() const;
	void changed();

	int getId() const;
	void setName(const juce::String& name);
	const juce::String getName() const;
	void setPath(const juce::String& path);
	const juce::String getPath() const;

	void prepareToPlay(double sampleRate, int bufferSize);
	double getSampleRate() const;
	int getBufferSize() const;

	template<IsCloneable SourceType = CloneableSource>
	class SafePointer {
	public:
		SafePointer() = default;
		SafePointer(SourceType* source) : weakRef(source) {};
		SafePointer(const SafePointer& other) noexcept : weakRef(other.weakRef) {};

		SafePointer& operator= (const SafePointer& other) { weakRef = other.weakRef; return *this; };
		SafePointer& operator= (SourceType* newSource) { weakRef = newSource; return *this; };

		SourceType* getSource() const noexcept { return dynamic_cast<SourceType*> (weakRef.get()); };
		operator SourceType* () const noexcept { return getSource(); };
		SourceType* operator->() const noexcept { return getSource(); };
		void deleteAndZero() { delete getSource(); };

		bool operator== (SourceType* component) const noexcept { return weakRef == component; };
		bool operator!= (SourceType* component) const noexcept { return weakRef != component; };

	private:
		juce::WeakReference<CloneableSource> weakRef;
	};

	void setSynthesizer(
		std::unique_ptr<juce::AudioPluginInstance> synthesizer,
		const juce::String& identifier);
	const juce::String getSynthesizerName() const;
	bool isSynthRunning() const;
	void stopSynth();
	void synth();

	void setDstSource(CloneableSource::SafePointer<> dst);
	CloneableSource::SafePointer<> getDstSource() const;

	virtual double getSourceSampleRate() const { return 0; };
	virtual int getChannelNum() const { return 0; };
	virtual int getTrackNum() const { return 0; };

	class SourceRenderHelperBase {
	public:
		SourceRenderHelperBase() = default;
		virtual ~SourceRenderHelperBase() = default;
		virtual SynthRenderThread* getImpl() const = 0;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceRenderHelperBase)
	};

private:
	virtual juce::MidiFile* getMidiContentPtr() { return nullptr; };
	virtual juce::AudioSampleBuffer* getAudioContentPtr() { return nullptr; };

public:
	void setSynthesizerState(const juce::MemoryBlock& state);
	const juce::MemoryBlock getSynthesizerState() const;

private:
	friend class CloneableSourceManager;
	static void resetIdCounter();
	void setId(int id);

private:
	friend class SourceRecordProcessor;
	void prepareToRecordInternal(
		int inputChannels, double sampleRate,
		int blockSize, bool updateOnly = false);
	void recordingFinishedInternal();

public:
	bool checkRecording() const;

protected:
	virtual void prepareToRecord(
		int inputChannels, double sampleRate,
		int blockSize, bool updateOnly) {};
	virtual void recordingFinished() {};

protected:
	virtual std::unique_ptr<CloneableSource> createThisType() const = 0;
	virtual bool clone(CloneableSource* dst) const = 0;
	virtual bool load(const juce::File& file) = 0;
	virtual bool save(const juce::File& file) const = 0;
	virtual double getLength() const = 0;
	virtual void sampleRateChanged() {};
	virtual void init(double sampleRate, int channelNum, int sampleNum) {};

private:
	static std::atomic_int globalCounter;
	mutable std::atomic_bool isSaved = true;
	int id = -1;
	juce::String name;
	juce::String path;
	std::atomic<double> currentSampleRate = 0;
	std::atomic_int currentBufferSize = 0;
	std::atomic_bool isRecording = false;

	friend class SynthRenderThread;
	std::unique_ptr<SourceRenderHelperBase> synthThread = nullptr;
	std::unique_ptr<juce::AudioPluginInstance> synthesizer = nullptr;
	juce::String pluginIdentifier;

	JUCE_DECLARE_WEAK_REFERENCEABLE(CloneableSource)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableSource)
};

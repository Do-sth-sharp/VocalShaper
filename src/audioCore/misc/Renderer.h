#pragma once

#include <JuceHeader.h>

class Track;

class Renderer final : private juce::DeletedAtShutdown {
public:
	Renderer();
	~Renderer() override;

	void setRendering(bool rendering);
	bool getRendering() const;

	using RenderTask = std::tuple<const Track*, int, juce::AudioChannelSet>;
	using RenderTaskList = juce::Array<RenderTask>;

	void updateSampleRateAndBufferSize(double sampleRate, int bufferSize);
	void prepareToRender(const RenderTaskList& tasks);
	void saveFile(const juce::File& dir,
		const juce::String& name, const juce::String& extension);
	void releaseBuffer();

private:
	friend class Track;
	void writeData(const Track* trackPtr,
		const juce::AudioBuffer<float>& buffer, int offset);

private:
	std::atomic_bool rendering = false;
	juce::CriticalSection lock;
	double sampleRate = 0;
	int bufferSize = 0;
	std::map<const Track*, std::tuple<
		int, juce::AudioChannelSet, juce::AudioBuffer<float>>> buffers;
	std::unique_ptr<juce::Thread> renderThread = nullptr;

public:
	static Renderer* getInstance();
	static void releaseInstance();

private:
	static Renderer* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Renderer)
};

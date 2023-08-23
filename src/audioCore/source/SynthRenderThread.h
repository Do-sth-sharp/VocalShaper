#pragma once
#include <JuceHeader.h>

class CloneableSynthSource;

class SynthRenderThread final : public juce::Thread {
public:
	SynthRenderThread() = delete;
	explicit SynthRenderThread(CloneableSynthSource* parent);
	~SynthRenderThread();

private:
	void run() override;

private:
	CloneableSynthSource* parent = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthRenderThread)
};

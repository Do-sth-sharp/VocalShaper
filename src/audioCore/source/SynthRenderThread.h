#pragma once
#include <JuceHeader.h>
#include "CloneableSource.h"

class SynthRenderThread final : public juce::Thread {
public:
	SynthRenderThread() = delete;
	explicit SynthRenderThread(CloneableSource* src);
	~SynthRenderThread();

	void setDstSource(CloneableSource::SafePointer<> dst);
	CloneableSource::SafePointer<> getDstSource() const;

private:
	void run() override;

private:
	CloneableSource* src = nullptr;
	CloneableSource::SafePointer<> dst = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthRenderThread)
};

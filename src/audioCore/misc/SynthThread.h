#pragma once

#include <JuceHeader.h>

class SeqSourceProcessor;

class SynthThread final : public juce::Thread {
public:
	SynthThread() = default;
	SynthThread(SeqSourceProcessor* parent);
	~SynthThread();

	void synthNow();

protected:
	void run() override;

private:
	SeqSourceProcessor* const parent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthThread)
};

#pragma once

#include <JuceHeader.h>

class ARAGlobalState final {
	ARAGlobalState() = delete;

public:
	static void startSourceAnalysising(ARA::ARAAudioSourceHostRef audioSourceHostRef);
	static void completeSourceAnalysising(ARA::ARAAudioSourceHostRef audioSourceHostRef);
	static bool hasSourceAnalysising();

private:
	static std::unordered_set<ARA::ARAAudioSourceHostRef> analysisingSources;
	static juce::ReadWriteLock analysisingSourcesLock;
};

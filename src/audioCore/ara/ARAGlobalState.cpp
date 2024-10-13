#include "ARAGlobalState.h"

std::unordered_set<ARA::ARAAudioSourceHostRef> ARAGlobalState::analysisingSources;
juce::ReadWriteLock ARAGlobalState::analysisingSourcesLock;

void ARAGlobalState::startSourceAnalysising(
	ARA::ARAAudioSourceHostRef audioSourceHostRef) {
	juce::ScopedWriteLock locker(ARAGlobalState::analysisingSourcesLock);
	ARAGlobalState::analysisingSources.insert(audioSourceHostRef);
}

void ARAGlobalState::completeSourceAnalysising(
	ARA::ARAAudioSourceHostRef audioSourceHostRef) {
	juce::ScopedWriteLock locker(ARAGlobalState::analysisingSourcesLock);
	ARAGlobalState::analysisingSources.erase(audioSourceHostRef);
}

bool ARAGlobalState::hasSourceAnalysising() {
	juce::ScopedReadLock locker(ARAGlobalState::analysisingSourcesLock);
	return (ARAGlobalState::analysisingSources.size() > 0);
}

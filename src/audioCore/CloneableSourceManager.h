#pragma once

#include <JuceHeader.h>
#include "CloneableSource.h"

class CloneableSourceManager final : private juce::DeletedAtShutdown {
public:
	CloneableSourceManager() = default;
	~CloneableSourceManager() override = default;

	/**
	* @attention	You must call this from message thread.
	*/
	bool addSource(std::unique_ptr<CloneableSource> src);
	/**
	* @attention	You must call this from message thread.
	*/
	bool removeSource(CloneableSource* src);
	/**
	* @attention	You must call this from message thread.
	*/
	bool removeSource(int index);
	CloneableSource::SafePointer<> getSource(int index) const;
	int getSourceNum() const;
	const juce::CriticalSection& getLock() const;

private:
	juce::OwnedArray<CloneableSource, juce::CriticalSection> sourceList;

public:
	static CloneableSourceManager* getInstance();
	static void releaseInstance();

private:
	static CloneableSourceManager* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableSourceManager)
};

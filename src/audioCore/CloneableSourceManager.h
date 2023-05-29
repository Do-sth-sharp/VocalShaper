#pragma once

#include <JuceHeader.h>
#include "CloneableSource.h"

class CloneableSourceManager final : public juce::DeletedAtShutdown {
public:
	CloneableSourceManager() = default;
	~CloneableSourceManager() override = default;

	void addSource(std::unique_ptr<CloneableSource> src);
	void removeSource(CloneableSource* src);
	void removeSource(int index);
	CloneableSource::SafePointer<> getSource(int index) const;
	int getSourceNum() const;

private:
	juce::OwnedArray<CloneableSource> sourceList;

public:
	static CloneableSourceManager* getInstance();
	static void releaseInstance();

private:
	static CloneableSourceManager* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableSourceManager)
};

#pragma once

#include <JuceHeader.h>
#include "SourceInternalContainer.h"

class SourceInternalPool final
	: private juce::DeletedAtShutdown {
public:
	SourceInternalPool() = default;

	std::shared_ptr<SourceInternalContainer> add(const juce::String& name,
		SourceInternalContainer::SourceType type);
	std::shared_ptr<SourceInternalContainer> find(const juce::String& name) const;
	std::shared_ptr<SourceInternalContainer> fork(const juce::String& name);
	void checkSourceReleased(const juce::String& name);

private:
	std::unordered_map<juce::String, std::shared_ptr<SourceInternalContainer>> list;
	uint64_t newSourceCount = 0;

public:
	static SourceInternalPool* getInstance();
	static void releaseInstance();

private:
	static SourceInternalPool* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceInternalPool)
};

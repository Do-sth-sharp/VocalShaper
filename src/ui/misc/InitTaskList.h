#pragma once

#include <JuceHeader.h>

class InitTaskList final : private juce::DeletedAtShutdown {
public:
	InitTaskList() = default;

	using InitTask = std::function<void(void)>;
	void add(const InitTask& task);
	void runNow();

private:
	juce::Array<InitTask> tasks;

public:
	static InitTaskList* getInstance();
	static void releaseInstance();

private:
	static InitTaskList* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InitTaskList)
};

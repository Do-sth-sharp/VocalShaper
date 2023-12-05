#pragma once

#include <JuceHeader.h>

class MainThreadPool final : private juce::DeletedAtShutdown {
public:
	MainThreadPool();
	~MainThreadPool();

	void stopAll();
	using Job = std::function<void(void)>;
	void runJob(const Job& job);

private:
	std::unique_ptr<juce::ThreadPool> pool = nullptr;

public:
	static MainThreadPool* getInstance();
	static void releaseInstance();

private:
	static MainThreadPool* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainThreadPool)
};

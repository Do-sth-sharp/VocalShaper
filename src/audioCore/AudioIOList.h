#pragma once

#include <JuceHeader.h>

class AudioIOList final : public juce::Thread,
	private juce::DeletedAtShutdown {
public:
	AudioIOList();
	~AudioIOList();

	void load(int index, juce::String path);
	void save(int index, juce::String path);
	bool isTask(int index) const;

private:
	void run() override;

private:
	using IOTask = std::tuple<bool, int, juce::String>;
	mutable std::queue<IOTask> list;
	juce::CriticalSection lock;
	std::atomic_int currentIndex = -1;

public:
	static AudioIOList* getInstance();
	static void releaseInstance();

private:
	static AudioIOList* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioIOList)
};

#define AUDIOCORE_ENSURE_IO_NOT_RUNNING(ret) \
	do { \
		JUCE_ASSERT_MESSAGE_THREAD; \
		if (AudioIOList::getInstance()->isThreadRunning()) { return ret; } \
	} while (false)

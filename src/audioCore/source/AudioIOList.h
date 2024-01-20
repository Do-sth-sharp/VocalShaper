#pragma once

#include <JuceHeader.h>

#include "CloneableSource.h"

class AudioIOList final : public juce::Thread,
	private juce::DeletedAtShutdown {
public:
	AudioIOList();
	~AudioIOList();

	void load(CloneableSource::SafePointer<> ptr, juce::String path, bool copy);
	void save(CloneableSource::SafePointer<> ptr, juce::String path);
	void clone(CloneableSource::SafePointer<> src, CloneableSource::SafePointer<> dst);
	bool isTask(CloneableSource::SafePointer<> ptr) const;

private:
	void run() override;

private:
	enum class TaskType { Load, Save, CopyLoad, Clone };
	using IOTask = std::tuple<TaskType, CloneableSource::SafePointer<>, CloneableSource::SafePointer<>, juce::String>;
	mutable std::queue<IOTask> list;
	juce::CriticalSection lock;
	std::atomic<CloneableSource*> currentTask = nullptr;

public:
	static AudioIOList* getInstance();
	static void releaseInstance();

private:
	static AudioIOList* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioIOList)
};

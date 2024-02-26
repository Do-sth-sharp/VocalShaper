#pragma once

#include <JuceHeader.h>
#include "../graph/SeqSourceProcessor.h"

class SourceIO final : public juce::Thread,
	private juce::DeletedAtShutdown {
public:
	SourceIO();
	~SourceIO();

	enum class TaskType {
		Read, Write
	};
	/** Type, SeqPtr, Path */
	using Task = std::tuple<TaskType, SeqSourceProcessor::SafePointer, juce::String>;
	void addTask(const Task& task);

protected:
	void run() override;

private:
	const juce::StringArray audioFormatsIn, midiFormatsIn;
	const juce::StringArray audioFormatsOut, midiFormatsOut;

	juce::CriticalSection lock;
	std::queue<Task> list;

	static const juce::StringArray trimFormat(const juce::StringArray& list);

public:
	static SourceIO* getInstance();
	static void releaseInstance();

private:
	static SourceIO* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceIO)
};

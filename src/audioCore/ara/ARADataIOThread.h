#pragma once

#include <JuceHeader.h>
#include "ARAVirtualDocument.h"

class ARADataIOThread final
	: public juce::Thread,
	private juce::DeletedAtShutdown {
public:
	ARADataIOThread();
	~ARADataIOThread();

	using DstPointer = ARAVirtualDocument::SafePointer;
	void addTask(const juce::String& path, DstPointer document, bool isWrite);

protected:
	void run() override;

private:
	/** Path, Document Pointer, Is Write */
	using ARADataIOTask = std::tuple<juce::String, DstPointer, bool, juce::MemoryBlock>;
	std::queue<ARADataIOTask> list;
	juce::CriticalSection lock;

	static bool writeToFile(const juce::MemoryBlock& data, const juce::String& path);
	static bool readFromFile(juce::MemoryBlock& data, const juce::String& path);

public:
	static ARADataIOThread* getInstance();
	static void releaseInstance();

private:
	static ARADataIOThread* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARADataIOThread)
};

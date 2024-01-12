#pragma once

#include <JuceHeader.h>
#include "ActionBase.h"

class ActionDispatcher final
	: private juce::DeletedAtShutdown {
public:
	ActionDispatcher();
	~ActionDispatcher();

	const juce::UndoManager& getActionManager() const;
	bool dispatch(std::unique_ptr<ActionBase> action);
	void clearUndoList();
	bool performUndo();
	bool performRedo();

	using OutputCallback = std::function<void(const juce::String&)>;
	using ErrorCallback = std::function<void(const juce::String&)>;
	void setOutput(const OutputCallback& output, const ErrorCallback& error);
	void removeOutput();

private:
	friend class ActionBase;
	void outputInternal(const juce::String& mes);
	void errorInternal(const juce::String& mes);

private:
	std::unique_ptr<juce::UndoManager> manager = nullptr;
	OutputCallback output = [](const juce::String&) {};
	ErrorCallback error = [](const juce::String&) {};

public:
	static ActionDispatcher* getInstance();
	static void releaseInstance();

private:
	static ActionDispatcher* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ActionDispatcher)
};

#pragma once

#include <JuceHeader.h>
#include "ActionBase.h"

class ActionDispatcher final
	: private juce::DeletedAtShutdown {
public:
	ActionDispatcher();

	const juce::UndoManager& getActionManager() const;
	bool dispatch(std::unique_ptr<ActionBase> action);

	using OutputCallback = std::function<void(const juce::String&)>;
	void setOutput(const OutputCallback& callback);
	void removeOutput();

private:
	friend class ActionBase;
	void outputInternal(const juce::String& mes);

private:
	std::unique_ptr<juce::UndoManager> manager = nullptr;
	OutputCallback output = [](const juce::String&) {};

public:
	static ActionDispatcher* getInstance();
	static void releaseInstance();

private:
	static ActionDispatcher* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ActionDispatcher)
};

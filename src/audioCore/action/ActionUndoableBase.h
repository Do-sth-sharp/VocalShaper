#pragma once

#include <JuceHeader.h>
#include "ActionBase.h"

class ActionUndoableBase : public ActionBase,
	public juce::UndoableAction {
public:
	ActionUndoableBase() = default;
	virtual ~ActionUndoableBase() = default;

public:
	bool perform() override;

private:
	JUCE_LEAK_DETECTOR(ActionUndoableBase)
};

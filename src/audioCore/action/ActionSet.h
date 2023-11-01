#pragma once

#include "ActionUndoableBase.h"
#include "../Utils.h"

class ActionSetDeviceAudioType final : public ActionBase {
public:
	ActionSetDeviceAudioType() = delete;
	ActionSetDeviceAudioType(const juce::String& type);

	bool doAction() override;

private:
	const juce::String type;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioType)
};

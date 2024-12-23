#pragma once

#include <JuceHeader.h>
#include "../recovery/ActionType.hpp"

class ActionUndoableBase : public juce::UndoableAction {
public:
	ActionUndoableBase() = default;
	virtual ~ActionUndoableBase() = default;

	virtual const juce::String getName() const = 0;

	bool perform() override;
	bool undo() override;

protected:
	void output(const juce::String& mes);

	virtual bool doAction() = 0;
	virtual bool undoAction() = 0;
	virtual const juce::String getStatusStr() const = 0;

	enum ShieldType {
		ShieldOff = 0,
		ShieldRendering = 1 << 0,
		ShieldSourceIO = 1 << 1,
		ShieldPluginLoad = 1 << 2,
		ShieldPluginScan = 1 << 3,
		ShieldARAAnalysis = 1 << 4
	};
	virtual int getShieldMask() const { return ShieldRendering; };
	virtual bool getProjectChange() const { return true; };
	virtual ActionType getActionType() const { return ActionType::ActionNone; };
	
	virtual void getRecoveryData(juce::MemoryOutputStream& stream) {};

private:
	juce::String outputTemp;

	bool checkShield();
	void writeRecovery(bool isUndo);
	void writeRecoveryResult(bool succeed);

	void outputInternal();
	void errorInternal();

	bool performInternal(bool isUndo);

	JUCE_LEAK_DETECTOR(ActionUndoableBase)
};

class ActionBase : public ActionUndoableBase {
public:
	ActionBase() = default;
	virtual ~ActionBase() = default;

protected:
	bool undoAction() final override { return false; };

	virtual int getShieldMask() const override { return ShieldOff; };
	virtual bool getProjectChange() const override { return false; };

private:
	JUCE_LEAK_DETECTOR(ActionBase)
};

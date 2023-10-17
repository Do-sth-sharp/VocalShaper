#include "ActionDispatcher.h"
#include "ActionUndoableBase.h"

ActionDispatcher::ActionDispatcher() {
	/** Undo Manager */
	this->manager = std::make_unique<juce::UndoManager>();
}

const juce::UndoManager& ActionDispatcher::getActionManager() const {
	return *(this->manager.get());
}

bool ActionDispatcher::dispatch(std::unique_ptr<ActionBase> action) {
	auto ptrAction = action.release();
	if (!ptrAction) { return false; }

	if (auto undoable = dynamic_cast<ActionUndoableBase*>(ptrAction)) {
		return this->manager->perform(undoable);
	}
	return ptrAction->doAction();
}

void ActionDispatcher::setOutput(const OutputCallback& callback) {
	this->output = callback;
}

void ActionDispatcher::removeOutput() {
	this->output = [](const juce::String&) {};
}

void ActionDispatcher::outputInternal(const juce::String& mes) {
	this->output(mes);
}

ActionDispatcher* ActionDispatcher::getInstance() {
	return ActionDispatcher::instance ? ActionDispatcher::instance : (ActionDispatcher::instance = new ActionDispatcher);
}

void ActionDispatcher::releaseInstance() {
	if (ActionDispatcher::instance) {
		delete ActionDispatcher::instance;
		ActionDispatcher::instance = nullptr;
	}
}

ActionDispatcher* ActionDispatcher::instance = nullptr;
